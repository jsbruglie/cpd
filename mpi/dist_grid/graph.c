#include "graph.h"

graph_node* graphNodeInsert(graph_node* first, int z, int state){

    graph_node* new = (graph_node*) malloc(sizeof(graph_node));
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

graph_node*** initGraph(int size){
    int i,j;
    graph_node*** graph = (graph_node***) malloc(sizeof(graph_node**) * size);

    for (i = 0; i < size; i++){
        graph[i] = (graph_node**) malloc(sizeof(graph_node*) * size);
        for (j = 0; j < size; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

graph_node*** initLocalGraph(int bsize, int size){

    int i,j;
    graph_node*** graph = (graph_node***) malloc(sizeof(graph_node**) * bsize);

    for (i = 0; i < bsize; i++){
        graph[i] = (graph_node**) malloc(sizeof(graph_node*) * size);
        for (j = 0; j < size; j++){
            graph[i][j] = NULL;
        }
    }
    return graph;
}

bool graphNodeAddNeighbour(graph_node** first, int z){
    graph_node* it;
    /* Search for the node */
    for(it = *first; it != NULL; it = it->next){
        if (it->z == z){
            it->neighbours++;
            return false;
        }
    }

    /* Need to insert the node */
    graph_node* new = graphNodeInsert(*first, z, DEAD);
    new->neighbours++;
    *first = new;
    return true;
}

void visitNeighbours(graph_node*** graph, int cube_size, int x, int y, int z){

    graph_node* ptr;
    int x1, x2, y1, y2, z1, z2;
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

void printAndSortActive(graph_node*** graph, int size){
    int x,y;
    graph_node* it;
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

void graphNodeSort(graph_node** first_ptr){
    graph_node* i, *j;
    if (*first_ptr != NULL){
        for(i = *first_ptr; i->next != NULL; i = i->next){
            for(j = i->next; j != NULL; j = j->next)
            {
                if(i->z > j->z){
                    int tmp_z = i->z; bool tmp_state = i->state;
                    i->z = j->z; i->state = j->state;
                    j->z = tmp_z; j->state = tmp_state;
                }
            }
        }
    }
}

void freeGraph(graph_node*** graph, int size){

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

void graphNodeDelete(graph_node* first){
    graph_node* it, *next;
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }
}

void graphListCleanup(graph_node** head){
    graph_node *temp, *prev;
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
