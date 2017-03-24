/** @file seq_3d.c
 *  @brief 3D Matrix with duplicate for update
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "seq_3d.h"

int main(int argc, char* argv[]){

    char* file;             /**< Input data file name */
    int generations = 0;    /**< Number of generations to proccess */
    int cube_size = 0;      /**< Size of the 3D space */
    
    bool*** graph0;         /**< The graph representation */
    bool*** graph1;         /**< A copy of the graph */                       

    int g,x,y,z;

    parse_args(argc, argv, &file, &generations);
    printf("ARGS: file: %s generations: %d.\n", file, generations);

    double start = omp_get_wtime();  // Start Timer

    graph0 = parse_file(file, &cube_size);
    graph1 = initGraph(cube_size);

    for(g = 1; g <= generations; g++){
        
        /* Check each node */
        for (x = 0; x < cube_size; ++x){
            for (y = 0; y < cube_size; ++y){
                for (z = 0; z < cube_size; ++z){
                    graph1[x][y][z] = setNextState(graph0, cube_size, x, y, z);
                }
            }
        }
        bool*** tmp = graph0;
        graph0 = graph1;
        graph1 = tmp;
    }

    double end = omp_get_wtime();   // Stop Timer
    
    /* Print the final set of live cells */
    printActive(graph1, cube_size);

    printf("Total Runtime: %f.\n", end - start);
    
    freeGraph(graph0, cube_size);
    freeGraph(graph1, cube_size);
    free(file);

    return 0;
}

bool*** initGraph(int size){

    int i, j, k;
    bool*** matrix = (bool***) malloc(sizeof(bool**) * size);
    
    for (i = 0; i < size; i++){
        matrix[i] = (bool**) malloc(sizeof(bool*) * size);
        for (j = 0; j < size; j++){
            matrix[i][j] = (bool*) malloc(sizeof(bool) * size);
            for(k = 0; k < size; k++){
                matrix[i][j][k] = DEAD;
            }
        }
    }
    return matrix;
}

void freeGraph(bool*** graph, int size){

    int i, j, k;
    if (graph != NULL){
        for (i = 0; i < size; i++){
            for (j = 0; j < size; j++){
                free(graph[i][j]);
            }
            free(graph[i]);
        }
        free(graph);
    }
}

void printActive(bool*** graph, int cube_size){
    int x,y,z;
    for (x = 0; x < cube_size; ++x){
        for (y = 0; y < cube_size; ++y){
            for (z = 0; z < cube_size; ++z){
                if (graph[x][y][z])
                    printf("x:%d y:%d z:%d\n", x,y,z);
            }
        }
    }
}

bool setNextState(bool*** graph, int cube_size, int x, int y, int z){
    int live_neighbours = liveNeighbors(graph, cube_size, x, y, z);
    bool state = graph[x][y][z];
    if (state == ALIVE){
        if(live_neighbours < 2 || live_neighbours > 4){
            return DEAD;
        }
    }else if(state == DEAD){
        if(live_neighbours == 2 || live_neighbours == 3){
            return ALIVE;
        }   
    }
    return state;
}

int liveNeighbors(bool*** graph, int cube_size, int x, int y, int z){
    int live_neighbours = 0;
    int x1,x2,y1,y2,z1,z2;

    x1 = (x+1)%cube_size; x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
    y1 = (y+1)%cube_size; y2 = (y-1) < 0 ? (cube_size-1) : (y-1);
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);

    if(graph[x1][y][z]){live_neighbours++;}
    if(graph[x2][y][z]){live_neighbours++;}
    if(graph[x][y1][z]){live_neighbours++;}
    if(graph[x][y2][z]){live_neighbours++;}
    if(graph[x][y][z1]){live_neighbours++;}
    if(graph[x][y][z2]){live_neighbours++;}

    return live_neighbours;
}

void parse_args(int argc, char* argv[], char** file, int* generations){
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

bool*** parse_file(char* file, int* cube_size){
    
    int first = 0;
    char line[100];
    int x, y, z;    //Coordinates
    FILE* fp = fopen(file, "r");
    bool*** graph;

    while(fgets(line, sizeof(line), fp)){ //fgets doesn't strip \n
        if(!first){
            if(sscanf(line, "%d\n", cube_size) == 1){
                first = 1;
                graph = initGraph(*cube_size);
            }    
        }else{
            if(sscanf(line, "%d %d %d\n", &x, &y, &z) == 3)
                graph[x][y][z] = ALIVE;
        }
    }
    fclose(fp);
    return graph;
}
