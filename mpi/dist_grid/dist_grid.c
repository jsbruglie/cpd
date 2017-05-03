/** @file dist_grids.c
*  @brief Distributed MPI version of bruteforce in C
*  @author Pedro Abreu
*  @author João Borrego
*  @author Miguel Cardoso
*/
#include "dist_grid.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MAIN +++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char **argv) {
    graph_node*** initial_global_graph;     /**<Initial Global Graph representation - 2D array of lists */
    graph_node*** local_graph; /**< Local Graph representation - partition of the total graph */
    graph_node* it; /**< Graph iterator */
    int nprocs = 	0, rank = 0; //Number of processes and rank of the process
    int size = 0; /**< Cube size */
    int i = 0; /**< Generic iterator */
    int cells_receive;  /**< Cells that are announced that the process will receive */
    int x,y,z;
    int generations; /**< Generations */
    int* sendcounts, *displs = NULL; /**< sendcounts - number of elements to send to each rank. displs - offset regarding the start of the array in terms of number of elements */

    /*Communication buffers*/
    node* receivebuffer;
    node* sendbuffer;

    /*Frontiers*/
    int low_number_amount, high_number_amount;
    int low_frontier_count=0, high_frontier_count=0;
    int low_frontier_size=0, high_frontier_size=0;
    node* sending_low_frontier; /**< Buffer for sending your side of the low frontier */
    node* sending_high_frontier; /**< Buffer for sending your side of the high frontier */
    node* receiving_low_frontier; /**< Buffer to receive the other side of the low frontier */
    node* receiving_high_frontier; /**< Buffer to receive the other side of the high frontier */

    int alive_nodes = 0;

    MPI_Init(&argc, &argv);
    /*MPI PREAMBLE*/
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //Assign nprocs
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Assign rank
    /*MPI Create a structure to send over messages*/
    MPI_Datatype 	MPI_CELL, MPI_CELL_t, struct_type[3] = {MPI_INT, MPI_INT, MPI_INT};
    int struct_b_len[3] = {1, 1, 1};
    MPI_Aint struct_extent, struct_lb, struct_displs[3] = 	{offsetof(node, x), offsetof(node, y), offsetof(node, z)};
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_CELL_t);
    MPI_Type_get_extent(MPI_CELL_t, &struct_lb, &struct_extent);
    MPI_Type_create_resized(MPI_CELL_t, -struct_lb, struct_extent, &MPI_CELL);
    MPI_Type_commit(&MPI_CELL);

    if(rank == ROOT){
        /***************************************************** RAN BY ROOT **************************************************************************/
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
        /************************************************** BROADCAST_SIZE *************************************************************************/
        MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        rank_print(rank); debug_print("Received size: %d\n", size);

        /************************************************** CALCULATE SENDCOUNTS AND DISPLS *************************************************************************/
        sendcounts = (int *) calloc(nprocs, sizeof(int));
        //Read the number of alive nodes and the sendcount value
        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                alive_nodes++;
                sendcounts[BLOCK_OWNER(x,nprocs,size)]++;
            }
        }
        displs = (int *) calloc(nprocs, sizeof(int));

        debug_print("Alive nodes: %d\n", alive_nodes);
        for(i = 0; i < nprocs; i++){
            displs[i+1] = displs[i] + sendcounts[i];
        }

        /*for(i=0; i<nprocs; i++){
        printf("Root computing number of nodes proc %d: %d\n", nprocs, sendcounts[i]);fflush(stdout);
        printf("Root computing displacements: %d\n", displs[i]);
        }*/
        /************************************************** SCATTER SENDCOUNTS FROM ROOT TO ALL *************************************************************************/
        MPI_Scatter(sendcounts, 1, MPI_INT, &cells_receive, 1, MPI_INT, ROOT, MPI_COMM_WORLD); //Tell all other procs how many cells they are going to receive

        cells_receive = sendcounts[ROOT]; //ROOT gets the first set of cells
        rank_print(rank);debug_print("Received their sendcount: %d\n", cells_receive);
        /************************************************** CREATE GLOBAL GRAPH *************************************************************************/
        //Create the global graph
        initial_global_graph = initGraph(size);
        fclose(fp);fp = fopen(file, "r"); //Close and reopen file to reset fp
        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                /* Insert live nodes in the graph and the update set */
                initial_global_graph[x][y] = graphNodeInsert(initial_global_graph[x][y], z, ALIVE);
            }
        }
        /************************************************** COPY GLOBAL GRAPH TO SENDBUFFER *************************************************************************/
        //Allocate an array with all the alive nodes
        sendbuffer = (node*) malloc(alive_nodes*sizeof(node));
        //From the global graph fill sendbuffer
        i=0;
        for(x=0; x < size; x++){
            for(y=0; y < size; y++){
                for(it = initial_global_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        //printf("%d %d %d\n", x, y, it->z);
                        //sendbuffer[count] = (node) malloc(sizeof(node));
                        sendbuffer[i].x = x;
                        sendbuffer[i].y = y;
                        sendbuffer[i].z = it->z;
                        i++;
                    }
                }
            }
        }

        /* Convert the coordinates read into an array for sending */
        receivebuffer = (node *) calloc(cells_receive, sizeof(node));
        /************************************************** SCATTER SENDBUFFER *************************************************************************/
        MPI_Scatterv(sendbuffer, sendcounts, displs, MPI_CELL, receivebuffer, cells_receive, MPI_CELL, ROOT, MPI_COMM_WORLD);
        /*ROOT has the first set of sendbuffer - it's own cells_receive*/
        for(int i=0; i<cells_receive; i++){
            receivebuffer[i].y = sendbuffer[i].y;
            receivebuffer[i].z = sendbuffer[i].z;
            receivebuffer[i].x = sendbuffer[i].x;
        }

        /************************************************** ALLOCATE LOCAL GRAPH *************************************************************************/
        local_graph = initLocalGraph(BLOCK_SIZE(rank,nprocs,size), size); //Allocate local graph

        rank_print(rank); debug_print("Created local graph.\n");
        for(int i=0; i<cells_receive; i++){
            int x = receivebuffer[i].x - BLOCK_LOW(rank, nprocs, size);
            int y = receivebuffer[i].y;
            local_graph[x][y] = graphNodeInsert(local_graph[x][y], receivebuffer[i].z, ALIVE);

        }
        /************************************************** COUNT/ALLOCATE FRONTIER SIZES  *************************************************************************/
        low_frontier_count=0; high_frontier_count=0;
        for(y=0; y<size; y++){
            for(it = local_graph[0][y]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    low_frontier_count++;
                }
            }

            for(it = local_graph[BLOCK_SIZE(rank,nprocs,size)-1][y]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    high_frontier_count++;
                }
            }
        }

        rank_print(rank); debug_print("LOW FRONTIER COUNT: %d, HIGH FRONTIER COUNT: %d\n", low_frontier_count, high_frontier_count);
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
        /************************************************** PUT NODES FROM OUR FRONTIERS IN BUFFERS *************************************************************************/
        //Add our nodes to our frontiers
        low_frontier_size=0,high_frontier_size=0;
        for(y=0; y<size; y++){
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

        /************************************************** COMPUTE WHICH RANKS TO SEND/RECEIVE FROM *************************************************************************/
        int low_rank = (rank-1) < 0 ? (nprocs-1) : (rank-1);
        int high_rank = (rank+1) >= nprocs ? (0) : (rank+1);
        rank_print(rank); debug_print("Low rank: %d High rank: %d\n", low_rank, high_rank);

        /************************************************** COMPUTE WHICH RANKS TO SEND/RECEIVE FROM *************************************************************************/
        rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
        MPI_Send(sending_low_frontier, low_frontier_size, MPI_CELL, low_rank, 0, MPI_COMM_WORLD);
        rank_print(rank);debug_print("Receiving from rank %d\n", low_rank);
        MPI_Status s1;
        MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s1);
        MPI_Get_count(&s1, MPI_CELL, &low_number_amount);
        receiving_low_frontier = (node*)malloc(sizeof(node) * low_number_amount);
        MPI_Recv(receiving_low_frontier, low_number_amount, MPI_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        rank_print(rank);debug_print("Receiving from rank %d\n", high_rank);
        MPI_Status s2;
        MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s2);
        MPI_Get_count(&s2, MPI_CELL, &high_number_amount);
        receiving_high_frontier = (node*)malloc(sizeof(node) * high_number_amount);
        MPI_Recv(receiving_high_frontier, high_number_amount, MPI_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
        MPI_Send(sending_high_frontier, high_frontier_size, MPI_CELL, high_rank, 0, MPI_COMM_WORLD);

        fclose(fp);

    }else{
        /***************************************************** OTHER PROCS *********************************************************/
        /***************************************************** RECEIVE BROADCAST OF CUBE SIZE *********************************************************/
        MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        rank_print(rank);debug_print("Received size: %d\n", size);

        /***************************************************** RECEIVE NUMBER OF CELLS *********************************************************/
        MPI_Scatter(sendcounts, 1, MPI_INT, &cells_receive, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        rank_print(rank);debug_print("Received their sendcount: %d\n", cells_receive);
        receivebuffer = (node *) calloc(cells_receive, sizeof(node)); //Allocate buffer for reception

        /***************************************************** RECEIVE CELLS *********************************************************/
        MPI_Scatterv(sendbuffer, sendcounts, displs, MPI_CELL, receivebuffer, cells_receive, MPI_CELL, ROOT, MPI_COMM_WORLD);

        rank_print(rank);debug_print("BLOCK_LOW: %d, BLOCK_HIGH: %d, BLOCK_SIZE: %d\n", BLOCK_LOW(rank,nprocs,size), BLOCK_HIGH(rank,nprocs,size), BLOCK_SIZE(rank,nprocs,size));

        /***************************************************** FILL LOCAL GRAPH *********************************************************/
        local_graph = initLocalGraph(BLOCK_SIZE(rank,nprocs,size), size);

        for(i=0; i<cells_receive; i++){
            int x = receivebuffer[i].x - BLOCK_LOW(rank, nprocs, size);
            int y = receivebuffer[i].y;
            local_graph[x][y] = graphNodeInsert(local_graph[x][y], receivebuffer[i].z, ALIVE);
        }
        /***************************************************** COUNT SIZE OF FRONTIER*********************************************************/
        low_frontier_count=0; high_frontier_count=0;
        for(y=0; y<size; y++){
            for(it = local_graph[0][y]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    low_frontier_count++;
                }
            }

            for(it = local_graph[BLOCK_SIZE(rank,nprocs,size)-1][y]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    high_frontier_count++;
                }
            }
        }

        rank_print(rank);debug_print("LOW FRONTIER COUNT: %d, HIGH FRONTIER COUNT: %d\n", low_frontier_count, high_frontier_count);
        //Go over the local graph and compute the number to assign to each frontier

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
        /***************************************************** FILL FRONTIER BUFFERS*********************************************************/
        //Add our nodes to our frontiers
        low_frontier_size=0,high_frontier_size=0;
        for(y=0; y<size; y++){
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
        /***************************************************** COMPUTE WHICH RANKS TO SEND TO *********************************************************/
        int low_rank = (rank-1) < 0 ? (nprocs-1) : (rank-1);
        int high_rank = (rank+1) >= nprocs ? (0) : (rank+1);
        rank_print(rank);debug_print("Low rank: %d High rank: %d\n", low_rank, high_rank);

        /***************************************************** EXCHANGE FRONTIERS *********************************************************/
        rank_print(rank);debug_print("Receiving from rank %d\n", high_rank);
        MPI_Status s1;
        MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s1);
        MPI_Get_count(&s1, MPI_CELL, &high_number_amount);
        receiving_high_frontier = (node*)malloc(sizeof(node) * high_number_amount);
        MPI_Recv(receiving_high_frontier, high_number_amount, MPI_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        rank_print(rank);debug_print("Sending %d to rank %d\n", high_frontier_size, high_rank);
        MPI_Send(sending_high_frontier, high_frontier_size, MPI_CELL, high_rank, 0, MPI_COMM_WORLD);

        rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
        MPI_Send(sending_low_frontier, low_frontier_size, MPI_CELL, low_rank, 0, MPI_COMM_WORLD);
        rank_print(rank);debug_print("Receiving from rank %d\n", low_rank);
        MPI_Status s2;
        MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s2);
        MPI_Get_count(&s2, MPI_CELL, &low_number_amount);
        receiving_low_frontier = (node*)malloc(sizeof(node) * low_number_amount);
        MPI_Recv(receiving_low_frontier, low_number_amount, MPI_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Use the frontiers received AND OUR OWN to finish count (Go over frontiers received and update)

        //Compute the next state of all our nodes

        //Free all frontiers this rank used and reset the frontier counters

    }


    /*Barrier at the end to make sure all procs sync here before finalizing*/
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize(); //Finalize
}

/***************************** AUXILIARY FUNCTIONS *******************************/

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
