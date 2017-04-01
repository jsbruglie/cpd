/** @file  par_grid_hash.c
 *  @brief  Parallel 2D Matrix with a hash
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "par_grid_hash.h"

#define NUM_THREADS 4    /**< Number of threads */
#define HASHRATIO 8
#define INITIAL_ALIVE_NUMBER 50

int main(int argc, char* argv[]){

    omp_set_num_threads(NUM_THREADS); /**< Set number of threads*/

    char* file;                 /**< Input data file name */
    int generations = 0;        /**< Number of generations to proccess */
    int cube_size = 0;          /**< Size of the 3D space */
    
    GraphNode*** graph;         /**< Graph representation - 2D array of lists */
    Hashtable* hashtable;       /**< Contains the information of nodes that are alive */

    /* Iterator variables */
    int g, i, j;
    GraphNode* g_it = NULL;
    Node* it = NULL;


    /* Lock variable */
    omp_lock_t** graph_lock;

    parseArgs(argc, argv, &file, &generations);
    int initial_alive = getAlive(file);
    
    /* Create the hashtable */
    hashtable = createHashtable(HASHRATIO * INITIAL_ALIVE_NUMBER); 

    graph = parseFile(file, hashtable, &cube_size);
    printHashtable(hashtable); //DEBUG -- print the hashtable
    
    /* Initialize lock variables */
    
    graph_lock = (omp_lock_t**)malloc(cube_size * sizeof(omp_lock_t*));
    for(i = 0; i < cube_size; i++){
        graph_lock[i] = (omp_lock_t*) malloc(cube_size * sizeof(omp_lock_t));
        for(j = 0; j < cube_size; j++){
            omp_init_lock(&(graph_lock[i][j]));
        }
    }

    double start = omp_get_wtime();  // Start Timer
    /* Generations */
    for(g = 1; g <= generations; g++){
        
        /* Convert hashtable to vector */
        int num_alive = hashtable->occupied; //Number of alive nodes the hashtable has
        Node** vector = (Node**) malloc(sizeof(Node*) * num_alive); //Vector has alive nodes
        /* Go through the hashtable and copy all nodes found to vector*/
        i=0;
        for(j=0; j<hashtable->size ; j++){
            for (it = listFirst(hashtable->table[j]); it != NULL; it = it->next){
                if(it != NULL)
                    vector[i++] = it;
            }            
        }

        /* Create the num_alive * 6 matrix that will store the neighbours of each alive node*/
        Node*** neighbour_vector = (Node***)malloc(sizeof(Node**) * num_alive);
        for(i=0; i<num_alive; i++){
            neighbour_vector[i] = (Node**)malloc(sizeof(Node*) * 6);
            for(j=0; j<6; j++)
                neighbour_vector[i][j] = NULL;
        }


        /* For each live node, inform its neighbors, mistake is likely here */
        for (i = 0; i < num_alive; i++){
            //Get the coordinates from the alive node
            coordinate x = vector[i]->x; 
            coordinate y = vector[i]->y; 
            coordinate z = vector[i]->z;
            
            coordinate x1, x2, y1, y2, z1, z2;
            x1 = (x+1)%cube_size; x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
            y1 = (y+1)%cube_size; y2 = (y-1) < 0 ? (cube_size-1) : (y-1);
            z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);
            GraphNode* ptr;

            /* Create nodes whose ->first = NULL and that have NULL pointers*/
            
            if(graphNodeAddNeighbour(&(graph[x1][y]), z, &ptr, &graph_lock[x1][y])){
                neighbour_vector[i][0] = nodeInsert(NULL, x1, y, z, ptr);
            }else{
                neighbour_vector[i][0] = NULL;
            }
            if(graphNodeAddNeighbour(&(graph[x2][y]), z, &ptr, &graph_lock[x2][y])){
                neighbour_vector[i][1] = nodeInsert(NULL, x2, y, z, ptr);
            }else{
                neighbour_vector[i][1] = NULL;
            }
            if(graphNodeAddNeighbour(&(graph[x][y1]), z, &ptr, &graph_lock[x][y1])){
                neighbour_vector[i][2] = nodeInsert(NULL, x, y1, z, ptr);
            }else{
                neighbour_vector[i][2] = NULL;
            }
            if(graphNodeAddNeighbour(&(graph[x][y2]), z, &ptr, &graph_lock[x][y2])){
                neighbour_vector[i][3] = nodeInsert(NULL, x, y2, z, ptr);
            }else{
                neighbour_vector[i][3] = NULL;
            }
            if(graphNodeAddNeighbour(&(graph[x][y]), z1, &ptr, &graph_lock[x][y])){
                neighbour_vector[i][4] = nodeInsert(NULL, x, y, z1, ptr);
            }else{
                neighbour_vector[i][4] = NULL;
            }
            if(graphNodeAddNeighbour(&(graph[x][y]), z2, &ptr, &graph_lock[x][y])){
                neighbour_vector[i][5] = nodeInsert(NULL, x, y, z2, ptr);
            }else{
                neighbour_vector[i][5] = NULL;
            }
            
        }




        //Process the neighbours and the alive node
        for(i=0; i < num_alive; i++){
            /* Process alive nodes that are in vector*/
            Node* it;
            it = vector[i];
            unsigned char live_neighbours = it->ptr->neighbours;
            it->ptr->neighbours = 0;
            if(it->ptr->state == ALIVE){
                if(live_neighbours < 2 || live_neighbours > 4){
                    it->ptr->state = DEAD;
                    graphNodeRemove(&(graph[it->x][it->y]), it->z, &(graph_lock[it->x][it->y]));

                    hashtableRemove(hashtable, it->x, it->y, it->z); //Remove from hashtable
                }                        
            }
            int j;
            for(j = 0; j < 6; j++){
                /*For all other nodes that should be in the matrix*/
                it = neighbour_vector[i][j];
                if(it != NULL){
                    unsigned char live_neighbours = it->ptr->neighbours;
                    it->ptr->neighbours = 0;
                    if(it->ptr->state == DEAD){

                        if(live_neighbours == 2 || live_neighbours == 3){
                            it->ptr->state = ALIVE;
                            //printf("Inserting ");
                            hashtableWrite(hashtable, it->x, it->y, it->z, it->ptr); //Insert in hashtable
                        }
                        else{
                            graphNodeRemove(&(graph[it->x][it->y]), it->z, &(graph_lock[it->x][it->y]));
                        }
                    }
                }
                
            }
        }
        
        /* Cleanup matrix and vector*/
        for(i=0; i<num_alive; i++){
            for(j=0; j<6; j++)
                free(neighbour_vector[i][j]);

            free(neighbour_vector[i]);
        }
        free(neighbour_vector);
        free(vector);
    }

    double end = omp_get_wtime();   // Stop Timer
    
    /* Print the final set of live cells */
    printAndSortActive(graph, cube_size);
    printf("Total Runtime: %f.\n", end - start);
    printToFile(graph, cube_size, generations, file);
    /* Free resources */
    freeGraph(graph, cube_size);
    
    hashtableFree(hashtable);    

    for(i = 0; i < cube_size; i++){
        for(j=0; j<cube_size; j++){
            omp_destroy_lock(&(graph_lock[i][j]));
        }
    }
    free(file);

    return 0;
}


