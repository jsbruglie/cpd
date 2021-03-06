#include "life3d.h"

int main(int argc, char* argv[]){

    char* file;             /**< Input data file name */
    int generations = 0;    /**< Number of generations to proccess */
    int cube_size = 0;      /**< Size of the 3D space */

    GraphNode*** graph;     /**< Graph representation - 2D array of lists */

    int g, i, j;
    GraphNode* it;
    int live_neighbours;

    parseArgs(argc, argv, &file, &generations);
    debug_print("ARGS: file: %s generations: %d", file, generations);

    double start = omp_get_wtime();  // Start Timer
    graph = parseFile(file, &cube_size);

    for(g = 1; g <= generations; g++){
        
        /* First passage in the graph - notify neighbours */
        for(i = 0; i < cube_size; i++){
            for(j = 0; j < cube_size; j++){
                for(it = graph[i][j]; it != NULL; it = it->next){
                    if(it->state == ALIVE)
                        visitNeighbours(graph, cube_size, i, j, it->z);
                }
            }
        }
        /* Second passage in the graph - decide next state */
        for(i = 0; i < cube_size; i++){
            for(j = 0; j < cube_size; j++){
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
        /* Remove dead nodes from the graph every REMOVAL_PERIOD generations */
        if(g % REMOVAL_PERIOD == 0){
            for(i = 0; i < cube_size; i++){
                for(j = 0; j < cube_size; j++){
                    GraphNode ** list = &graph[i][j];
                    graphListCleanup(list);
                }
            }
        }
    }

    double end = omp_get_wtime();   // Stop Timer

    /* Print the final set of live cells */
    printAndSortActive(graph, cube_size);

    time_print("%f\n", end - start);

    freeGraph(graph, cube_size);
    free(file);
    return(EXIT_SUCCESS);
}

void visitNeighbours(GraphNode*** graph, int cube_size, coordinate x, coordinate y, coordinate z){

    GraphNode* ptr;
    coordinate x1, x2, y1, y2, z1, z2;
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

void printAndSortActive(GraphNode*** graph, int cube_size){
    int x,y;
    GraphNode* it;
    for (x = 0; x < cube_size; ++x){
        for (y = 0; y < cube_size; ++y){
            /* Sort the list by ascending coordinate z */
            graphNodeSort(&(graph[x][y]));
            for (it = graph[x][y]; it != NULL; it = it->next){    
                if (it->state == ALIVE)
                    out_print("%d %d %d\n", x, y, it->z);
            }
        }
    }
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

GraphNode*** parseFile(char* file, int* cube_size){
    
    int first = 0;
    char line[BUFFER_SIZE];
    int x, y, z;
    FILE* fp = fopen(file, "r");
    if(fp == NULL){
        err_print("Please input a valid file name");
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
            }
        }
    }

    fclose(fp);
    return graph;
}
