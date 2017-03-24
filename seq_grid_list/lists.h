#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

#define ALIVE 1
#define DEAD 0

#define REMOVE -1	/**< Used to signal that an entry in list should be removed */
#define UPDATE 1	/**< Used to force a GraphNode insertion to simply update an existing node */

typedef unsigned char bool;
typedef int coordinate;

/** @brief Structure for storing a node of the graph
 *
 */
typedef struct Graph_Node_Struct{
    coordinate z;                   /**< z coordinate, x and y are implicitly mapped */
    bool state;						/**< State of a node cell (DEAD or ALIVE) */
    unsigned char neighbours;       /**< Neighbour counter */
    struct Graph_Node_Struct* next; /**< Pointer to the next entry in the list */
}GraphNode;

/** @brief Structure for storing cells 
 *  
 */
typedef struct Node_Struct{
    coordinate x;
    coordinate y;
    coordinate z;
    struct Graph_Node_Struct* ptr;
    struct Node_Struct* next;
}Node;

/** @brief List container structure
 */
typedef struct List_Struct{
    int size;                   /**< Number of elements in list */
    Node* first;                /**< First node of the list */
}List;

/* NodeGraph Lists -related functions */

/** @brief Inserts a node in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
GraphNode* graphNodeInsert(GraphNode* first, coordinate z, bool state);

/** @brief Removes a node from the list with value z
 *
 *  @param first_ptr A pointer to the first node of the list
 *  @param z Value of the node to be removed
 *  @return The head of the updated list.
 */
void graphNodeRemove(GraphNode** first_ptr, coordinate z);

/** @brief Deletes a list of graph nodes
 *
 *  @param first The first node of the list
 *  @return Void.
 */
void graphNodeDelete(GraphNode* first);

/**
 *  @return Whether the incremented node now has exactly 2 live neighbours.
 *  This is needed to keep track of this cell, as it's state might change 
 */
bool graphNodeAddNeighbour(GraphNode** first, coordinate z, GraphNode** ptr);

/*
 *
 */
void graphNodeSort(GraphNode** first_ptr);

/* Node Lists-related functions*/

/** @brief Creates a list structure
 *
 *  @param 
 *  @return .
 */
List* listCreate();

/** @brief 
 *
 *  @param 
 *  @return .
 */
Node* listFirst(List* list);

/** @brief 
 *
 *  @param 
 *  @return .
 */
void listInsert(List* list, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief 
 *
 *  @param 
 *  @return .
 */
Node* nodeInsert(Node* first, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief 
 *
 *  @param 
 *  @return .
 */
void listRemove(List* list, coordinate x, coordinate y, coordinate z);

/** @brief 
 *
 *  @param 
 *  @return .
 */
void nodeRemove(Node** first_ptr, coordinate x, coordinate y, coordinate z);

/** @brief 
 *
 *  @param 
 *  @return .
 */
void listDelete(List* list);

/** @brief 
 *
 *  @param 
 *  @return .
 */
void listCleanup(List* list);

/** @brief 
 *
 *  @param 
 *  @return .
 */
Node* nodeExchange(Node** obj, Node* newval);

#endif