#include "graph.h"

GraphNode* graphNodeInsert(GraphNode *first, int z, int state){

    GraphNode *new = malloc(sizeof(GraphNode));
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

GraphNode*** initGraph(int dim_x, int dim_y){
    int i,j;
    GraphNode ***graph = malloc(sizeof(GraphNode**) * dim_x);

    for (i = 0; i < dim_x; i++){
        graph[i] = malloc(sizeof(GraphNode*) * dim_y);
        for (j = 0; j < dim_y; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

bool graphNodeAddNeighbour(GraphNode **first, int z){
    GraphNode *it;
    /* Search for the node */
    for (it = *first; it != NULL; it = it->next){
        if (it->z == z){
            it->neighbours++;
            return false;
        }
    }

    /* Need to insert the node */
    GraphNode* new = graphNodeInsert(*first, z, DEAD);
    new->neighbours++;
    *first = new;
    return true;
}

void visitInternalNeighbours(GraphNode ***local_graph, int cube_size, int x, int y, int z){

    /**
     * @attention x+1, x-1, y+1, y-1 are guaranteed to be valid indices,
     * since (x,y,z) is considered to be internal, i.e., not on a border
     */
    
    int z1, z2;
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);
    
    graphNodeAddNeighbour(&(local_graph[x+1][y]), z);
    graphNodeAddNeighbour(&(local_graph[x-1][y]), z);
    graphNodeAddNeighbour(&(local_graph[x][y+1]), z);
    graphNodeAddNeighbour(&(local_graph[x][y-1]), z);
    graphNodeAddNeighbour(&(local_graph[x][y]), z1);
    graphNodeAddNeighbour(&(local_graph[x][y]), z2);
}

void visitBoundaryNeighbours(GraphNode ***local_graph, int cube_size,
    int dim_x, int dim_y, int x, int y, int z){

    int z1, z2;
    z1 = (z+1)%cube_size; z2 = (z-1) < 0 ? (cube_size-1) : (z-1);

    if (x+1 < dim_x){ graphNodeAddNeighbour(&(local_graph[x+1][y]), z);}
    if (x-1 >= 0){ graphNodeAddNeighbour(&(local_graph[x-1][y]), z); }
    if (y+1 < dim_y){ graphNodeAddNeighbour(&(local_graph[x][y+1]), z);}
    if (y-1 >= 0){ graphNodeAddNeighbour(&(local_graph[x][y-1]), z); }
    graphNodeAddNeighbour(&(local_graph[x][y]), z1);
    graphNodeAddNeighbour(&(local_graph[x][y]), z2);
}

void printAndSortActiveSub(GraphNode ***graph, int offset_x, int offset_y, int dim_x, int dim_y){
    
    int x,y;
    GraphNode* it;
    for (x = 0; x < dim_x; ++x){
        for (y = 0; y < dim_y; ++y){
            /* Sort the list by ascending coordinate z */
            graphNodeSort(&(graph[x][y]));
            for (it = graph[x][y]; it != NULL; it = it->next){
                if (it->state == ALIVE){
                    printf("%d %d %d\n", x + offset_x, y + offset_y, it->z);
                    fflush(stdin);
                }
            }
        }
    }
}

void printAndSortActive(GraphNode ***graph, int size){
    int x,y;
    GraphNode *it;
    for (x = 0; x < size; ++x){
        for (y = 0; y < size; ++y){
            /* Sort the list by ascending coordinate z */
            graphNodeSort(&(graph[x][y]));
            for (it = graph[x][y]; it != NULL; it = it->next){
                if (it->state == ALIVE)
                    printf("%d %d %d\n", x, y, it->z);
            }
        }
    }
}

void graphNodeSort(GraphNode **first_ptr){
    GraphNode *i, *j;
    int tmp_z; bool tmp_state;
    if (*first_ptr != NULL){
        for (i = *first_ptr; i->next != NULL; i = i->next){
            for (j = i->next; j != NULL; j = j->next){
                if (i->z > j->z){
                    tmp_z = i->z; tmp_state = i->state;
                    i->z = j->z; i->state = j->state;
                    j->z = tmp_z; j->state = tmp_state;
                }
            }
        }
    }
}

void freeGraph(GraphNode ***graph, int dim_x, int dim_y){

    int i, j;
    if (graph != NULL){
        for (i = 0; i < dim_x; i++){
            for (j = 0; j < dim_y; j++){
                graphNodeDelete(graph[i][j]);
            }
            free(graph[i]);
        }
        free(graph);
    }
}

void graphNodeDelete(GraphNode *first){
    GraphNode *it, *next;
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }
}

void graphListCleanup(GraphNode** head){
    GraphNode *temp, *prev;
    if(*head != NULL){
        temp = *head;
        /* Delete from the beginning */
        while(temp != NULL && temp->state == DEAD){
            *head = temp->next;
            free(temp);
            temp = *head;
        }
        /*Delete from the middle*/
        while(temp != NULL){
            while (temp != NULL && temp->state != DEAD){
                prev = temp;
                temp = temp->next;
            }
            if(temp == NULL)
                return;

            prev->next = temp->next;
            free(temp);
            temp = prev->next;
        }
    }
}
