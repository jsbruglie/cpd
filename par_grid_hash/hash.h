#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <inttypes.h>

#include "lists.h"
typedef int coordinate;

typedef struct _Hashtable{
	List** table;
	int size;
	omp_lock_t* locks;
	int occupied;
}Hashtable;

Hashtable* createHashtable(int size);

int hash(int size, coordinate x, coordinate y, coordinate z);

void hashtableWrite(Hashtable* hashtable, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

void hashtableRemove(Hashtable* hashtable, coordinate x, coordinate y, coordinate z);

//DEBUG
void printHashtable(Hashtable* hashtable);

void hashtableFree(Hashtable* hashtable);

#endif