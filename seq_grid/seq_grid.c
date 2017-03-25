/** @file seq_3d.c
 *  @brief Grid with lists in C
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "seq_grid.h"

int main(int argc, char* argv[]){
	char* file;             /**< Input data file name */
    int generations = 0;    /**< Number of generations to proccess */    
    parse_args(argc, argv, &file, &generations);
    printf("ARGS: file: %s generations: %d.\n", file, generations);

    int cube_size = 0;      /**< Size of the 3D space */
    Node*** grid;           /**< The graph representation */
    
    grid = parse_file(file, &cube_size);

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