/**
 * @file dist_grid_checkerboard.c
 * @brief Game Of Life 3D MPI Implementation
 * 
 * @details Uses checkerboard decomposition to scatter the 
 * 3D graph across several processes.
 * If an invalid configuration is provided the program will not attempt
 * to run, and will notify the user instead.
 * 
 * @attention Each process is assumed to have access to the input file!
 *  
 * @author João Borrego
 * @author Pedro Abreu
 * @author Miguel Cardoso
 * @bug No known bugs.  
 */

#include "dist_grid_checkerboard.h"

int main (int argc, char **argv) {

    /* Main program variables */

    /** Global graph representation */
    GraphNode ***global_graph;
    /** Local graph representation */
    GraphNode ***local_graph;

    /* Local graph dimensions */
    int local_x, local_y;
    int local_dim_x, local_dim_y;

    int n_processes, rank;

    /** Size of the side of the cube space */
    int cube_size;
    /** Number of generations to be computed */
    int generations;

    /* Auxiliary variables */

    /* Main program execution */

    /** Live neighbours counter */
    int live_neighbours;
    /** Generations counter */
    int g;

    /* Iterative variables*/
    int x, y, z;
    int i, j;
    int snd_i, snd_j, snd_k, snd_l;
    GraphNode *it;

    /* Function return values, status codes and request identifiers */
    int mpi_rv;
    MPI_Status mpi_status_prb, mpi_status_snd, mpi_status_rcv;
    MPI_Request snd_req[4], rcv_req[4];

    /* Buffers */
    char buffer[BUFFER_SIZE] = {0};

    /* Number of live cells to be sent to each neighbour */
    int snd_count_low_x, snd_count_high_x, snd_count_low_y, snd_count_high_y;

    /* Frontier buffers to be sent to each neighbour */
    RNode *snd_low_x, *snd_high_x, *snd_low_y, *snd_high_y;

    /* Number of live cells to be received from each neighbour */
    int rcv_count_low_x, rcv_count_high_x, rcv_count_low_y, rcv_count_high_y;

    /* Frontier buffers to be received from each neighbour */
    RNode *rcv_low_x, *rcv_high_x, *rcv_low_y, *rcv_high_y;

    /* Final gather variables */
    int local_graph_length;
    int* lg_lengths;
    int* lg_displs;
    int total_length;
    Node* all_lg;
    Node* lg_send;

    /* Other */
    int mapped_x, mapped_y;

    /* Timing */
    double global_start_t, global_end_t;
    
    /* MPI */
    MPI_Init(&argc, &argv);

    /* MPI Preamble */
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /***********************************************************************************/

    /* Start Timer */
    MPI_Barrier(MPI_COMM_WORLD);
    global_start_t = MPI_Wtime();

    /***********************************************************************************/

    //if (rank == ROOT) debugPrint("Number of processes %d", n_processes);

    /* MPI Create a structure to send cells across processes */
    MPI_Datatype MPI_NEIGHBOUR_CELL, MPI_NEIGHBOUR_CELL_t,
        struct_type[3] = {MPI_UNSIGNED_SHORT, MPI_UNSIGNED_SHORT, MPI_UNSIGNED_SHORT};
    int struct_b_len[3] = {1, 1, 1};
    MPI_Aint struct_extent, struct_lb, struct_displs[3] = {offsetof(Node, x), offsetof(Node, y), offsetof(Node, z)};
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_NEIGHBOUR_CELL_t);
    MPI_Type_get_extent(MPI_NEIGHBOUR_CELL_t, &struct_lb, &struct_extent);
    MPI_Type_create_resized(MPI_NEIGHBOUR_CELL_t, -struct_lb, struct_extent, &MPI_NEIGHBOUR_CELL);
    MPI_Type_commit(&MPI_NEIGHBOUR_CELL);

    /* MPI Create a structure to send reduced (2 coordinates) cells across processes */
    MPI_Datatype MPI_R_NEIGHBOUR_CELL, MPI_R_NEIGHBOUR_CELL_t,
        r_struct_type[2] = {MPI_UNSIGNED_SHORT, MPI_UNSIGNED_SHORT};
    int r_struct_b_len[2] = {1, 1};
    MPI_Aint r_struct_extent, r_struct_lb, r_struct_displs[2] = {offsetof(RNode, a), offsetof(RNode, z)};
    MPI_Type_create_struct(2, r_struct_b_len, r_struct_displs, r_struct_type, &MPI_R_NEIGHBOUR_CELL_t);
    MPI_Type_get_extent(MPI_R_NEIGHBOUR_CELL_t, &r_struct_lb, &r_struct_extent);
    MPI_Type_create_resized(MPI_R_NEIGHBOUR_CELL_t, -r_struct_lb, r_struct_extent, &MPI_R_NEIGHBOUR_CELL);
    MPI_Type_commit(&MPI_R_NEIGHBOUR_CELL);

    /* MPI Configure Cartesian Communicator */

    /**< Cartesian 2D grid dimensions */
    int dims[N_DIMS] = {0};
    /**< Allow process reordering for optimal performance */
    int reorder = 1;
    /**< Allow wrap-around */
    int wrap_around[N_DIMS] = {1, 1};
    /**< Cartesian communicator */
    MPI_Comm grid_comm;

    /* Create a cartesian topology for processes */
    MPI_Dims_create(n_processes, N_DIMS, dims);
    /* Create the cartesian mapping */
    mpi_rv = MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, wrap_around, reorder, &grid_comm);

    /* Find this process' coordinates in cartesian comm group*/
    int coord[N_DIMS] = {0};
    MPI_Cart_coords(grid_comm, rank, N_DIMS, coord);

    /* Use coordinates to find rank in cartesian comm group */
    int cart_rank;
    MPI_Cart_rank(grid_comm, coord, &cart_rank);

    /*
    if (rank == ROOT){
        for (i = 0; i < N_DIMS; i++) {
            debugPrint("Dimension %d has %d processes", i, dims[i]);
        }
    }
    */

    /* Get neighbours */
    /* axis = 0 (X) p[row-1] : curr_p : p[row+1] */
    /* axis = 1 (Y) p[col-1] : curr_p : p[col+1] */
    int nbr_low_x, nbr_high_x, nbr_low_y, nbr_high_y;
    MPI_Cart_shift(grid_comm, SHIFT_ROW, DISP, &nbr_low_x, &nbr_high_x);
    MPI_Cart_shift(grid_comm, SHIFT_COL, DISP, &nbr_low_y, &nbr_high_y);

    /*
    MPI_Barrier(grid_comm);
    debugPrint("(%d,%d) Rank %d Neighbour x lo %d hi %d y: lo %d hi %d",
        coord[0], coord[1], cart_rank, nbr_low_x, nbr_high_x, nbr_low_y, nbr_high_y);
    */

    /***********************************************************************************/

    /* Parse arguments */

    /**< Input data file and file name */
    FILE* fp; char* file_name;

    parseArgs(argc, argv, &file_name, &generations);
    fp = fopen(file_name, "r");
    if (fp == NULL){
        fprintf(stderr, "Please input a valid file name\n" );
        exit(EXIT_FAILURE);
    }

    fgets(buffer,BUFFER_SIZE,fp);
    sscanf(buffer,"%d", &cube_size);

    /***********************************************************************************/

    /* Calculate block dimensions and offsets */

    int other_x = cube_size / dims[0];
    /* The first block gets the excess cells */
    int first_x = cube_size - (other_x * (dims[0] - 1));
    int other_y = cube_size / dims[1];
    int first_y = cube_size - (other_y * (dims[1] - 1));

    /* Block offsets */
    int offset_x = (coord[0] == 0) ? 0 : first_x + (coord[0] - 1) * other_x;
    int offset_y = (coord[1] == 0) ? 0 : first_y + (coord[1] - 1) * other_y;
    /* Block dimensions */
    int dim_x = (coord[0] == 0) ? first_x : other_x;
    int dim_y = (coord[1] == 0) ? first_y : other_y;
    /* Maximum value of a coordinate (absolute index in local graph <= MAX) */
    int max_x = offset_x + dim_x - 1;
    int max_y = offset_y + dim_y - 1;

    /* Graciously exit if an incompatible setup is provided */
    if (dim_x == 0 || dim_y == 0){
        errPrint("Incompatible number of processes and problem size");
        exit(EXIT_FAILURE);
    }

    //MPI_Barrier(grid_comm);
    //debugPrint("(%d,%d) Rank %d offset_x %d offset y %d dim_x %d dim_y %d",
    //    coord[0], coord[1], cart_rank, offset_x, offset_y, dim_x, dim_y);

    /***********************************************************************************/

    //MPI_Barrier(grid_comm);

    /* Fill local graph structure */
    local_graph = initGraph(dim_x, dim_y);

    /* Read file and fill with the nodes that belong to the current process */
    while (fgets(buffer, BUFFER_SIZE, fp)){
        if (sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
            /* If I am the owner of the block the read data point is in */
            if ((offset_x <= x && x <= max_x) && (offset_y <= y && y <= max_y)){
                mapped_x = x - offset_x; mapped_y = y - offset_y;
                local_graph[mapped_x][mapped_y] = graphNodeInsert(local_graph[mapped_x][mapped_y], z, ALIVE);
                //debugPrint("(%d,%d) Rank %d - Inserting (%d,%d,%d)", coord[0], coord[1], cart_rank, x, y, z);
            }
        }
    }
    //debugPrint("(%d,%d) Rank %d - Finished inserting nodes", coord[0], coord[1], cart_rank);

    //MPI_Barrier(grid_comm);

    /***********************************************************************************/

    for (g = 1; g <= generations; g++){

        /* Count number of live cells in each own frontier so they can be sent */
        snd_count_low_x = 0; snd_count_high_x = 0;
        snd_count_low_y = 0; snd_count_high_y = 0;

        for (i = 0; i < dim_y; i++){
            for (it = local_graph[0][i]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    snd_count_low_x++;
                }
            }
            for (it = local_graph[dim_x - 1][i]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    snd_count_high_x++;
                }
            }
        }
        for (i = 0; i < dim_x; i++){
            for (it = local_graph[i][0]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    snd_count_low_y++;
                }
            }
            for (it = local_graph[i][dim_y - 1]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    snd_count_high_y++;
                }
            }
        }

    /***********************************************************************************/

        /* Allocate buffers to be sent */
        snd_low_x = malloc(sizeof(RNode) * snd_count_low_x);
        snd_high_x = malloc(sizeof(RNode) * snd_count_high_x);
        snd_low_y = malloc(sizeof(RNode) * snd_count_low_y);
        snd_high_y = malloc(sizeof(RNode) * snd_count_high_y);

        snd_i = 0; snd_j = 0; snd_k = 0; snd_l = 0;

        /* Fill buffers */
        for (y = 0; y < dim_y; y++){
            for (it = local_graph[0][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    addToSndArray(snd_low_x, snd_i, y, it->z);
                    snd_i++;
                }
            }
            for (it = local_graph[dim_x - 1][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    addToSndArray(snd_high_x, snd_j, y, it->z);
                    snd_j++;
                }
            }
        }
        for (x = 0; x < dim_x; x++){
            for (it = local_graph[x][0]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    addToSndArray(snd_low_y, snd_k, x, it->z);
                    snd_k++;
                }
            }
            for (it = local_graph[x][dim_y - 1]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    addToSndArray(snd_high_y, snd_l, x, it->z);
                    snd_l++;
                }
            }
        }

    /***********************************************************************************/

        //MPI_Barrier(grid_comm);
        //debugPrint("Finished adding boundary to send arrays");

        /* Reset the counters of live neighbour cells to be received by each neighbour */
        rcv_count_low_x = 0; rcv_count_high_x = 0; rcv_count_low_y = 0; rcv_count_high_y = 0;

    /***********************************************************************************/

        /* Send and receive borders to/from neighbours */
        
        /* Communicate with lower x neighbour */
        sendBorder(grid_comm, TAG_LOW_X, cart_rank, nbr_low_x,
            &(snd_req[0]), MPI_R_NEIGHBOUR_CELL, snd_low_x, snd_count_low_x);
        rcv_count_high_x = receiveBorder(grid_comm, TAG_LOW_X, cart_rank, nbr_high_x,
            &(rcv_req[1]), &mpi_status_prb, MPI_R_NEIGHBOUR_CELL, &rcv_high_x);

        /* Communicate with higher x neighbour */
        sendBorder(grid_comm, TAG_HIGH_X, cart_rank, nbr_high_x,
            &(snd_req[1]), MPI_R_NEIGHBOUR_CELL, snd_high_x, snd_count_high_x);
        rcv_count_low_x = receiveBorder(grid_comm, TAG_HIGH_X, cart_rank, nbr_low_x,
            &(rcv_req[0]), &mpi_status_prb, MPI_R_NEIGHBOUR_CELL, &rcv_low_x);
        
        /* Communicate with lower y neighbour */
        sendBorder(grid_comm, TAG_LOW_Y, cart_rank, nbr_low_y,
            &(snd_req[2]), MPI_R_NEIGHBOUR_CELL, snd_low_y, snd_count_low_y);
        rcv_count_high_y = receiveBorder(grid_comm, TAG_LOW_Y, cart_rank, nbr_high_y,
            &(rcv_req[3]), &mpi_status_prb, MPI_R_NEIGHBOUR_CELL, &rcv_high_y);

        /* Communicate with lower x neighbour */
        sendBorder(grid_comm, TAG_HIGH_Y, cart_rank, nbr_high_y,
            &(snd_req[3]), MPI_R_NEIGHBOUR_CELL, snd_high_y, snd_count_high_y);
        rcv_count_low_y = receiveBorder(grid_comm, TAG_HIGH_Y, cart_rank, nbr_low_y,
            &(rcv_req[2]), &mpi_status_prb, MPI_R_NEIGHBOUR_CELL, &rcv_low_y);

    /***********************************************************************************/

        //MPI_Barrier(grid_comm);
        //debugPrint("Rank %d Finished exchanging boundaries", cart_rank);

    /***********************************************************************************/

        /* Process internal nodes, i.e. not on the boundary of the local graph */
        for (x = 1; x < dim_x - 1; x++){
            for (y = 1; y < dim_y - 1; y++){
                for (it = local_graph[x][y]; it != NULL; it = it->next){
                    if (it->state == ALIVE){
                        visitInternalNeighbours(local_graph, cube_size, x, y, it->z);
                    }
                }
            }
        }

        //MPI_Barrier(grid_comm);
        //debugPrint("Rank %d Finished visiting internal cells", cart_rank);

    /***********************************************************************************/

        /* Process nodes on the boundaries */

        /* Process first row */
        for (y = 0; y < dim_y; y++){
            for (it = local_graph[0][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    visitBoundaryNeighbours(local_graph, cube_size, dim_x, dim_y, 0, y, it->z);
                }
            }
        }

        /* If block has a single row, then the upper row and last coincide, and its nodes
            have already been processed */
        if (dim_x > 1){
            /* Process last row */
            for (y = 0; y < dim_y; y++){
                for (it = local_graph[dim_x - 1][y]; it != NULL; it = it->next){
                    if (it->state == ALIVE){
                        visitBoundaryNeighbours(local_graph, cube_size, dim_x, dim_y, dim_x - 1, y, it->z);
                    }
                }
            }
        }

        /* Process first column */
        for (x = 1; x < dim_x - 1; x++){
            for (it = local_graph[x][0]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    visitBoundaryNeighbours(local_graph, cube_size, dim_x, dim_y, x, 0, it->z);
                }
            }
        }

        /* If block has a single column, then the upper column and last coincide, and its nodes
            have already been processed */
        if (dim_y > 1){
            for (x = 1; x < dim_x - 1; x++){
                for (it = local_graph[x][dim_y - 1]; it != NULL; it = it->next){
                    if (it->state == ALIVE){
                        visitBoundaryNeighbours(local_graph, cube_size, dim_x, dim_y, x, dim_y - 1, it->z);
                    }
                }
            }
        }

        // MPI_Barrier(grid_comm);
        // debugPrint("Rank %d Finished visiting boundary cells", cart_rank);

    /***********************************************************************************/

        /* Process neighbour nodes */

        MPI_Wait(&(rcv_req[0]), &(mpi_status_rcv));
        
        /* Process lower x row */
        for (i = 0; i < rcv_count_low_x; i++){
            y = rcv_low_x[i].a;
            z = rcv_low_x[i].z;
            graphNodeAddNeighbour(&(local_graph[0][y]), z);
        }

        MPI_Wait(&(rcv_req[1]), &(mpi_status_rcv));
        
        /* Process higher x row */
        for (i = 0; i < rcv_count_high_x; i++){
            y = rcv_high_x[i].a;
            z = rcv_high_x[i].z;
            graphNodeAddNeighbour(&(local_graph[dim_x - 1][y]), z);
        }

        MPI_Wait(&(rcv_req[2]), &(mpi_status_rcv));

        /* Process lower y column */
        for (i = 0; i < rcv_count_low_y; i++){
            x = rcv_low_y[i].a;
            z = rcv_low_y[i].z;
            graphNodeAddNeighbour(&(local_graph[x][0]), z);
        }

        MPI_Wait(&(rcv_req[3]), &(mpi_status_rcv));        

        /* Process higher y column */
        for (i = 0; i < rcv_count_high_y; i++){
            x = rcv_high_y[i].a;
            z = rcv_high_y[i].z;
            graphNodeAddNeighbour(&(local_graph[x][dim_y - 1]), z);
        }

        // MPI_Barrier(grid_comm);
        // debugPrint("Rank %d Finished processing neighbour cells", cart_rank);

    /***********************************************************************************/

        /* Determine next state of each cell */
        for (x = 0; x < dim_x; x++){
            for (y = 0; y < dim_y; y++){
                for (it = local_graph[x][y]; it != NULL; it = it->next){
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

        /* Remove dead nodes from the graph every REMOVAL_PERIOD generations */
        if (g % REMOVAL_PERIOD == 0){
            for (x = 0; x < dim_x; x++){
                for (y = 0; y < dim_y; y++){
                    graphListCleanup(&(local_graph[x][y]));
                }
            }
        }

        for (i = 0; i < 4; i++){
            MPI_Wait(&(snd_req[i]), &(mpi_status_snd));
        }

    /***********************************************************************************/

        /* Cleanup sending buffers */
        free(snd_low_x); free(snd_high_x);
        free(snd_low_y); free(snd_high_y);

    /***********************************************************************************/

    } // END MAIN FOR LOOP

    /***********************************************************************************/
    
    /* Compute local graph size*/
    
    local_graph_length = 0;
    for (x = 0; x < dim_x; x++){
        for (y = 0; y < dim_y; y++){
            for (it = local_graph[x][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    local_graph_length++;
                }
            }
        }
    }

    if (rank == ROOT) {
        lg_lengths = malloc(sizeof(int) * n_processes);
        /* lc_length of root was already computed */
        lg_lengths[ROOT] = local_graph_length;
    }

    /***********************************************************************************/
    
    /* Announce local graph size to root */
    
    MPI_Gather(&local_graph_length, 1, MPI_INT, lg_lengths, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    
    if (rank == ROOT){
        
        /*
        for (i = 0; i < n_processes; i++){
            debugPrint("ROOT got local graph length %d from rank %d", lg_lengths[i], i);
        }
        */
        
        /* Displacements */
        lg_displs = malloc( sizeof(int) * n_processes);
        total_length = 0;
        for (i = 0; i < n_processes; i++){
            lg_displs[i] = total_length;
            total_length += lg_lengths[i];
        }

        /* Buffer to receive all local graphs */
        all_lg = malloc(sizeof(Node) * total_length);
    }

    /***********************************************************************************/
    
    /* Fill buffer to send to ROOT from local graph */
    lg_send = malloc(sizeof(Node) * local_graph_length);
    local_graph_length = 0;
    for (x = 0; x < dim_x; x++){
        for (y = 0; y < dim_y; y++){
            for (it = local_graph[x][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    // Add offsets to obtain global coordinates, instead of local
                    lg_send[local_graph_length].x = x + offset_x;
                    lg_send[local_graph_length].y = y + offset_y;
                    lg_send[local_graph_length].z = it->z;
                    local_graph_length++;
                }
            }
        }
    }

    /* Gather global graph in ROOT process */
    MPI_Gatherv(lg_send, local_graph_length, MPI_NEIGHBOUR_CELL, all_lg, lg_lengths,
        lg_displs, MPI_NEIGHBOUR_CELL, ROOT, MPI_COMM_WORLD);
    
    /***********************************************************************************/

    /* Stop Timer */
    global_end_t = MPI_Wtime();

    /***********************************************************************************/

    if(rank == ROOT){
        
        /* Convert array of final alive nodes into a global graph */
        global_graph = initGraph(cube_size,cube_size);
        for (i = 0; i < total_length; i++){
            x = all_lg[i].x; y = all_lg[i].y; z = all_lg[i].z;
            global_graph[x][y] = graphNodeInsert(global_graph[x][y], z, ALIVE);
        }

        /* Print the final set of live cells */
        printAndSortActive(global_graph, cube_size); fflush(stdout);

        /* Free global graph */
        freeGraph(global_graph, cube_size, cube_size);

        /* Print global execution time */
        timePrint(global_end_t - global_start_t);
    }

    /***********************************************************************************/

    /* Clean up */
    freeGraph(local_graph, dim_x, dim_y);
    free(file_name);

    /* Force a synchronisation point and exit */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
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

void addToSndArray(RNode *array, int index, int a, int z){

    array[index].a = a;
    array[index].z = z;
}

void sendBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
               MPI_Request *req, MPI_Datatype mpi_datatype,
                RNode *snd, int snd_size){

    MPI_Isend(snd, snd_size, mpi_datatype, nbr_rank, mpi_tag, mpi_comm, req);
    //debugPrint("Rank %d <-> %d - sent %d (TAG %d)", my_rank, nbr_rank, snd_size, mpi_tag);
}

int receiveBorder(MPI_Comm mpi_comm, int mpi_tag, int my_rank, int nbr_rank,
                    MPI_Request *req, MPI_Status *status_prb,
                    MPI_Datatype mpi_datatype, RNode **rcv){

    int rcv_size = 0;

    MPI_Probe(nbr_rank, mpi_tag, mpi_comm, status_prb);
    MPI_Get_count(status_prb, mpi_datatype, &rcv_size);

    if (rcv_size > 0){
        *rcv = malloc(sizeof(Node) * rcv_size);
    }

    MPI_Irecv(*rcv, rcv_size, mpi_datatype, nbr_rank, mpi_tag, mpi_comm, req);
    //debugPrint("Rank %d <-> %d - received %d (TAG %d)", my_rank, nbr_rank, rcv_size, mpi_tag);
    return rcv_size;
}
