/** @file dist_grids.c
*  @brief Distributed MPI version of bruteforce in C
*  @author Pedro Abreu
*  @author João Borrego
*  @author Miguel Cardoso
*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++ INCLUDES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BUFFER_SIZE 200
#define INF 10000000
#define REMOVAL_PERIOD 5
#define ROOT 0
#define ALIVE 1
#define DEAD 0
#define debug_print(M, ...) printf("DEBUG: %s:%d:%s: " M "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define true 1
#define false 0
typedef unsigned char bool;
/*+++++++++++++++++++++++++++++++++++++++++++++++++ BLOCK MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BLOCK_LOW(rank,numprocs,size) 	((rank)*(size)/(numprocs))
#define BLOCK_HIGH(rank,numprocs,size) 	(BLOCK_LOW((rank)+1,numprocs,size)-1)
#define BLOCK_SIZE(rank,numprocs,size) 	(BLOCK_HIGH(rank,numprocs,size)-BLOCK_LOW(rank,numprocs,size)+1)
#define BLOCK_OWNER(element,numprocs,size) ((numprocs*(element+1)-1)/size)

/*+++++++++++++++++++++++++++++++++++++++++++++++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef struct _node{
    int z;
    int y;
    int x;
}node;

/** @brief Structure for storing a node of the graph */
typedef struct _graph_node{
    int z;                   /**< z coordinate, x and y are implicitly mapped */
    int state;                     /**< State of a node cell (DEAD or ALIVE) */
    unsigned char neighbours;       /**< Neighbour counter */
    struct _graph_node* next; /**< Pointer to the next entry in the list */
}graph_node;


void parseArgs(int argc, char* argv[], char** file, int* generations){
    if (argc == 3){
        char* file_name = malloc(sizeof(char) * (strlen(argv[1]) + 1));
        strcpy(file_name, argv[1]);
        *file = file_name;

        *generations = atoi(argv[2]);
        if (*generations > 0 && file_name != NULL)
        return;
    }
    printf("Usage: %s [data_file.in] [number_generations]", argv[0]);
    exit(EXIT_FAILURE);
}

