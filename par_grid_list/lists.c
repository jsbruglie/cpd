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

void graphNodeRemove(GraphNode** first_ptr, coordinate z, omp_lock_t* lock_ptr){
    GraphNode** cur;
    omp_set_lock(lock_ptr);
    for (cur = first_ptr; *cur; ){
        GraphNode* entry = *cur;
        if (entry->z == z){
            *cur = entry->next;
            free(entry);
        }else{
            cur = &entry->next;
        }
    }
    omp_unset_lock(lock_ptr);
}

void graphNodeDelete(GraphNode* first){
    GraphNode* it, *next;
    for(it = first; it != NULL; it = next){
        next = it->next;
        free(it);
    }
}

bool graphNodeAddNeighbour(GraphNode** first, coordinate z, GraphNode** ptr, omp_lock_t* lock_ptr){
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
    *ptr = new;
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

/* Node Lists related functions*/

List* listCreate(){
    List* list = (List*) malloc(sizeof(List));
    if (list == NULL){
        fprintf(stderr, "Malloc failed. Memory full");
        exit(EXIT_FAILURE);
    }
    list->size = 0;
    list->first = NULL;
    return list;
}

Node* listFirst(List* list){
    if (list != NULL){
        return list->first;
    }
    return NULL;
}


void listInsert(List* list, coordinate x, coordinate y, coordinate z, GraphNode* ptr){
    if(list != NULL){
        list->first = nodeInsert(list->first, x, y, z, ptr);
        list->size++;
    }
}

void listInsertLock(List* list, coordinate x, coordinate y, coordinate z, GraphNode* ptr, omp_lock_t* lock_ptr){
    if(list != NULL){
        omp_set_lock(lock_ptr);
        list->first = nodeInsert(list->first, x, y, z, ptr);
        list->size++;
        omp_unset_lock(lock_ptr);
    }
}

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

void listRemove(List* list, coordinate x, coordinate y, coordinate z){
    if(list != NULL){
        if(nodeRemove(&(list->first), x, y, z))
            list->size--;
    }   
}

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

void listDelete(List* list){
    Node* it, *next;
    if (list != NULL){
        for(it = list->first; it != NULL; it = next){
            next = it->next;
            free(it);
        }
        free(list);
    }    
}

void listCleanup(List* list){
    Node *temp, *prev;
    if (list != NULL){
        temp = list->first;
        
        /* Delete from the beginning */
        while (temp != NULL && temp->x == REMOVE){
            list->first = temp->next;   
            free(temp);                 
            list->size--;
            temp = list->first;
        }
     
        /* Delete from the middle */
        while (temp != NULL){
            while (temp != NULL && temp->x != REMOVE){
                prev = temp;
                temp = temp->next;
            }
        
            /* No entries scheduled for removal found */
            if (temp == NULL) return;
     
            prev->next = temp->next;
            free(temp);
            list->size--;

            temp = prev->next;
        }
    }
}