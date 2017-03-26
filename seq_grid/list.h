#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define ALIVE 1 /**<  */
#define DEAD 0 /**<  */
#define TRUE 1 /**<  */
#define FALSE 0 /**<  */

typedef char bool; /**< */

/** @brief Structure for storing a list node of the graph */
typedef struct _Node{
	short z; /**<  */
	bool status; /**<  */
	int counter; /**<  */            
    struct _Node* next; /**< Pointer to the next entry in the list */
}Node;

/** @brief 
 *  
 *  @param 
 */
void insertCell(Node** head, int z, int status);

/** @brief 
 *  
 *  @param 
 */
Node* createNode(int z, int status);

/** @brief 
 *  
 *  @param 
 */
Node**** initGraph(int size);

#endif