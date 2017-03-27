#include "lists.h"

/* NodeGraph Lists related functions */

GraphNode* graphNodeInsert(GraphNode* first, coordinate z, bool state){

    GraphNode* new = (GraphNode*) malloc(sizeof(GraphNode));
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

void graphNodeRemove(GraphNode** first_ptr, coordinate z){
    GraphNode** cur;
    for (cur = first_ptr; *cur; ){
        GraphNode* entry = *cur;
        if (entry->z == z){
            *cur = entry->next;
            free(entry);
        }else{
            cur = &entry->next;
        }
    }
}

void graphNodeDelete(GraphNode* first){
    GraphNode* it, *next;
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }
}

bool graphNodeAddNeighbour(GraphNode** first, coordinate z){
    GraphNode* it;
    /* Search for the node */
    for(it = *first; it != NULL; it = it->next){
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

void graphNodeSort(GraphNode** first_ptr){
    GraphNode* i, *j;
    if (*first_ptr != NULL){
        for(i = *first_ptr; i->next != NULL; i = i->next){
            for(j = i->next; j != NULL; j = j->next)
            {
                if(i->z > j->z){
                    coordinate tmp_z = i->z; bool tmp_state = i->state;
                    i->z = j->z; i->state = j->state;
                    j->z = tmp_z; j->state = tmp_state;
                }
            }
        }
    }
}