/**************************************************************************/
GraphNode*** initGraph(int size){

    int i,j;
    GraphNode*** graph = (GraphNode***) malloc(sizeof(GraphNode**) * size);

    for (i = 0; i < size; i++){
        graph[i] = (GraphNode**) malloc(sizeof(GraphNode*) * size);
        for (j = 0; j < size; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

/**************************************************************************/
void freeGraph(GraphNode*** graph, int size){

    int i, j;
    if (graph != NULL){
        for (i = 0; i < size; i++){
            for (j = 0; j < size; j++){
                graphNodeDelete(graph[i][j]);
            }
            free(graph[i]);
        }
        free(graph);
    }
}

/**************************************************************************/
void printAndSortActive(GraphNode*** graph, int cube_size){
    int x,y;
    GraphNode* it;
    for (x = 0; x < cube_size; ++x){
        for (y = 0; y < cube_size; ++y){
            /* Sort the list by ascending coordinate z */
            graphNodeSort(&(graph[x][y]));
            for (it = graph[x][y]; it != NULL; it = it->next){    
                /* At the end of each generation, the graph is guranteed to only have live cells */
                printf("%d %d %d\n", x, y, it->z);
            }
        }
    }
}

/**************************************************************************/
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

/**************************************************************************/
GraphNode*** parseFile(char* file, Hashtable* hashtable, int* cube_size){
    
    int first = 0;
    char line[BUFFER_SIZE];
    int x, y, z;
    FILE* fp = fopen(file, "r");
    if(fp == NULL){
        fprintf(stderr, "Please input a valid file name\n");
        exit(EXIT_FAILURE);
    }
    GraphNode*** graph;

    while(fgets(line, sizeof(line), fp)){
        if(!first){
            if(sscanf(line, "%d\n", cube_size) == 1){
                first = 1;
                graph = initGraph(*cube_size);
            }    
        }else{
            if(sscanf(line, "%d %d %d\n", &x, &y, &z) == 3){
                /* Insert live nodes in the graph and the update set */
                graph[x][y] = graphNodeInsert(graph[x][y], z, ALIVE);
                hashtableWrite(hashtable, x, y, z, (GraphNode*)(graph[x][y]));                
            }
        }
    }

    fclose(fp);
    return graph;
}

/**************************************************************************/
void printToFile(GraphNode*** graph, int cube_size, int generations, char* file){
    char base_name[255];
    int n = strstr(file,"in") - file;
    strncpy(base_name, file, n);
    printf("%s\n", base_name);
    const char separator = '/';
    char * const sep_at = strrchr(base_name, separator);
    *sep_at = '\0';
    char* name = sep_at + 1;
    char gen_str[255];
    sprintf(gen_str, "%d", generations);
    strcat(name, gen_str);
    strcat(name, ".out");
    printf("%s\n", name);
    FILE* output_fd = fopen(name, "w");
    //fprintf(output_fd, "%d\n", cube_size);
    int x,y;
    GraphNode* it;
    for (x = 0; x < cube_size; ++x){
        for (y = 0; y < cube_size; ++y){
            for (it = graph[x][y]; it != NULL; it = it->next){    
                /* At the end of each generation, the graph is guranteed to only have live cells */
                fprintf(output_fd, "%d %d %d\n", x, y, it->z);
            }
        }
    }
}


int getAlive(char* file){
    char base_name[255];
    int n = strstr(file,"in") - file;
    strncpy(base_name, file, n);
    printf("%s\n", base_name);
    const char separator = '/';
    char * const sep_at = strrchr(base_name, separator);
    *sep_at = '\0';
    char* name = sep_at + 1;
}