graph_node* graphNodeInsert(graph_node* first, int z, int state){

    graph_node* new = (graph_node*) malloc(sizeof(graph_node));
    if (new == NULL){
        fprintf(stderr, "Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    new->z = z;
    new->state = state;
    new->neighbours = 0;
    new->next = first;
    return new;
}

graph_node*** initGraph(int size){

    int i,j;
    graph_node*** graph = (graph_node***) malloc(sizeof(graph_node**) * size);

    for (i = 0; i < size; i++){
        graph[i] = (graph_node**) malloc(sizeof(graph_node*) * size);
        for (j = 0; j < size; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

graph_node*** initLocalGraph(int bsize, int size){

    int i,j;
    graph_node*** graph = (graph_node***) malloc(sizeof(graph_node**) * bsize);

    for (i = 0; i < bsize; i++){
        graph[i] = (graph_node**) malloc(sizeof(graph_node*) * size);
        for (j = 0; j < size; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

bool graphNodeAddNeighbour(graph_node** first, int z){
    graph_node* it;
    /* Search for the node */
    for(it = *first; it != NULL; it = it->next){
        if (it->z == z){
            it->neighbours++;
            return false;
        }
    }

    /* Need to insert the node */
    graph_node* new = graphNodeInsert(*first, z, DEAD);
    new->neighbours++;
    *first = new;
    return true;
}

void visitNeighbours(graph_node*** graph, int cube_size, int x, int y, int z){

    graph_node* ptr;
    int x1, x2, y1, y2, z1, z2;
    x1 = (x+1)%cube_size; x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
    y1 = (y+1)%cube_size; y2 = (y-1) < 0 ? (cube_size-1) : (y-1);
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);
    /* If a cell is visited for the first time, add it to the update list, for fast access */
    graphNodeAddNeighbour(&(graph[x1][y]), z);
    graphNodeAddNeighbour(&(graph[x2][y]), z);
    graphNodeAddNeighbour(&(graph[x][y1]), z);
    graphNodeAddNeighbour(&(graph[x][y2]), z);
    graphNodeAddNeighbour(&(graph[x][y]), z1);
    graphNodeAddNeighbour(&(graph[x][y]), z2);
}

void graphListCleanup(graph_node** head){
    graph_node *temp, *prev;
    if(*head != NULL){
        temp = *head;
        /* Delete from the beginning */
        while(temp != NULL && temp->state == DEAD){
            *head = temp->next;
            free(temp);
            temp = *head;
        }
        /*Delete from the middle*/
        while(temp != NULL){
            while (temp != NULL && temp->state != DEAD){
                prev = temp;
                temp = temp->next;
            }
            if(temp == NULL)
            return;

            prev->next = temp->next;
            free(temp);
            temp = prev->next;
        }
    }
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MAIN +++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char **argv) {
    graph_node*** initial_global_graph;     /**< Graph representation - 2D array of lists */
    graph_node*** final_global_graph;

    graph_node*** local_graph; /**< Local Graph representation - partition of the total graph */
    graph_node* it;
    int nprocs = 	0, rank = 0; //Number of processes and rank of the process
    int size = 0;
    int i = 0, j = 0;
    int g;
    int cells_receive;
    int x,y,z;
    int generations; /**< Generations */
    int* sendcounts, *displs = NULL;;

    int local_graph_length;
    node* lg_send;
    int* lc_lengths, lg_displs;
    node* all_lg;

    /*Communication buffers*/
    node* receivebuffer;
    node* sendbuffer;

    /*Frontiers*/
    int low_number_amount, high_number_amount;
    node* sending_low_frontier;
    node* sending_high_frontier;
    node* receiving_low_frontier;
    node* receiving_high_frontier;

    int alive_nodes = 0;

    MPI_Init(&argc, &argv);
    /*MPI PREAMBLE*/
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //Assign rank and nprocess
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /*MPI Create struct*/
    MPI_Datatype 	MPI_CELL, MPI_CELL_t, struct_type[3] = {MPI_INT, MPI_INT, MPI_INT};
    int struct_b_len[3] = {1, 1, 1};
    MPI_Aint struct_extent, struct_lb, struct_displs[3] = 	{offsetof(node, x), offsetof(node, y), offsetof(node, z)};
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_CELL_t);

    MPI_Type_get_extent(MPI_CELL_t, &struct_lb, &struct_extent);
    MPI_Type_create_resized(MPI_CELL_t, -struct_lb, struct_extent, &MPI_CELL);
    MPI_Type_commit(&MPI_CELL);

    if(rank == ROOT){
        /***************************************************** RAN BY ROOT *********************************************************/
        /***************************************************** PARSE COMMAND LINE ARGUMENTS *********************************************************/
        char* file;   /**< Input data file name */
        parseArgs(argc, argv, &file, &generations);
        /***************************************************** OPEN FILE AND READ FIRST LINE *********************************************************/
        FILE* fp = fopen(file, "r");
        //free(file); //Free filename we no longer need it
        if(fp == NULL){
            fprintf(stderr, "Please input a valid file name\n" );
            exit(EXIT_FAILURE);
        }
        char buffer[BUFFER_SIZE] = {0};
        fgets(buffer,BUFFER_SIZE,fp);
        sscanf(buffer,"%d", &size);
        /************************************************** BROADCAST_SIZE **************************************************/
        MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        printf("@Rank %d - Received size: %d\n", rank, size); fflush(stdout);

        sendcounts 	= (int *) malloc(nprocs * sizeof(int));
        for(i = 0; i < nprocs; i++){
            sendcounts[i] = 0;
        }
        //Read the number of alive nodes and the sendcount value
        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                alive_nodes++;
                sendcounts[BLOCK_OWNER(x,nprocs,size)]++;
            }
        }
        displs = (int *) malloc(nprocs * sizeof(int));
        memset(displs, 0, nprocs * sizeof(int));
        printf("Alive nodes: %d\n", alive_nodes);fflush(stdout);
        for(i = 0; i < nprocs; i++){
            displs[i+1] = displs[i] + sendcounts[i];
        }

        for(i=0; i<nprocs; i++){
            printf("Root computing number of nodes proc %d: %d\n", nprocs, sendcounts[i]);fflush(stdout);
            printf("Root computing displacements: %d\n", displs[i]);
        }
        MPI_Scatter(sendcounts, 1, MPI_INT, &cells_receive, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        cells_receive = sendcounts[ROOT];
        printf("@Rank %d - Received their sendcount: %d\n", rank, cells_receive);fflush(stdout);
        //Create the global graph t
        initial_global_graph = initGraph(size);
        fclose(fp);fp = fopen(file, "r"); //Close and reopen file to reset fp
        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                /* Insert live nodes in the graph and the update set */
                initial_global_graph[x][y] = graphNodeInsert(initial_global_graph[x][y], z, ALIVE);
            }
        }
        int count=0;
        //Allocate an array with all the alive nodes
        sendbuffer = (node*) malloc(alive_nodes*sizeof(node));
        //From the global graph fill sendbuffer

        for(x=0; x < size; x++){
            for(y=0; y < size; y++){
                for(it = initial_global_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        //printf("%d %d %d\n", x, y, it->z);
                        //sendbuffer[count] = (node) malloc(sizeof(node));
                        sendbuffer[count].x = x;
                        sendbuffer[count].y = y;
                        sendbuffer[count].z = it->z;
                        count++;
                    }
                }
            }
        }
        printf("Counted %d\n", count);fflush(stdout);

        /* Convert the coordinates read into an array for sending */
        receivebuffer = (node *) calloc(cells_receive, sizeof(node));

        MPI_Scatterv(sendbuffer, sendcounts, displs, MPI_CELL, receivebuffer, cells_receive, MPI_CELL, ROOT, MPI_COMM_WORLD);

        for(int i=0; i<cells_receive; i++){
            receivebuffer[i].x = sendbuffer[i].x;
            receivebuffer[i].y = sendbuffer[i].y;
            receivebuffer[i].z = sendbuffer[i].z;
        }
        int low_frontier_count=0;
        int high_frontier_count=0;

        local_graph = initLocalGraph(BLOCK_SIZE(rank,nprocs,size), size);

        printf("Created local graph...\n");
        for(int i=0; i<cells_receive; i++){
            int x = receivebuffer[i].x - BLOCK_LOW(rank, nprocs, size);
            int y = receivebuffer[i].y;
            //printf("Seg %d %d\n", x, y);
        }



        for(g=0; g<generations; g++){

            //Compute frontier sizes (go over the local_graph at 0 and BLOCK_SIZE)
            low_frontier_count=0;
            for(y=0; y<size; y++){
                for(it = local_graph[0][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        low_frontier_count++;
                    }
                }
            }
            high_frontier_count=0;
            for(y=0; y<size; y++){
                for(it = local_graph[BLOCK_SIZE(rank,nprocs,size)-1][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        high_frontier_count++;
                    }
                }
            }
            printf("@Rank %d - LOW FRONTIER COUNT: %d, HIGH FRONTIER COUNT: %d\n", rank, low_frontier_count, high_frontier_count);


            //Alloc frontiers and copy nodes to it
            sending_low_frontier = (node *) calloc(low_frontier_count, sizeof(node));
            sending_high_frontier = (node *) calloc(high_frontier_count, sizeof(node));

            //Calculate neighbours for all (excluding in our side frontiers - can't add wrap arounds)
            for(x = 1; x < (BLOCK_SIZE(rank, nprocs, size) - 1); x++){
                for(y = 0; y < size; y++){
                    for(it = local_graph[x][y]; it != NULL; it = it->next){
                        if(it->state == ALIVE)
                        visitNeighbours(local_graph, size, x, y, it->z);
                    }
                }
            }
            //Add our nodes to our frontiers
            int low_frontier_size=0,high_frontier_size=0;
            for(int y=0; y<size; y++){
                for(it = local_graph[0][y]; it !=NULL; it = it->next){
                    if(it->state == ALIVE){
                        sending_low_frontier[low_frontier_size].x = 0;
                        sending_low_frontier[low_frontier_size].y = y;
                        sending_low_frontier[low_frontier_size].z = it->z;
                        low_frontier_size++;
                    }
                }
                for(it = local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it !=NULL; it = it->next){
                    if(it->state == ALIVE){
                        sending_low_frontier[high_frontier_size].x = 0;
                        sending_low_frontier[high_frontier_size].y = y;
                        sending_low_frontier[high_frontier_size].z = it->z;
                        high_frontier_size++;
                    }
                }
            }
            //Send our frontiers
            //To which ranks do we send?
            int low_rank = (rank-1) < 0 ? (nprocs-1) : (rank-1);
            int high_rank = (rank+1) >= nprocs ? (0) : (rank+1);
            printf("@Rank %d - Low rank: %d High rank: %d\n", rank, low_rank, high_rank);

            //Receive size of the other frontiers

            //printf("Sending %d to rank %d\n", low_frontier_size, low_rank);

            MPI_Send(sending_low_frontier, low_frontier_size, MPI_CELL, low_rank, 0, MPI_COMM_WORLD);
            MPI_Status s1;
            MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s1);
            MPI_Get_count(&s1, MPI_CELL, &low_number_amount);
            receiving_low_frontier = (node*)malloc(sizeof(node) * low_number_amount);
            MPI_Recv(receiving_low_frontier, low_number_amount, MPI_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            //printf("ROOT Sending to the right to rank %d\n", high_rank);

            //printf("ROOT Receiving from right...\n");
            MPI_Status s2;
            MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s2);
            MPI_Get_count(&s2, MPI_CELL, &high_number_amount);
            receiving_high_frontier = (node*)malloc(sizeof(node) * high_number_amount);
            MPI_Recv(receiving_high_frontier, high_number_amount, MPI_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(sending_high_frontier, high_frontier_size, MPI_CELL, high_rank, 0, MPI_COMM_WORLD);

            //We have all the frontiers here
            //Go over our side of the high frontier and take in consideration all those that are on the high frontier that was sent to us
            for(y=0; y < size; y++){
                for(it=local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 5 possible neighbours on our side of the frontier
                        int x2, y1, y2, z1, z2;
                        int z = it->z;
                        int x=(BLOCK_SIZE(rank, nprocs, size) - 1);
                        x2 = (x-1);
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);
                        /* If a cell is visited for the first time, add it to the update list, for fast access */
                        //graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x2][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                        //Check the possible neighbour on the frontier we just received (look for y and z)
                        for(i=0; i<high_number_amount; i++){
                            if(receiving_high_frontier[i].y == y && receiving_high_frontier[i].z == z){
                                it->neighbours++;
                            }
                        }
                    }
                }

                //Go over our side of the low frontier and take in consideration all those that are on the low frontier that was sent to us
                for(it=local_graph[0][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 5 possible neighbours on our side of the frontier
                        int x1, y1, y2, z1, z2;
                        int z = it->z;
                        int x = 0;
                        x1 = (x+1);
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);
                        /* If a cell is visited for the first time, add it to the update list, for fast access */
                        //graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                        //Check the possible neighbour on the frontier we just received (look for y and z)
                        for(i=0; i<low_number_amount; i++){
                            if(receiving_low_frontier[i].y == y && receiving_low_frontier[i].z == z){
                                it->neighbours++;
                            }
                        }
                    }
                }
            }

            //Compute the next state of all our nodes
            for(i = 0; i < BLOCK_SIZE(rank,nprocs,size); i++){
                for(j = 0; j < size; j++){
                    for (it = local_graph[i][j]; it != NULL; it = it->next){
                        int live_neighbours = it->neighbours;
                        it->neighbours = 0;
                        if(it->state == ALIVE){
                            if(live_neighbours < 2 || live_neighbours > 4){
                                it->state = DEAD;
                            }
                        }else{
                            if(live_neighbours == 2 || live_neighbours == 3){
                                it->state = ALIVE;
                            }
                        }
                    }
                }
            }
            //Free all frontiers this rank used and reset the frontier counters
            free(receivebuffer);
            free(sending_low_frontier);
            free(sending_high_frontier);
            free(receiving_low_frontier);
            free(receiving_high_frontier);
            //Do a cleanup (for eficiency)
            if(g % REMOVAL_PERIOD == 0){
                for(i = 0; i < BLOCK_SIZE(rank,nprocs,size); i++){
                    for(j = 0; j < size; j++){
                        graph_node ** list = &local_graph[i][j];
                        graphListCleanup(list);
                    }
                }
            }
        } //Generations loop end
        /********************************************POS-GENERATION-PROCESSING**************************************************/
        local_graph_length=0;
        //Generations ended. Copy your local_graph to an array
        for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
            for(y = 0; y < size; y++){
                for(it = local_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        local_graph_length++;
                    }
                }
            }
        }

        //Send the length of that array to ROOT
        lc_lengths = (int*)malloc(sizeof(int)*nprocs);
        lg_displs = (int*)malloc(sizeof(int)*nprocs);
        MPI_Gather(local_graph_length, 1, MPI_INT, lc_lengths, nprocs, MPI_INT, ROOT, MPI_COMM_WORLD);

        //Allocate our array and copy everything to it
        lg_send = (node*)malloc(sizeof(node) * local_graph_length);
        local_graph_length=0;
        for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
            for(y = 0; y < size; y++){
                for(it = local_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        lg_send[local_graph_length].x = x + BLOCK_LOW(rank, nprocs, size);
                        lg_send[local_graph_length].y = y;
                        lg_send[local_graph_length].z = it->z;
                        local_graph_length++;
                    }
                }
            }
        }
        int total_length = 0;
        for(i=0; i<nprocs; i++){
            lg_displs[i]=total_length;
            total_length+=lc_lengths[i];
        }
        all_lg = (node*)malloc(sizeof(node) * total_length);
        //Send the actual array to ROOT
        MPI_Gatherv(lg_send, local_graph_length, MPI_CELL, all_lg, total_length, lg_displs, ROOT, MPI_COMM_WORLD);
        //Transform lg_all to global graph
        initial_global_graph = initGraph(size);
        for(i=0; i<total_length; i++){
            x = all_lg[i].x;
            y = all_lg[i].y;
            z = all_lg[i].z;
            final_global_graph[x][y] = graphNodeInsert(initial_global_graph[x][y], z, ALIVE);
        }
        //Print to a file for checking

        fclose(fp);

    }else{
        /***************************************************** OTHER PROCS *********************************************************/
        MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        printf("Proc %d received size: %d\n", rank, size);fflush(stdout);

        MPI_Scatter(sendcounts, 1, MPI_INT, &cells_receive, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        printf("Proc %d received their sendcount: %d\n", rank, cells_receive);fflush(stdout);
        receivebuffer = (node *) calloc(cells_receive, sizeof(node)); //calloc instead of malloc to save a memset

        MPI_Scatterv(sendbuffer, sendcounts, displs, MPI_CELL, receivebuffer, cells_receive, MPI_CELL, ROOT, MPI_COMM_WORLD);
        int max_x = -INF;
        int min_x = INF;
        //Register minimum x found and maximum x found and output them so we can check there is no overlap between procs
        /*FOR DEBUG*/
        for(int i=0; i<cells_receive; i++){
            if(receivebuffer[i].x > max_x)
            max_x = receivebuffer[i].x;

            if(receivebuffer[i].x < min_x)
            min_x = receivebuffer[i].x;
        }

        printf("DEBUG -  @Rank %d - Maximum x found: %d Minimum x found: %d\n",rank, max_x, min_x);
        printf("BLOCK_LOW: %d, BLOCK_HIGH: %d, BLOCK_SIZE: %d\n", BLOCK_LOW(rank,nprocs,size), BLOCK_HIGH(rank,nprocs,size), BLOCK_SIZE(rank,nprocs,size));

        local_graph = initLocalGraph(BLOCK_SIZE(rank,nprocs,size), size);
        printf("Created local graph...\n");

        int low_frontier_count=0;
        int high_frontier_count=0;
        for(int i=0; i<cells_receive; i++){
            int x = receivebuffer[i].x - BLOCK_LOW(rank, nprocs, size);
            int y = receivebuffer[i].y;
            //printf("Seg %d %d\n", x, y);
            local_graph[x][y] = graphNodeInsert(local_graph[x][y], receivebuffer[i].z, ALIVE);
        }
        for(g=0; g<generations; g++){
            //Compute size of the frontiers
            //Compute frontier sizes (go over the local_graph at 0 and BLOCK_SIZE)
            low_frontier_count=0;
            for(y=0; y<size; y++){
                for(it = local_graph[0][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        low_frontier_count++;
                    }
                }
            }
            high_frontier_count=0;
            for(y=0; y<size; y++){
                for(it = local_graph[BLOCK_SIZE(rank,nprocs,size)-1][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        high_frontier_count++;
                    }
                }
            }

            printf("@Rank %d - LOW FRONTIER COUNT: %d, HIGH FRONTIER COUNT: %d\n", rank, low_frontier_count, high_frontier_count);
            //Go over the local graph and compute the number to assign to each frontier

            //Alloc frontiers and copy nodes to it
            sending_low_frontier = (node *) malloc(low_frontier_count, sizeof(node)); //Could use calloc here, not sure if issue
            sending_high_frontier = (node *) malloc(high_frontier_count, sizeof(node)); //Could use calloc here, not sure if issue

            //Calculate neighbours for all (excluding in our side frontiers - can't add wrap arounds)
            for(x = 1; x < (BLOCK_SIZE(rank, nprocs, size) - 1); x++){
                for(y = 0; y < size; y++){
                    for(it = local_graph[x][y]; it != NULL; it = it->next){
                        if(it->state == ALIVE)
                        visitNeighbours(local_graph, size, x, y, it->z);
                    }
                }
            }
            //Add our nodes to our frontiers
            int low_frontier_size=0,high_frontier_size=0;
            for(int y=0; y<size; y++){
                for(it = local_graph[0][y]; it !=NULL; it = it->next){
                    if(it->state == ALIVE){
                        sending_low_frontier[low_frontier_size].x = 0;
                        sending_low_frontier[low_frontier_size].y = y;
                        sending_low_frontier[low_frontier_size].z = it->z;
                        low_frontier_size++;
                    }
                }
                for(it = local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it !=NULL; it = it->next){
                    if(it->state == ALIVE){
                        sending_high_frontier[high_frontier_size].x = 0;
                        sending_high_frontier[high_frontier_size].y = y;
                        sending_high_frontier[high_frontier_size].z = it->z;
                        high_frontier_size++;
                    }
                }
            }
            //Send our frontiers
            //To which ranks do we send?
            int low_rank = (rank-1) < 0 ? (nprocs-1) : (rank-1);
            int high_rank = (rank+1) >= nprocs ? (0) : (rank+1);
            printf("@Rank %d - Low rank: %d High rank: %d\n", rank, low_rank, high_rank);



            //Send our frontiers to the low and then to the high rank
            printf("Rank %d starting probe on high rank: %d...\n", rank, high_rank);
            MPI_Status s1;
            MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s1);
            MPI_Get_count(&s1, MPI_CELL, &high_number_amount);
            receiving_high_frontier = (node*)malloc(sizeof(node) * high_number_amount);
            printf("Rank %d waiting on high rank: %d to receive...\n", rank, high_rank);
            MPI_Recv(receiving_high_frontier, high_number_amount, MPI_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("rank %d Sending high frontier size to the right...\n", rank);
            MPI_Send(sending_high_frontier, high_frontier_size, MPI_CELL, high_rank, 0, MPI_COMM_WORLD);

            MPI_Send(sending_low_frontier, low_frontier_size, MPI_CELL, low_rank, 0, MPI_COMM_WORLD);
            MPI_Status s2;
            MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s2);
            MPI_Get_count(&s2, MPI_CELL, &low_number_amount);
            receiving_low_frontier = (node*)malloc(sizeof(node) * low_number_amount);
            MPI_Recv(receiving_low_frontier, low_number_amount, MPI_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            //We have all the frontiers here
            //Go over our side of the high frontier and take in consideration all those that are on the high frontier that was sent to us
            for(y=0; y < size; y++){
                for(it=local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 5 possible neighbours on our side of the frontier
                        int x2, y1, y2, z1, z2;
                        int z = it->z;
                        int x = (BLOCK_SIZE(rank, nprocs, size) - 1);
                        x2 = (x-1);
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);
                        /* If a cell is visited for the first time, add it to the update list, for fast access */
                        //graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x2][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                        //Check the possible neighbour on the frontier we just received (look for y and z)
                        for(i=0; i<high_number_amount; i++){
                            if(receiving_high_frontier[i].y == y && receiving_high_frontier[i].z == z){
                                it->neighbours++;
                            }
                        }
                    }
                }

                //Go over our side of the low frontier and take in consideration all those that are on the low frontier that was sent to us
                for(it=local_graph[0][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 5 possible neighbours on our side of the frontier
                        int x1, y1, y2, z1, z2;
                        int z = it->z;
                        int x = 0;
                        x1 = (x+1);
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);
                        /* If a cell is visited for the first time, add it to the update list, for fast access */
                        //graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                        //Check the possible neighbour on the frontier we just received (look for y and z)
                        for(i=0; i<low_number_amount; i++){
                            if(receiving_low_frontier[i].y == y && receiving_low_frontier[i].z == z){
                                it->neighbours++;
                            }
                        }
                    }
                }
            }

            //Compute the next state of all our nodes
            for(i = 0; i < BLOCK_SIZE(rank,nprocs,size); i++){
                for(j = 0; j < size; j++){
                    for (it = graph[i][j]; it != NULL; it = it->next){
                        live_neighbours = it->neighbours;
                        it->neighbours = 0;
                        if(it->state == ALIVE){
                            if(live_neighbours < 2 || live_neighbours > 4){
                                it->state = DEAD;
                            }
                        }else{
                            if(live_neighbours == 2 || live_neighbours == 3){
                                it->state = ALIVE;
                            }
                        }
                    }
                }
            }
            //Free all frontiers this rank used and reset the frontier counters
            free(receivebuffer);
            free(sending_low_frontier);
            free(sending_high_frontier);
            free(receiving_low_frontier);
            free(receiving_high_frontier);
            //Do a cleanup (for eficiency)
            if(g % REMOVAL_PERIOD == 0){
                for(i = 0; i < BLOCK_SIZE(rank,nprocs,size); i++){
                    for(j = 0; j < cube_size; j++){
                        graph_node ** list = &local_graph[i][j];
                        graphListCleanup(list);
                    }
                }
            }
        } //Generations loop end
        /********************************************POS-GENERATION-PROCESSING**************************************************/
        local_graph_length=0;
        //Generations ended. Copy your local_graph to an array
        for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
            for(y = 0; y < size; y++){
                for(it = local_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        local_graph_length++;
                    }
                }
            }
        }
        //Send the length of that array to ROOT
        MPI_Gather(local_graph_length, 1, MPI_INT, NULL, 0, MPI_INT, ROOT, MPI_COMM_WORLD);
        //Allocate our array and copy everything to it
        lg_send = (node*)malloc(sizeof(node) * local_graph_length);
        local_graph_length=0;
        for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
            for(y = 0; y < size; y++){
                for(it = local_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        lg_send[local_graph_length].x = x + BLOCK_LOW(rank, nprocs, size);
                        lg_send[local_graph_length].y = y;
                        lg_send[local_graph_length].z = it->z;
                        local_graph_length++;
                    }
                }
            }
        }
        //Send the actual array to ROOT
        MPI_Gatherv(lg_send, local_graph_length, MPI_CELL, NULL, 0, lg_displs, ROOT, MPI_COMM_WORLD);
        //Free the temporary array to send the local_graph
        free(lg_send);
    }



    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
}
