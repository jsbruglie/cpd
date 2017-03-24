/** @file seq_3d.c
 *  @brief 3D Matrix with duplicate for update
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#include "seq_grid_list.h"

int main(int argc, char* argv[]){

    char* file;             /**< Input data file name */
    int generations = 0;    /**< Number of generations to proccess */
    int cube_size = 0;      /**< Size of the 3D space */
    
    GraphNode*** graph;     /**< Graph representation - 2D array of lists */
    List* update;           /**< Contains the information of nodes that might change state */

    int g;
    coordinate x,y,z;
    GraphNode* g_it = NULL;
    Node* it = NULL;

    parse_args(argc, argv, &file, &generations);
    printf("ARGS: file: %s generations: %d.\n", file, generations);

    update = listCreate();

    double start = omp_get_wtime();  // Start Timer

    graph = parse_file(file, update, &cube_size);
    
    // DEBUG
     
    printf("DEBUG\n");
    printf("GRAPH\n");
    printActive(graph, cube_size);
    printf("UPDATE SET\n");
    for (it = update->first; it != NULL; it = it->next){
        printf("x:%d y:%d: z:%d pointing to z:%d\n", it->x, it->y, it->z, it->ptr->z);
    } 

    for(g = 1; g <= generations; g++){
        
        //printf("GENERATION %d\n", g);

        /* Convert lit to vector */
        Node** vector = (Node**) malloc(sizeof(Node*) * update->size);
        int i = 0;
        for (it = listFirst(update); it != NULL; it = it->next){
            vector[i++] = it;
        }

        int size = update->size;
        /* Per live node, inform its neighbors */
        for (i = 0; i < size; i++){
            printf("[%d] ALIVE x:%d y:%d z:%d\n", g, vector[i]->x, vector[i]->y, vector[z]->z);
            visitNeighbours(graph, cube_size, update, vector[i]->x, vector[i]->y, vector[i]->z);
        }

        free(vector);

        /*
        for (it = listFirst(update); it != NULL; it = it->next){
            printf("[%d] COUNT - x:%d y:%d: z:%d pointing to z:%d state:%d\n", g, it->x, it->y, it->z, it->ptr->z, it->ptr->state);
        }
        */ 
        

        /* Update graph and update set */         
        for (it = listFirst(update); it != NULL; it = it->next){
            //printf("\t[%d] UPDATE - x:%d y:%d z:%d state:%d neighbours:%d\n", g, it->x, it->y, it->z, it->ptr->state, it->ptr->neighbours);
            unsigned char live_neighbours = it->ptr->neighbours;
            it->ptr->neighbours = 0;
            if(it->ptr->state == ALIVE){
                if(live_neighbours < 2 || live_neighbours > 4){
                    //printf("TRYING TO REMOVE x:%d y:%d z:%d pointing to %d\n", it->x, it->y, it->z, it->ptr->z);
                    graphNodeRemove(&(graph[it->x][it->y]), it->z);
                    it->x = REMOVE;
                }
            }else{
                if(live_neighbours == 2 || live_neighbours == 3){
                    it->ptr->state = ALIVE; 
                }
            }
        }
        /* Clean dead cells from the set */
        listCleanup(update);
    }

    double end = omp_get_wtime();   // Stop Timer
    
    /* Print the final set of live cells */
    printActive(graph, cube_size);

    printf("Total Runtime: %f.\n", end - start);
    
    freeGraph(graph, cube_size);
    listDelete(update);
    free(file);

    return 0;
}

void visitNeighbours(GraphNode*** graph, int cube_size, List* list, coordinate x, coordinate y, coordinate z){

    GraphNode* ptr;
    coordinate x1, x2, y1, y2, z1, z2;
    x1 = (x+1)%cube_size; x2 = (x-1) < 0 ? (cube_size-1) : (x-1);
    y1 = (y+1)%cube_size; y2 = (y-1) < 0 ? (cube_size-1) : (y-1);
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);
    /* If a cell already has 2 live neighbours it is added to the update list, for fast access */
    if(graphNodeAddNeighbour(&graph[x1][y], z, &ptr)){ listInsert(list, x1, y, z, ptr); }
    if(graphNodeAddNeighbour(&graph[x2][y], z, &ptr)){ listInsert(list, x2, y, z, ptr); }
    if(graphNodeAddNeighbour(&graph[x][y1], z, &ptr)){ listInsert(list, x, y1, z, ptr); }
    if(graphNodeAddNeighbour(&graph[x][y2], z, &ptr)){ listInsert(list, x, y2, z, ptr); }
    if(graphNodeAddNeighbour(&graph[x][y], z1, &ptr)){ listInsert(list, x, y, z1, ptr); }
    if(graphNodeAddNeighbour(&graph[x][y], z2, &ptr)){ listInsert(list, x, y, z2, ptr); }
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

void printActive(GraphNode*** graph, int cube_size){
    int x,y;
    GraphNode* it;
    for (x = 0; x < cube_size; ++x){
        for (y = 0; y < cube_size; ++y){
            graphNodeSort(&(graph[x][y]));
            for (it = graph[x][y]; it != NULL; it = it->next){    
                if (it->state == ALIVE){
                    printf("x:%d y:%d z:%d\n", x, y, it->z);
                }else{
                    //printf("x:%d y:%d z:%d [DEAD]\n", x, y, it->z);
                }
            }
        }
    }
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

GraphNode*** parse_file(char* file, List* list, int* cube_size){
    
    int first = 0;
    char line[100];
    int x, y, z;    //Coordinates
    FILE* fp = fopen(file, "r");
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
                listInsert(list, x, y, z, graph[x][y]);                
            }
        }
    }

    fclose(fp);
    return graph;
}



