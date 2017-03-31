#include "hash.h"

//http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
int hash(int size, coordinate x, coordinate y, coordinate z)
{
	uint32_t hashval = ((uint32_t)x * 73856093) ^ ((uint32_t)y * 19349663) ^ ((uint32_t)z * 83492791) ;
	return (int)(hashval % size);
}

Hashtable* createHashtable(int size){
	Hashtable* new = (Hashtable*) malloc(sizeof(Hashtable));
	new->table = (List**) malloc(sizeof(List*) * size);
	new->size = size;
	int i;
	for(i=0; i<size; i++){
		new->table[i] = listCreate();
		
	}
	new->occupied = 0;
	
	new->locks = (omp_lock_t*)malloc(sizeof(omp_lock_t) * size);
	for(i=0;i<size;i++)
		 omp_init_lock(&new->locks[i]);

	return new;
}

void hashtableWrite(Hashtable* hashtable, coordinate x, coordinate y, coordinate z, GraphNode* ptr){
	int hashval = hash(hashtable->size, x, y, z);
	//Check if it already exists in this current bucket
	Node* n;
	for(n = listFirst(hashtable->table[hashval]); n != NULL; n = n->next){
		if(n->x == x && n->y == y && n->z == z)
			return;
	}
	listInsert(hashtable->table[hashval], x, y, z, ptr);
	hashtable->occupied++;
	return;
}

void hashtableRemove(Hashtable* hashtable, coordinate x, coordinate y, coordinate z){
	int hashval = hash(hashtable->size, x, y, z);
	if(hashtable->table[hashval] != NULL){
		listRemove(hashtable->table[hashval], x, y, z);
		hashtable->occupied--;
	}
	return;
}

void printHashtable(Hashtable* hashtable){
	int i;
	Node* n;
	
	printf("Current Hashtable:\n");
	for (i = 0; i < hashtable->size; i++){
		printf("%d - ", i);
		for(n = listFirst(hashtable->table[i]); n != NULL; n = n->next){
			printf("(%d,%d,%d)->", n->x, n->y, n->z);
		}
		
		printf("\n");
	}
	return;
}

void hashtableFree(Hashtable* hashtable){
	int i;
	for(i=0; i<hashtable->size; i++){
		listDelete(hashtable->table[i]);
		omp_destroy_lock(&hashtable->locks[i]);
	}
	free(hashtable->table);
	free(hashtable->locks);
	free(hashtable);
	
}
/*GraphNode* hashtableRead(Hashtable* hashtable, coordinate x, coordinate y, coordinate z){
	Node* it;
	int hashval = hash(hashtable->size,x,y,z);
	for(it = listFirst(hashtable->table[hashval]);it != NULL; it = it->next){
		if(it->x == x && it->y == y && it->z == z){
			return it->ptr;
		}
	}
	return NULL; //doesnt exist
}*/
