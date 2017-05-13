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

bool graphNodeAddNeighbour(GraphNode** first, coordinate z, omp_lock_t* lock_ptr){
    GraphNode* it;
    omp_set_lock(lock_ptr);
    /* Search for the node */
    for(it = *first; it != NULL; it = it->next){
        if (it->z == z){
            it->neighbours++;
            omp_unset_lock(lock_ptr);
            return false;
        }
    }
    
    /* Need to insert the node */
    GraphNode* new = graphNodeInsert(*first, z, DEAD);
    new->neighbours++;
    *first = new;
    omp_unset_lock(lock_ptr);
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

