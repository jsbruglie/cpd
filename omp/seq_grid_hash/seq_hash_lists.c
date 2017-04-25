#include "seq_hash_lists.h"

/* NodeGraph Lists related functions */

/**************************************************************************/
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

/**************************************************************************/
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

/**************************************************************************/
void graphNodeDelete(GraphNode* first){
    GraphNode* it, *next;
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }
}

/**************************************************************************/
bool graphNodeAddNeighbour(GraphNode** first, coordinate z, GraphNode** ptr){
    GraphNode* it;
    
    /* Search for the node */
    for(it = *first; it != NULL; it = it->next){
        if (it->z == z){
            it->neighbours++;
            *ptr = NULL; 
            return false;
        }
    }
    
    /* Need to insert the node */
    GraphNode* new = graphNodeInsert(*first, z, DEAD);
    new->neighbours++;
    *ptr = new;
    *first = new;

    return true;
}

/**************************************************************************/
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

/**************************************************************************/
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

/* Node Lists related functions*/

/**************************************************************************/
Node* nodeInsert(Node* first, coordinate x, coordinate y, coordinate z, GraphNode* ptr){
    Node* new = (Node*) malloc(sizeof(Node));
    if (new == NULL){
        fprintf(stderr, "Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    new->x = x;
    new->y = y;
    new->z = z;
    new->ptr = ptr;
    new->next = first;
    return new;
}

/**************************************************************************/
bool nodeRemove(Node** first_ptr, coordinate x, coordinate y, coordinate z){
    Node** cur;
    for (cur = first_ptr; *cur; ){
        Node* entry = *cur;
        if (entry->x == x && entry->y == y && entry->z == z){
            *cur = entry->next;
            free(entry);
            return true;
        }else{
            cur = &entry->next;
        }
    }
    return false;
}

/**************************************************************************/
void nodeListFree(Node* first){
    Node* it, *next;
    
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }    
}