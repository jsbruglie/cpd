//hash = ((((prime1 + value1) * prime2) + value2) * prime3) + value3) * prime4
//Primes: 31,37,127,149,163

#include "hash.h"

int hash(int size, coordinate x, coordinate y, coordinate z){
	uint32_t hashval = 
	return hashval % size;
}

Hashtable* createHashtable(int size){

	Hashtable* new = (Hashtable*)malloc(sizeof(Hashtable));
	new->table = (List**)malloc(sizeof(List*) * size);

	int i;
	for(i=0;i<size;i++){
		new->table[i]->first = NULL;
		new->table[i]->size = 0;
	}
	
	new->locks = (omp_lock_t*)malloc(sizeof(omp_lock_t) * size);
	for(i=0;i<size;i++)
		 omp_init_lock(&new->locks[i]);
	new->size = size;
	return new;
}

void hashtableWrite(Hashtable* hashtable, coordinate x, coordinate y, coordinate z, GraphNode* ptr){
	int hashval = hash(size, x, y, z);

	listInsertLock(hashtable->table[hashval], x, y, z, ptr, &(hashtable->locks[hashval]);
	return;

}

void hashtableRead(){
	
}