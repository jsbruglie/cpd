#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "lists.h"

typedef struct _Hashtable{
	List**;
	int size;
	omp_lock_t* locks;
}Hashtable;

Hashtable* createHashtable(int size);

int hash(int size, int x, int y, int z);

void hashtableWrite();
int hashtableRead();

int hashtableDelete();

void freehashtable();

//DEBUG
void printHashtable();

#endif