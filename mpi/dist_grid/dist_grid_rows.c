/** @file dist_grids.c
*  @brief Distributed MPI version of bruteforce in C
*  @author Pedro Abreu
*  @author João Borrego
*  @author Miguel Cardoso
*/
#include "dist_grid_rows.h"
const bool ROW_WISE = false;
const bool COLUMN_WISE = true;

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MAIN +++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char **argv) {
    if(ROW_WISE == COLUMN_WISE){
        err_print("Please choose ROW_WISE or COLUMN_WISE mode");
        exit(EXIT_FAILURE);
    }
    graph_node*** initial_global_graph, ***final_global_graph; /**<Initial Global Graph representation - 2D array of lists */
    graph_node*** local_graph; /**< Local Graph representation - partition of the total graph */
    graph_node* it; /**< Graph iterator */
    int nprocs = 0, rank = 0; //Number of processes and rank of the process
    int size = 0; /**< Cube size */
    int x, y, z, i, j; /**< Generic iterators */
    int cells_receive;  /**< Cells that are announced that the process will receive */
    int generations; /**< Generations */
    int* sendcounts, *displs = NULL; /**< sendcounts - number of elements to send to each rank. displs - offset regarding the start of the array in terms of number of elements */

    /*Communication buffers*/
    node *receivebuffer, *sendbuffer;

    /*Frontiers*/
    int low_number_amount, high_number_amount;
    int low_frontier_count=0, high_frontier_count=0;
    int low_frontier_size=0, high_frontier_size=0;
    neighbour_node* sending_low_frontier; /**< Buffer for sending your side of the low frontier */
    neighbour_node* sending_high_frontier; /**< Buffer for sending your side of the high frontier */
    neighbour_node* receiving_low_frontier; /**< Buffer to receive the other side of the low frontier */
    neighbour_node* receiving_high_frontier; /**< Buffer to receive the other side of the high frontier */

    int alive_nodes = 0;
    FILE* fp; char* file;   /**< Input data file and file name */

    int local_graph_length, total_length;
    int* lg_lengths, *lg_displs;
    node* all_lg, *lg_send;

    MPI_Init(&argc, &argv);
    /*MPI PREAMBLE*/
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //Assign nprocs
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //Assign rank
    int low_rank = (rank-1) < 0 ? (nprocs-1) : (rank-1);
    int high_rank = (rank+1) >= nprocs ? (0) : (rank+1);
    /************************************************** SHOW WHICH RANKS TO SEND/RECEIVE FROM *************************************************************************/
    rank_print(rank); debug_print("Low rank: %d High rank: %d\n", low_rank, high_rank);

    /*MPI Create a structure to send over initial nodes*/
    MPI_Datatype MPI_CELL, MPI_CELL_t, struct_type[3] = {MPI_UINT16_T, MPI_UINT16_T, MPI_UINT16_T};
    int struct_b_len[3] = {1, 1, 1};
    MPI_Aint struct_extent, struct_lb, struct_displs[3] = 	{offsetof(node, x), offsetof(node, y), offsetof(node, z)};
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_CELL_t);
    MPI_Type_get_extent(MPI_CELL_t, &struct_lb, &struct_extent);
    MPI_Type_create_resized(MPI_CELL_t, -struct_lb, struct_extent, &MPI_CELL);
    MPI_Type_commit(&MPI_CELL);
    /*MPI Create a structure used to send frontier*/
    MPI_Datatype MPI_NEIGHBOUR_CELL, MPI_NEIGHBOUR_CELL_t, s_type[2] = {MPI_UINT16_T, MPI_UINT16_T};
    int s_b_len[2] = {1, 1};
    MPI_Aint s_extent, s_lb, s_displs[2] = 	{offsetof(neighbour_node, y), offsetof(neighbour_node, z)};
    MPI_Type_create_struct(2, s_b_len, s_displs, s_type, &MPI_NEIGHBOUR_CELL_t);
    MPI_Type_get_extent(MPI_NEIGHBOUR_CELL_t, &s_lb, &s_extent);
    MPI_Type_create_resized(MPI_NEIGHBOUR_CELL_t, -s_lb, s_extent, &MPI_NEIGHBOUR_CELL);
    MPI_Type_commit(&MPI_NEIGHBOUR_CELL);

    double starttime, endtime;
    starttime = MPI_Wtime();

    char buffer[BUFFER_SIZE] = {0};
    if(rank == ROOT){
        /***************************************************** RAN BY ROOT **************************************************************************/
        /***************************************************** PARSE COMMAND LINE ARGUMENTS *********************************************************/
        parseArgs(argc, argv, &file, &generations);
        /***************************************************** OPEN FILE AND READ FIRST LINE *********************************************************/
        fp = fopen(file, "r");
        //free(file); //Free filename we no longer need it
        if(fp == NULL){
            fprintf(stderr, "Please input a valid file name\n" );
            exit(EXIT_FAILURE);
        }

        fgets(buffer,BUFFER_SIZE,fp);
        sscanf(buffer,"%d", &size);
    }

    /************************************************** BROADCAST_SIZE *************************************************************************/
    MPI_Bcast(&size, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    rank_print(rank); debug_print("Received size: %d\n", size);
    if(nprocs > size){
        err_print("Number of processors is higher than cube side size - Not possible in this row-wise/column-wise implementation");
        exit(EXIT_FAILURE);
    }
    /************************************************** BROADCAST_GENERATIONS *************************************************************************/
    MPI_Bcast(&generations, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    rank_print(rank); debug_print("Received generations: %d\n", generations);

    if(rank == ROOT){
        /************************************************** CALCULATE SENDCOUNTS AND DISPLS *************************************************************************/
        sendcounts = (int *) calloc(nprocs, sizeof(int));
        //Read the number of alive nodes and the sendcount value

        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                alive_nodes++;
                if(ROW_WISE)
                    sendcounts[BLOCK_OWNER(x,nprocs,size)]++;
                if(COLUMN_WISE)
                    sendcounts[BLOCK_OWNER(y,nprocs,size)]++;
            }
        }
        displs = (int *) calloc(nprocs, sizeof(int));

        debug_print("Alive nodes: %d\n", alive_nodes);
        for(i = 0; i < nprocs; i++){
            displs[i+1] = displs[i] + sendcounts[i];
        }
    }
    /************************************************** SCATTER SENDCOUNTS FROM ROOT TO ALL *************************************************************************/
    MPI_Scatter(sendcounts, 1, MPI_INT, &cells_receive, 1, MPI_INT, ROOT, MPI_COMM_WORLD); //Tell all other procs how many cells they are going to receive
    if(rank == ROOT){
        cells_receive = sendcounts[ROOT]; //ROOT gets the first set of cells
    }
    rank_print(rank);debug_print("Received their sendcount: %d\n", cells_receive);

    if(rank == ROOT){
        /************************************************** CREATE GLOBAL GRAPH *************************************************************************/
        //Create the global graph
        initial_global_graph = initGraph(size);
        fclose(fp);fp = fopen(file, "r"); //Close and reopen file to reset fp
        while(fgets(buffer, BUFFER_SIZE, fp)){
            if(sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
                if(nprocs != 1){
			if(ROW_WISE){
                    		initial_global_graph[x][y] = graphNodeInsert(initial_global_graph[x][y], z, ALIVE); /* Insert live nodes in the graph*/
                	}
                	if(COLUMN_WISE){
                    		initial_global_graph[y][x] = graphNodeInsert(initial_global_graph[y][x], z, ALIVE); /* Insert live nodes in the graph*/
                	}
		}else{
			 initial_global_graph[x][y] = graphNodeInsert(initial_global_graph[x][y], z, ALIVE); /* Insert live nodes in the graph*/
		}
            }
        }
        if(nprocs == 1){
            int g;
            for(g = 1; g <= generations; g++){
                for(i = 0; i < size; i++){ /* First passage in the graph - notify neighbours */
                    for(j = 0; j < size; j++){
                        for(it = initial_global_graph[i][j]; it != NULL; it = it->next){
                            if(it->state == ALIVE)
                                visitNeighbours(initial_global_graph, size, i, j, it->z);
                        }
                    }
                }
                /* Second passage in the graph - decide next state */
                for(i = 0; i < size; i++){
                    for(j = 0; j < size; j++){
                        for (it = initial_global_graph[i][j]; it != NULL; it = it->next){
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
                /* Remove dead nodes from the graph every REMOVAL_PERIOD generations */
                if(g % REMOVAL_PERIOD == 0){
                    for(i = 0; i < size; i++){
                        for(j = 0; j < size; j++){
                            graph_node ** list = &initial_global_graph[i][j];
                            graphListCleanup(list);
                        }
                    }
                }
            }
            /* Print the final set of live cells */
            printAndSortActive(initial_global_graph, size);
            //Free graph
            freeGraph(initial_global_graph, size);

            endtime = MPI_Wtime();
            time_print(rank, endtime - starttime);
            /*Barrier at the end to make sure all procs sync here before finalizing*/
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Finalize(); //Finalize
            exit(EXIT_SUCCESS);
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
    }
    /* Convert the coordinates read into an array for sending */
    receivebuffer = (node *) calloc(cells_receive, sizeof(node));
    /************************************************** SCATTER SENDBUFFER *************************************************************************/
    MPI_Scatterv(sendbuffer, sendcounts, displs, MPI_CELL, receivebuffer, cells_receive, MPI_CELL, ROOT, MPI_COMM_WORLD);
    /*ROOT has the first set of sendbuffer - it's own cells_receive*/
    rank_print(rank);debug_print("BLOCK_LOW: %d, BLOCK_HIGH: %d, BLOCK_SIZE: %d\n", BLOCK_LOW(rank,nprocs,size), BLOCK_HIGH(rank,nprocs,size), BLOCK_SIZE(rank,nprocs,size));
    if(rank == ROOT){
        for(i=0; i<cells_receive; i++){
            receivebuffer[i].y = sendbuffer[i].y;
            receivebuffer[i].z = sendbuffer[i].z;
            receivebuffer[i].x = sendbuffer[i].x;
        }
    }

    /************************************************** ALLOCATE LOCAL GRAPH *************************************************************************/
    local_graph = initLocalGraph(BLOCK_SIZE(rank,nprocs,size), size); //Allocate local graph

    rank_print(rank); debug_print("Created local graph.\n");
    for(i=0; i<cells_receive; i++){
        int x = (int)receivebuffer[i].x - BLOCK_LOW(rank, nprocs, size);
        int y = (int)receivebuffer[i].y;
        local_graph[x][y] = graphNodeInsert(local_graph[x][y], receivebuffer[i].z, ALIVE);
    }
    /****************************************************** GENERATION LOOP  *************************************************************************/
    int g;
    for(g=1; g<=generations; g++){
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
        sending_low_frontier = (neighbour_node *) calloc(low_frontier_count, sizeof(neighbour_node));
        sending_high_frontier = (neighbour_node *) calloc(high_frontier_count, sizeof(neighbour_node));

        /************************************************** PUT NODES FROM OUR FRONTIERS IN BUFFERS *************************************************************************/
        //Add our nodes to our frontiers
        low_frontier_size=0,high_frontier_size=0;
        for(y=0; y<size; y++){
            for(it = local_graph[0][y]; it !=NULL; it = it->next){
                if(it->state == ALIVE){
                    sending_low_frontier[low_frontier_size].y = y;
                    sending_low_frontier[low_frontier_size].z = it->z;
                    low_frontier_size++;
                }
            }
            for(it = local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it !=NULL; it = it->next){
                if(it->state == ALIVE){
                    sending_high_frontier[high_frontier_size].y = y;
                    sending_high_frontier[high_frontier_size].z = it->z;
                    high_frontier_size++;
                }
            }
        }

        if(rank == ROOT){
            /************************************************** COMPUTE WHICH RANKS TO SEND/RECEIVE FROM *************************************************************************/
            rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
            MPI_Send(sending_low_frontier, low_frontier_size, MPI_NEIGHBOUR_CELL, low_rank, 0, MPI_COMM_WORLD);
            rank_print(rank);debug_print("Receiving from rank %d\n", low_rank);
            MPI_Status s1;
            MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s1);
            MPI_Get_count(&s1, MPI_NEIGHBOUR_CELL, &low_number_amount);
            receiving_low_frontier = (neighbour_node*)malloc(sizeof(neighbour_node) * low_number_amount);
            MPI_Recv(receiving_low_frontier, low_number_amount, MPI_NEIGHBOUR_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            rank_print(rank);debug_print("Receiving from rank %d\n", high_rank);
            MPI_Status s2;
            MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s2);
            MPI_Get_count(&s2, MPI_NEIGHBOUR_CELL, &high_number_amount);
            receiving_high_frontier = (neighbour_node*)malloc(sizeof(neighbour_node) * high_number_amount);
            MPI_Recv(receiving_high_frontier, high_number_amount, MPI_NEIGHBOUR_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
            MPI_Send(sending_high_frontier, high_frontier_size, MPI_NEIGHBOUR_CELL, high_rank, 0, MPI_COMM_WORLD);
        }else{
            rank_print(rank);debug_print("Receiving from rank %d...\n", high_rank);
            MPI_Status s1;
            MPI_Probe(high_rank, 0, MPI_COMM_WORLD, &s1);
            MPI_Get_count(&s1, MPI_NEIGHBOUR_CELL, &high_number_amount);
            receiving_high_frontier = (neighbour_node*)malloc(sizeof(neighbour_node) * high_number_amount);
            MPI_Recv(receiving_high_frontier, high_number_amount, MPI_NEIGHBOUR_CELL, high_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            rank_print(rank);debug_print("Received from rank %d...\n", high_rank);
            rank_print(rank);debug_print("Sending %d to rank %d\n", high_frontier_size, high_rank);
            MPI_Send(sending_high_frontier, high_frontier_size, MPI_NEIGHBOUR_CELL, high_rank, 0, MPI_COMM_WORLD);

            rank_print(rank);debug_print("Sending %d to rank %d\n", low_frontier_size, low_rank);
            MPI_Send(sending_low_frontier, low_frontier_size, MPI_NEIGHBOUR_CELL, low_rank, 0, MPI_COMM_WORLD);
            rank_print(rank);debug_print("Receiving from rank %d...\n", low_rank);
            MPI_Status s2;
            MPI_Probe(low_rank, 0, MPI_COMM_WORLD, &s2);
            MPI_Get_count(&s2, MPI_NEIGHBOUR_CELL, &low_number_amount);
            receiving_low_frontier = (neighbour_node*)malloc(sizeof(neighbour_node) * low_number_amount);
            MPI_Recv(receiving_low_frontier, low_number_amount, MPI_NEIGHBOUR_CELL, low_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            rank_print(rank);debug_print("Received from rank %d\n", low_rank);
        }

        rank_print(rank);debug_print("Frontiers done! \n");

        /************************************************** COMPUTE THE NEIGHBOURS *************************************************************************/
        /************************************************** COMPUTE ALL NEIGHBOURS THAT ARE NOT ON OUR FRONTIERS *************************************************************************/
        //Use the frontiers received AND OUR OWN to finish count (Go over frontiers received and update)
        for(x = 1; x < (BLOCK_SIZE(rank, nprocs, size) - 1); x++){
            for(y = 0; y < size; y++){
                for(it = local_graph[x][y]; it != NULL; it = it->next){
                    if(it->state == ALIVE){
                        visitNeighbours(local_graph, size, x, y, it->z);
                    }
                }
            }
        }
        /************************************************** COMPUTE ALL NEIGHBOURS THAT ARE ON OUR FRONTIERS WITHOUT PROCESSING THE OTHER SIDE *************************************************************************/
        for(y=0; y < size; y++){
            if(BLOCK_SIZE(rank,nprocs,size) != 1){
                for(it=local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 5 possible neighbours on our side of the frontier
                        int x2, y1, y2, z1, z2;
                        int z = it->z;
                        int x=(BLOCK_SIZE(rank, nprocs, size) - 1);
                        x2 = (x-1);
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);

                        graphNodeAddNeighbour(&(local_graph[x2][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
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
                        graphNodeAddNeighbour(&(local_graph[x1][y]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                    }
                }
            }else{
                for(it=local_graph[0][y]; it!=NULL; it=it->next){
                    if(it->state == ALIVE){
                        //Check the 4 possible neighbours since we ARE the frontier
                        int y1, y2, z1, z2;
                        int z = it->z;
                        int x = 0;
                        y1 = (y+1)%size; y2 = (y-1) < 0 ? (size-1) : (y-1);
                        z1 = (z+1)%size; z2 = (z-1) < 0 ? (size-1) : (z-1);
                        graphNodeAddNeighbour(&(local_graph[x][y1]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y2]), z);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z1);
                        graphNodeAddNeighbour(&(local_graph[x][y]), z2);
                    }
                }
            }

        }

        /*********************************HIGH FRONTIER PROCESSING***********************************/
        for(i=0; i<high_number_amount; i++){ //This means we found an adjacent node on the other side of the frontier
            y = receiving_high_frontier[i].y;
            z = receiving_high_frontier[i].z;
            graphNodeAddNeighbour(&(local_graph[(BLOCK_SIZE(rank, nprocs, size) - 1)][y]),z);
        }

        /*********************************LOW FRONTIER PROCESSING***********************************/
        for(i=0; i<low_number_amount; i++){ //This means we found an adjacent node on the other side of the frontier
            y = receiving_low_frontier[i].y;
            z = receiving_low_frontier[i].z;
            graphNodeAddNeighbour(&(local_graph[0][y]),z);
        }

        free(sending_low_frontier);
        free(sending_high_frontier);
        free(receiving_low_frontier);
        free(receiving_high_frontier);

        /************************************************** COMPUTE THE NEXT STATE OF ALL NODES (FRONTIER + OTHERS) *************************************************************************/
        for(x = 0; x < BLOCK_SIZE(rank,nprocs,size); x++){
            for(y = 0; y < size; y++){
                for (it = local_graph[x][y]; it != NULL; it = it->next){
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


        /* Remove dead nodes from the graph every REMOVAL_PERIOD generations */
        if(g % REMOVAL_PERIOD == 0){
            for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
                for(y = 0; y < size; y++){
                    graph_node** list = &local_graph[x][y];
                    graphListCleanup(list);
                }
            }
        }

    }//Generations loop end
    /********************************************POS-GENERATION-PROCESSING BY ROOT**************************************************/
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
    rank_print(rank);debug_print("LOCAL GRAPH LENGTH: %d\n", local_graph_length);
    if(rank == ROOT){
        lg_lengths = (int*)malloc(sizeof(int)*nprocs);
        lg_lengths[ROOT] = local_graph_length; //lc_length of root was already computed
    }
    MPI_Gather(&local_graph_length, 1, MPI_INT, lg_lengths, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    if(rank == ROOT){
        //We should have lc lengths here (from each proc)
        for(i=0; i<nprocs; i++){
            rank_print(rank);debug_print("Got local graph length %d from rank %d\n", lg_lengths[i], i);
        }

        lg_displs = (int*)malloc(sizeof(int)*nprocs);
        total_length = 0;
        for(i=0; i<nprocs; i++){
            lg_displs[i] = total_length;
            total_length += lg_lengths[i];
        }
        //printf("LOCAL GRAPH LENGTH OF ROOT: %d\n",local_graph_length);
        all_lg = (node*)malloc(sizeof(node) * total_length);
    }

    lg_send = (node*)malloc(sizeof(node) * local_graph_length);
    local_graph_length=0;
    for(x = 0; x < BLOCK_SIZE(rank, nprocs, size); x++){
        for(y = 0; y < size; y++){
            for(it = local_graph[x][y]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    lg_send[local_graph_length].x = x + BLOCK_LOW(rank, nprocs, size); //add the offset so the x value is the real value (not the local)
                    lg_send[local_graph_length].y = y;
                    lg_send[local_graph_length].z = it->z;
                    local_graph_length++;
                }
            }
        }
    }

    MPI_Gatherv(lg_send, local_graph_length, MPI_CELL, all_lg, lg_lengths, lg_displs, MPI_CELL, ROOT, MPI_COMM_WORLD);
    if(rank == ROOT){
        /***************ROOT SHOULD HAVE ALL ARRAYS HERE***********************/
        //Transform all_lg to final global graph
        final_global_graph = initGraph(size);
        for(i=0; i<total_length; i++){
            x = all_lg[i].x;
            y = all_lg[i].y;
            z = all_lg[i].z;
            if(ROW_WISE){
                final_global_graph[x][y] = graphNodeInsert(final_global_graph[x][y], z, ALIVE);
            }
            if(COLUMN_WISE){
                final_global_graph[y][x] = graphNodeInsert(final_global_graph[y][x], z, ALIVE);
            }
        }
        /* Print the final set of live cells */
        printAndSortActive(final_global_graph, size);fflush(stdout);

        //Free graphs
        freeGraph(final_global_graph, size);
        freeGraph(initial_global_graph, size);

    }
    //TODO - Free the temporary arrays used to send the local_graph

    endtime = MPI_Wtime();
    time_print(rank, endtime - starttime);
    /*Barrier at the end to make sure all procs sync here before finalizing*/
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize(); //Finalize
    exit(EXIT_SUCCESS);
}

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
