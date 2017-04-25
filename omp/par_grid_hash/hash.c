#include "hash.h"

Hashtable* createHashtable(int size){
    Hashtable* new = (Hashtable*) malloc(sizeof(Hashtable));
    new->table = (Node**) malloc(sizeof(Node*) * size);
    new->table_locks = (omp_lock_t*) malloc(sizeof(omp_lock_t) * size);
    int i;
    for(i = 0; i < size; i++){
        new->table[i] = NULL;
        omp_init_lock(&(new->table_locks[i]));
    }
    omp_init_lock(&(new->elements_lock));
    omp_init_lock(&(new->occupied_lock));
    new->size = size;
    new->elements = 0;
    new->occupied = 0;

    return new;
}

/**************************************************************************/
int hash(int size, coordinate x, coordinate y, coordinate z){
    uint32_t hashval = ((uint32_t)x * 73856093) ^ ((uint32_t)y * 19349663) ^ ((uint32_t)z * 83492791) ;
    return (int)(hashval % size);
}

/**************************************************************************/
void hashtableWrite(Hashtable* hashtable, coordinate x, coordinate y, coordinate z, GraphNode* ptr){
    int hashval = hash(hashtable->size, x, y, z);
    
    omp_set_lock(&(hashtable->table_locks[hashval]));
    
        if (hashtable->table[hashval] == NULL){
            omp_set_lock(&(hashtable->occupied_lock));
                hashtable->occupied++;
            omp_unset_lock(&(hashtable->occupied_lock));
        }
        hashtable->table[hashval] = nodeInsert(hashtable->table[hashval], x, y, z, ptr);
        omp_set_lock(&(hashtable->elements_lock));
            hashtable->elements++;
        omp_unset_lock(&(hashtable->elements_lock));
        
    omp_unset_lock(&(hashtable->table_locks[hashval]));
    return;
}

/**************************************************************************/
void hashtableRemove(Hashtable* hashtable, coordinate x, coordinate y, coordinate z){
    int hashval = hash(hashtable->size, x, y, z);
    
    omp_set_lock(&(hashtable->table_locks[hashval]));
    
        if(hashtable->table[hashval] != NULL){
            if(nodeRemove(&(hashtable->table[hashval]), x, y, z)){
                omp_set_lock(&(hashtable->elements_lock));
                    hashtable->elements--;
                omp_unset_lock(&(hashtable->elements_lock));    
                if (hashtable->table[hashval] == NULL){
                    omp_set_lock(&(hashtable->occupied_lock));
                        hashtable->occupied--;
                    omp_unset_lock(&(hashtable->occupied_lock));
                }
            }     
        }

    omp_unset_lock(&(hashtable->table_locks[hashval])); 
    
    return;
}

/**************************************************************************/
void hashtableFree(Hashtable* hashtable){
    int i;
    for(i = 0; i < hashtable->size; i++){
        nodeListFree(hashtable->table[i]);
        omp_destroy_lock(&(hashtable->table_locks[i]));
    }
    omp_destroy_lock(&(hashtable->occupied_lock));
    omp_destroy_lock(&(hashtable->elements_lock));
    free(hashtable->table);
    free(hashtable->table_locks);
    free(hashtable); 
}

/**************************************************************************/
void printHashtable(Hashtable* hashtable){
    int i;
    Node* it;
    
    printf("Current Hashtable:\n");
    for (i = 0; i < hashtable->size; i++){
        printf("%d - ", i);
        for(it = hashtable->table[i]; it != NULL; it = it->next){
            printf("(%d,%d,%d)->", it->x, it->y, it->z);
        }
        printf("\n");
    }
    return;
}