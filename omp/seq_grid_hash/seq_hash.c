#include "seq_hash.h"

Hashtable* createHashtable(int size){
    Hashtable* new = (Hashtable*) malloc(sizeof(Hashtable));
    new->table = (Node**) malloc(sizeof(Node*) * size);
    int i;
    for(i = 0; i < size; i++){
        new->table[i] = NULL;
    }
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
    if (hashtable->table[hashval] == NULL){
        hashtable->occupied++;
    }
    hashtable->table[hashval] = nodeInsert(hashtable->table[hashval], x, y, z, ptr);
    hashtable->elements++;
    return;
}

/**************************************************************************/
void hashtableRemove(Hashtable* hashtable, coordinate x, coordinate y, coordinate z){
    int hashval = hash(hashtable->size, x, y, z);
    if(hashtable->table[hashval] != NULL){
        if(nodeRemove(&(hashtable->table[hashval]), x, y, z)){
            hashtable->elements--;
            if (hashtable->table[hashval] == NULL){
                hashtable->occupied--;
            }
        }     
    }
    return;
}

/**************************************************************************/
void hashtableFree(Hashtable* hashtable){
    int i;
    for(i = 0; i < hashtable->size; i++){
        nodeListFree(hashtable->table[i]);
    }
    free(hashtable->table);
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