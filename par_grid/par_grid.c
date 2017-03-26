/** @file seq_3d.c
 *  @brief Grid bruteforce in C
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "par_grid.h"
#include <omp.h>

#define BUFFER_SIZE 100

int main(int argc, char* argv[]){
    omp_set_num_threads(4);
    /**************************************************************************/
	char* file;             /**< Input data file name */
    int generations = 0, g;    /**< Number of generations to proccess */    
    parse_args(argc, argv, &file, &generations);
    printf("ARGS: file: %s generations: %d.\n", file, generations);
    int cube_size = 0;      /**< Size of the 3D space */
    double start,end,s,e,t; //Time measuring
    /**************************************************************************/

    Node**** graph0;           /**< The graph representation */
    Node**** graph1;           /**< The graph representation for next gen */
    Node**** temp;             /**< For swapping */

    /*Parsing files*/
    int first = 0;
    char line[BUFFER_SIZE];
    int x, y, z;    //Coordinates
    FILE* fp = fopen(file, "r");
    Node** hinsert;
    if(fp == NULL){
        printf("Incorrect file or path\n");
        exit(EXIT_FAILURE);
    }
    while(fgets(line, sizeof(line), fp)){ //fgets doesn't strip \n
        if(!first){
            if(sscanf(line, "%d\n", &cube_size) == 1){
                first = 1;
                graph0 = initGraph(cube_size);
                graph1 = initGraph(cube_size);
            }    
        }else{
            if(sscanf(line, "%d %d %d\n", &x, &y, &z) == 3){
                hinsert = graph0[x][y];
                insertCell(hinsert,z,ALIVE);
                hinsert = graph1[x][y];
                insertCell(hinsert,z,ALIVE);            
            }
        }
    }
    fclose(fp);

    omp_lock_t** graph0_lock = (omp_lock_t**)malloc(cube_size * sizeof(omp_lock_t*));
    omp_lock_t** graph1_lock = (omp_lock_t**)malloc(cube_size * sizeof(omp_lock_t*));
    for(x = 0; x < cube_size; x++){
        graph0_lock[x] = (omp_lock_t*) malloc(cube_size * sizeof(omp_lock_t));
        graph1_lock[x] = (omp_lock_t*) malloc(cube_size * sizeof(omp_lock_t));
        for(y = 0; y < cube_size; y++){
            omp_init_lock(&(graph0_lock[x][y]));
            omp_init_lock(&(graph1_lock[x][y]));
        }
    }


    for(g = 0; g < generations; g++){
        s = omp_get_wtime();
        start = omp_get_wtime();  // Start Timer
        visitNeighbours(graph0, graph1, cube_size, graph0_lock, graph1_lock);
        end = omp_get_wtime();  // End Timer
        printf("VisitNeighbours time: %f.\n", end - start);
        start = omp_get_wtime();  // Start Timer
        upgradeGraph(graph0, graph1, cube_size, graph1_lock);
        end = omp_get_wtime();  // End Timer
        printf("UpgradeGraph time: %f.\n", end - start);
        temp = graph1;
        graph1 = graph0;
        graph0 = temp;
        e = omp_get_wtime();
        t+=(e-s);
    }
    
    //Print the output
    /*
    for(x = 0; x < cube_size; x++){
        for(y = 0; y < cube_size; y++){
            Node *aux2;
            for(aux2 = *(graph0[x][y]); aux2 != NULL ; aux2 = aux2->next){
                if(aux2->status == ALIVE){
                    printf("%d %d %d - S = %d N = %d\n", x, y, aux2->z, aux2->status, aux2->counter);  
                }
            }
        }
    }*/
    printf("Total time %f\n", t);
    for(x = 0; x < cube_size; x++){
        for(y=0; y<cube_size; y++){
            omp_destroy_lock(&(graph0_lock[x][y]));
            omp_destroy_lock(&(graph1_lock[x][y]));
        }
    }
    exit(EXIT_SUCCESS);

}

/****************************************************************************/
void visitNeighbours(Node**** graph0, Node**** graph1, int cube_size, omp_lock_t** graph0_lock, omp_lock_t** graph1_lock ){
    //printf("Threads %d\n", omp_get_num_threads());
    Node* it;
    int x,y,z;
    //#pragma omp parallel for private(y)
    for(x=0; x < cube_size; x++){
        for(y=0; y < cube_size;y++){
            //printf("Visit Neihbours Thread %d\n", omp_get_thread_num());
            //omp_set_lock(&graph0_lock[x][y]);
            for(it = *(graph0[x][y]); it != NULL && it->status == ALIVE; it = it->next){
                z = it->z;
                int z1, z2;
                z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);
                int x1 = (x+1)%cube_size;
                int y1 = (y+1)%cube_size;
                int x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
                int y2 = (y-1) < 0 ? (cube_size-1) : (y-1);

                //omp_set_lock(&graph1_lock[x1][y]);
                    Node ** h = graph1[x1][y];
                    insertCell(h, z, DEAD);
                //omp_unset_lock(&graph1_lock[x1][y]);

                //omp_set_lock(&graph1_lock[x][y1]);
                    h = graph1[x][y1];
                    insertCell(h, z, DEAD);
                //omp_unset_lock(&graph1_lock[x][y1]);

                //omp_set_lock(&graph1_lock[x2][y]);
                    h = graph1[x2][y];
                    insertCell(h, z, DEAD);
                //omp_set_lock(&graph1_lock[x2][y]);

                //omp_set_lock(&graph1_lock[x][y2]);
                    h = graph1[x][y2];
                    insertCell(h, z, DEAD);
                //omp_set_lock(&graph1_lock[x][y2]);

                //omp_set_lock(&graph1_lock[x][y]);
                    Node** hz = graph1[x][y];
                    insertCell(hz, z1, DEAD);
                    insertCell(hz, z2, DEAD);
                //omp_set_lock(&graph1_lock[x][y]);

                //< it->status = DEAD;
            } 
            //omp_unset_lock(&graph0_lock[x][y]);
        }
    }
}

/****************************************************************************/
void upgradeGraph(Node**** graph0, Node**** graph1, int cube_size, omp_lock_t** graph1_lock){

    int x,y;
    #pragma omp parallel for private(x,y)
    for(x=0; x<cube_size;x++){
        for(y=0; y<cube_size; y++){
            //printf("%d\n", omp_get_thread_num());
            //printf("UpgradeGraph Thread %d\n", omp_get_thread_num());
            omp_set_lock(&graph1_lock[x][y]);
            Node* it;
            for(it = *(graph1[x][y]); it != NULL; it = it->next){
                int s = it->status;
                int c = it->counter;
                if(s == ALIVE){
                    if(c < 2 || c > 4){
                        it->status = DEAD;
                    }
                }else{
                    if(c == 2 || c == 3){
                        it->status = ALIVE;
                    }
                }
                it->counter = 0; 
            }
            omp_unset_lock(&graph1_lock[x][y]);
        }
    }
}



/****************************************************************************/
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
