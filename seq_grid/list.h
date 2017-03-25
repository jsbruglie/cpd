#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define ALIVE 1
#define DEAD 0
#define TRUE 1
#define FALSE 0

typedef char bool;

/**
 * @brief 
 */
typedef struct _Node{
	short z;
	bool status;
	int counter;           
    struct _Node* next; /**< Pointer to the next entry in the list */
}Node;


void insertCell(Node** head, int z, int status);

Node* createNode(int z, int status);

Node**** initGraph(int size);

#endif