/**
 * @file dist_grid_checkerboard.c
 * @brief
 * @author
 */

#include "dist_grid_checkerboard.h"

/*
int ownerRank(MPI_Comm comm, int big_x, int small_x, int x,
                int big_y, int small_y, int y){

    int rank = -1;
    int coord[N_DIMS] = {0};

    coord[0] = (x < big_x)? 0 : (x - big_x) % small_x;
    coord[1] = (y < big_y)? 0 : (y - big_y) % small_y;
    MPI_Cart_rank(comm, coord, &rank);
    return rank;
}
*/

void insertLocalGraph(GraphNode ***graph, int offset_x, int offset_y, int x, int y, int z){

    int mapped_x = (offset_x == 0)? x : x % offset_x;
    int mapped_y = (offset_y == 0)? y : y % offset_y;
    graph[mapped_x][mapped_y] = graphNodeInsert(graph[mapped_x][mapped_y], z, ALIVE);
}

int main (int argc, char **argv) {

    /* Main program variables */

    /**< Global graph representation */
    GraphNode ***global_graph;
    /**< Local graph representation */
    GraphNode ***local_graph;

    /* Local graph dimensions */
    int local_x, local_y;
    int local_dim_x, local_dim_y;

    int n_processes, rank;

    /**< Size of the side of the cube space */
    int cube_size;
    /**< Number of generations to be computed */
    int generations;

    /* Auxiliary variables */
    int x, y, z;
    int i, j;
    int mpi_rv;
    char buffer[BUFFER_SIZE] = {0};
    GraphNode *it;

    /* Number of live cells to be sent to each neighbour */
    int snd_count_low_x;
    int snd_count_high_x;
    int snd_count_low_y;
    int snd_count_high_y;

    /* Frontier buffers to be sent to each neighbour */
    Node* snd_low_x;
    Node* snd_high_x;
    Node* snd_low_y;
    Node* snd_high_y;

    /* Number of live cells to be received from each neighbour */
    int rcv_count_low_x;
    int rcv_count_high_x;
    int rcv_count_low_y;
    int rcv_count_high_y;

    /* Frontier buffers to be received from each neighbour */
    Node* rcv_low_x;
    Node* rcv_high_x;
    Node* rcv_low_y;
    Node* rcv_high_y;

    /***********************************************************************************/

    /* MPI */
    MPI_Init(&argc, &argv);

    /* MPI Preamble */
    MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // TODO DEBUG
    if (rank == ROOT) debug_print("Number of processes %d", n_processes);

    /*MPI Create a structure to send over frontiers*/
    MPI_Datatype MPI_NEIGHBOUR_CELL, MPI_NEIGHBOUR_CELL_t, struct_type[3] = {MPI_INT, MPI_INT, MPI_INT};
    int struct_b_len[3] = {1, 1, 1};
    MPI_Aint struct_extent, struct_lb, struct_displs[3] = 	{offsetof(Node, x), offsetof(Node, y), offsetof(Node, z)};
    MPI_Type_create_struct(3, struct_b_len, struct_displs, struct_type, &MPI_NEIGHBOUR_CELL_t);
    MPI_Type_get_extent(MPI_NEIGHBOUR_CELL_t, &struct_lb, &struct_extent);
    MPI_Type_create_resized(MPI_NEIGHBOUR_CELL_t, -struct_lb, struct_extent, &MPI_NEIGHBOUR_CELL);
    MPI_Type_commit(&MPI_NEIGHBOUR_CELL);

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

    // TODO BEGIN DEBUG
    if (rank == ROOT){
        for (i = 0; i < N_DIMS; i++) {
            debug_print("Dimension %d has %d processes", i, dims[i]);
        }
    }
    // TODO END

    /* Get neighbours */
    /* axis = 0 (X) p[row-1] : curr_p : p[row+1] */
    /* axis = 1 (Y) p[col-1] : curr_p : p[col+1] */
    int nbr_low_x, nbr_high_x, nbr_low_y, nbr_high_y;
    MPI_Cart_shift(grid_comm, SHIFT_ROW, DISP, &nbr_low_x, &nbr_high_x);
    MPI_Cart_shift(grid_comm, SHIFT_COL, DISP, &nbr_low_y, &nbr_high_y);

    // TODO BEGIN DEBUG
    MPI_Barrier(grid_comm);
    debug_print("(%d,%d) Rank %d Neighbour x lo %d hi %d y: lo %d hi %d",
        coord[0], coord[1], cart_rank, nbr_low_x, nbr_high_x, nbr_low_y, nbr_high_y);
    // TODO END

    /***********************************************************************************/

    /* Parse arguments */

    /**< Input data file and file name */
    FILE* fp; char* file_name;

    parseArgs(argc, argv, &file_name, &generations);
    fp = fopen(file_name, "r");
    if(fp == NULL){
        fprintf(stderr, "Please input a valid file name\n" );
        exit(EXIT_FAILURE);
    }

    fgets(buffer,BUFFER_SIZE,fp);
    sscanf(buffer,"%d", &cube_size);

    /***********************************************************************************/

    /* Calculate block dimensions and offsets */

    int other_x = cube_size / dims[0];
    int first_x = (cube_size % other_x) + other_x;
    int other_y = cube_size / dims[1];
    int first_y = (cube_size % other_y) + other_y;

    /* Block offsets */
    int offset_x = (coord[0] == 0) ? 0 : first_x + (coord[0] - 1) * other_x;
    int offset_y = (coord[1] == 0) ? 0 : first_y + (coord[1] - 1) * other_y;
    /* Block dimensions */
    int dim_x = (coord[0] == 0) ? first_x : other_x;
    int dim_y = (coord[1] == 0) ? first_y : other_y;
    /* Maximum value of a local coordinate (local index <= MAX) */
    int max_x = offset_x + dim_x - 1;
    int max_y = offset_y + dim_y - 1;

    // TODO DEBUG
    MPI_Barrier(grid_comm);
    debug_print("(%d,%d) Rank %d offset_x %d offset y %d dim_x %d dim_y %d",
        coord[0], coord[1], cart_rank, offset_x, offset_y, dim_x, dim_y);

    /***********************************************************************************/

    // TODO DEBUG
    MPI_Barrier(grid_comm);

    /* Fill local graph structure */
    local_graph = initGraph(dim_x, dim_y);

    /* Read file and fill with the nodes that belong to the current process */
    while (fgets(buffer, BUFFER_SIZE, fp)){
        if (sscanf(buffer, "%d %d %d\n", &x, &y, &z) == 3){
            /* If I am the owner of the block the read data point is in */
            if ((offset_x <= x && x <= max_x) && (offset_y <= y && y <= max_y)){
                insertLocalGraph(local_graph, offset_x, offset_y, x, y, z);
                // TODO DEBUG
                //debug_print("(%d,%d) Rank %d - Inserting (%d,%d,%d)", coord[0], coord[1], cart_rank, x, y, z);
            }
        }
    }
    debug_print("(%d,%d) Rank %d - Finished inserting nodes", coord[0], coord[1], cart_rank);

    // TODO DEBUG
    MPI_Barrier(grid_comm);

    /***********************************************************************************/

    /* Allocate neighbour rows and columns that will be received */

    /* Neighbour rows */
    GraphNode** row_low_x = malloc(sizeof(GraphNode*) * dim_y);
    GraphNode** row_high_x = malloc(sizeof(GraphNode*) * dim_y);
    /* The x coordinates of each neighbour row */
    int nbr_coord_low_x = (offset_x == 0)? cube_size - 1 : offset_x - 1;
    int nbr_coord_high_x = (offset_x == 0)? cube_size - 1 : offset_x - 1;
    /* Neighbour columns */
    GraphNode** col_low_y = malloc(sizeof(GraphNode*) * dim_x);
    GraphNode** col_high_y = malloc(sizeof(GraphNode*) * dim_x);
    /* The y coordinates of each neighbour column */
    int nbr_coord_low_y = (offset_y == 0)? cube_size - 1 : offset_y - 1;
    int nbr_coord_high_y = (offset_y == 0)? cube_size - 1 : offset_y - 1;

    // TODO THE GENERATIONS LOOP SHOULD START ABOUT HERE
    // DECLARE VARIABLES OUTSIDE TO MINIMIZE HEAP

    /***********************************************************************************/

    /* Count number of live cells in each own frontier so they can be sent */
    snd_count_low_x = 0;
    snd_count_high_x = 0;
    snd_count_low_y = 0;
    snd_count_high_y = 0;

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
    snd_low_x = malloc(sizeof(Node) * snd_count_low_x);
    snd_high_x = malloc(sizeof(Node) * snd_count_high_x);
    snd_low_y = malloc(sizeof(Node) * snd_count_low_y);
    snd_high_y = malloc(sizeof(Node) * snd_count_high_y);
    snd_count_low_x=0; snd_count_low_y=0; snd_count_high_x=0; snd_count_high_y=0;
    /* Fill buffers */
    for (i = 0; i < dim_y; i++){
        for (it = local_graph[0][i]; it != NULL; it = it->next){
            if (it->state == ALIVE){
                snd_low_x[snd_count_low_x].x = 0;
                snd_low_x[snd_count_low_x].y = i;
                snd_low_x[snd_count_low_x].z = it->z;
                snd_count_low_x++;
            }
        }
        for (it = local_graph[dim_x - 1][i]; it != NULL; it = it->next){
            if (it->state == ALIVE){
                snd_high_x[snd_count_high_x].x = dim_x - 1;
                snd_high_x[snd_count_high_x].y = i;
                snd_high_x[snd_count_high_x].z = it->z;
                snd_count_high_x++;
            }
        }
    }
    for (i = 0; i < dim_x; i++){
        for (it = local_graph[i][0]; it != NULL; it = it->next){
            if (it->state == ALIVE){
                snd_low_y[snd_count_low_y].x = i;
                snd_low_y[snd_count_low_y].y = 0;
                snd_low_y[snd_count_low_y].z = it->z;
                snd_count_low_y++;
            }
        }
        for (it = local_graph[i][dim_y - 1]; it != NULL; it = it->next){
            if (it->state == ALIVE){
                snd_high_y[snd_count_high_y].x = i;
                snd_high_y[snd_count_high_y].y = dim_y - 1;
                snd_high_y[snd_count_high_y].z = it->z;
                snd_count_high_y++;
            }
        }
    }
    rcv_count_low_x = 1;
    rcv_count_high_x = 1;
    rcv_count_low_y = 1;
    rcv_count_high_y = 1;

    if(coord[0] == 0){ //First ranks in this row
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_low_x, nbr_low_x);
        MPI_Send(snd_low_x, snd_count_low_x, MPI_NEIGHBOUR_CELL, nbr_low_x, 0, MPI_COMM_WORLD);
        debug_print("Rank %d receiving from rank %d\n", rank, nbr_low_x);
        MPI_Status s1_x;
        MPI_Probe(nbr_low_x, 0, MPI_COMM_WORLD, &s1_x);
        MPI_Get_count(&s1_x, MPI_NEIGHBOUR_CELL, &rcv_count_low_x);
        rcv_low_x = malloc(sizeof(Node) * rcv_count_low_x);
        MPI_Recv(rcv_low_x, rcv_count_low_x, MPI_NEIGHBOUR_CELL, nbr_low_x, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        debug_print("Rank %d receiving from rank %d\n",rank, nbr_high_x);
        MPI_Status s2_x;
        MPI_Probe(nbr_high_x, 0, MPI_COMM_WORLD, &s2_x);
        MPI_Get_count(&s2_x, MPI_NEIGHBOUR_CELL, &rcv_count_high_x);
        rcv_high_x = malloc(sizeof(Node) * rcv_count_high_x);
        MPI_Recv(rcv_high_x, rcv_count_high_x, MPI_NEIGHBOUR_CELL, nbr_high_x, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_high_x, nbr_high_x);
        MPI_Send(snd_high_x, snd_count_high_x, MPI_NEIGHBOUR_CELL, nbr_high_x, 0, MPI_COMM_WORLD);
    }else{
        debug_print("Rank %d receiving from rank %d\n",rank, nbr_high_x);
        MPI_Status s2_x;
        MPI_Probe(nbr_high_x, 0, MPI_COMM_WORLD, &s2_x);
        MPI_Get_count(&s2_x, MPI_NEIGHBOUR_CELL, &rcv_count_high_x);
        rcv_high_x = malloc(sizeof(Node) * rcv_count_high_x);
        MPI_Recv(rcv_high_x, rcv_count_high_x, MPI_NEIGHBOUR_CELL, nbr_high_x, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_high_x, nbr_high_x);
        MPI_Send(snd_high_x, snd_count_high_x, MPI_NEIGHBOUR_CELL, nbr_high_x, 0, MPI_COMM_WORLD);

        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_low_x, nbr_low_x);
        MPI_Send(snd_low_x, snd_count_low_x, MPI_NEIGHBOUR_CELL, nbr_low_x, 0, MPI_COMM_WORLD);
        debug_print("Rank %d receiving from rank %d\n", rank, nbr_low_x);
        MPI_Status s1_x;
        MPI_Probe(nbr_low_x, 0, MPI_COMM_WORLD, &s1_x);
        MPI_Get_count(&s1_x, MPI_NEIGHBOUR_CELL, &rcv_count_low_x);
        rcv_low_x = malloc(sizeof(Node) * rcv_count_low_x);
        MPI_Recv(rcv_low_x, rcv_count_low_x, MPI_NEIGHBOUR_CELL, nbr_low_x, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if(coord[1] == 0){
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_low_y, nbr_low_y);
        MPI_Send(snd_low_y, snd_count_low_y, MPI_NEIGHBOUR_CELL, nbr_low_y, 0, MPI_COMM_WORLD);
        debug_print("Rank %d receiving from rank %d\n", rank, nbr_low_y);
        MPI_Status s1_y;
        MPI_Probe(nbr_low_y, 0, MPI_COMM_WORLD, &s1_y);
        MPI_Get_count(&s1_y, MPI_NEIGHBOUR_CELL, &rcv_count_low_y);
        rcv_low_y = malloc(sizeof(Node) * rcv_count_low_y);
        MPI_Recv(rcv_low_y, rcv_count_low_y, MPI_NEIGHBOUR_CELL, nbr_low_y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        debug_print("Rank %d receiving from rank %d\n",rank, nbr_high_y);
        MPI_Status s2_y;
        MPI_Probe(nbr_high_y, 0, MPI_COMM_WORLD, &s2_y);
        MPI_Get_count(&s2_y, MPI_NEIGHBOUR_CELL, &rcv_count_high_y);
        rcv_high_y = malloc(sizeof(Node) * rcv_count_high_y);
        MPI_Recv(rcv_high_y, rcv_count_high_y, MPI_NEIGHBOUR_CELL, nbr_high_y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_high_y, nbr_high_y);
        MPI_Send(snd_high_y, snd_count_high_y, MPI_NEIGHBOUR_CELL, nbr_high_y, 0, MPI_COMM_WORLD);

    }else{
        debug_print("Rank %d receiving from rank %d\n",rank, nbr_high_y);
        MPI_Status s2_y;
        MPI_Probe(nbr_high_y, 0, MPI_COMM_WORLD, &s2_y);
        MPI_Get_count(&s2_y, MPI_NEIGHBOUR_CELL, &rcv_count_high_y);
        rcv_high_y = malloc(sizeof(Node) * rcv_count_high_y);
        MPI_Recv(rcv_high_y, rcv_count_high_y, MPI_NEIGHBOUR_CELL, nbr_high_y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_high_y, nbr_high_y);
        MPI_Send(snd_high_y, snd_count_high_y, MPI_NEIGHBOUR_CELL, nbr_high_y, 0, MPI_COMM_WORLD);

        debug_print("Rank %d sending %d to rank %d\n",rank, snd_count_low_y, nbr_low_y);
        MPI_Send(snd_low_y, snd_count_low_y, MPI_NEIGHBOUR_CELL, nbr_low_y, 0, MPI_COMM_WORLD);
        debug_print("Rank %d receiving from rank %d\n", rank, nbr_low_y);
        MPI_Status s1_y;
        MPI_Probe(nbr_low_y, 0, MPI_COMM_WORLD, &s1_y);
        MPI_Get_count(&s1_y, MPI_NEIGHBOUR_CELL, &rcv_count_low_y);
        rcv_low_y = malloc(sizeof(Node) * rcv_count_low_y);
        MPI_Recv(rcv_low_y, rcv_count_low_y, MPI_NEIGHBOUR_CELL, nbr_low_y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    }
    debug_print("Rank %d finished exchanging frontiers\n",rank);

    // TODO - Proccess nodes that are not on our side of the frontier in this processor
    for(i = 1; i < dim_x - 1; i++){
        for(j = 1; j < dim_y - 1; j++){
            for(it = local_graph[i][j]; it != NULL; it = it->next){
                if(it->state == ALIVE){
                    //CUBE SIZE IS COMPLETLY IRRELEVANT
                    visitNeighbours(local_graph, cube_size, i, j, it->z);
                }
            }
        }
    }
    // TODO - Proccess nodes that are on our frontier

    // TODO - Taking in consideration all received frontiers update the frontiers in the previous step



    /* Allocate buffers to received sent */
    //rcv_low_x = malloc(sizeof(Node) * rcv_count_low_x);
    //rcv_high_x = malloc(sizeof(Node) * rcv_count_high_x);
    //rcv_low_y = malloc(sizeof(Node) * rcv_count_low_y);
    //rcv_high_y = malloc(sizeof(Node) * rcv_count_high_y);

    /***********************************************************************************/

    // CLEANUP BUFFERS

    /***********************************************************************************/

    // PROCESS GENERATION

    /***********************************************************************************/

    /* Clean up */
    freeGraph(local_graph, dim_x, dim_y);
    free(file_name);

    /* Force a synchronisation point and exit */
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}

/**
 *
 */
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
