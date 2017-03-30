#include "hash.h"

//http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
int hash(int size, coordinate x, coordinate y, coordinate z){
	uint32_t hashval = (((uint32_t)x * 73856093) ^ ((uint32_t)y * 19349663) ^ ((uint32_t)z * 83492791));
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
	int hashval = hash(hashtable->size, x, y, z);

	listInsertLock(hashtable->table[hashval], x, y, z, ptr, &(hashtable->locks[hashval]));
	return;
	
}

GraphNode* hashtableRead(Hashtable* hashtable, coordinate x, coordinate y, coordinate z){
	Node* it;
	int hashval = hash(hashtable->size,x,y,z);
	for(it = listFirst(hashtable->table[hashval]);it != NULL; it = it->next){
		if(it->x == x && it->y == y && it->z == z){
			return it->ptr;
		}
	}
	return NULL; //doesnt exist
}

void printHashtable(Hashtable hashtable){
	int i = 0;
	LinkedList* list;
	kv_pair* temp_kv;
	
	printf("Current Hashtable:\n");
	for (i = 0; i < _hashtable->size; i++){
		printf("%d - ", i);
		pthread_mutex_lock(&(_hashtable->lock[i]));
		for(list = _hashtable->table[i]; list != NULL; list = getNextNodeLinkedList(list)){
			temp_kv = (kv_pair*) getItemLinkedList(list);
			printf("%d->%s ", temp_kv->key, (char*) temp_kv->value);
		}
		pthread_mutex_unlock(&(_hashtable->lock[i]));
		printf("\n");
	}
}
