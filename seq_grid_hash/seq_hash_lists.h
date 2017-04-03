/** @file seq_hash_lists.h
 *  @brief Function prototypes for seq_hash_lists.c
 *
 *  Function prototypes for list structures
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#ifndef SEQUENTIAL_HASH_LIST_H
#define SEQUENTIAL_HASH_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define true 1     
#define false 0     

#define ALIVE 1     /**< Macro for representing a live cell */
#define DEAD 0      /**< Macro for representing a dead cell */

#define REMOVE -1   /**< Used to signal that an entry in list should be removed */
#define UPDATE 1    /**< Used to force a GraphNode insertion to simply update an existing node */

typedef unsigned char bool;
typedef int coordinate;

/** @brief Structure for storing a node of the graph */
typedef struct Graph_Node_Struct{
    coordinate z;                   /**< z coordinate, x and y are implicitly mapped */
    bool state;                     /**< State of a node cell (DEAD or ALIVE) */
    unsigned char neighbours;       /**< Neighbour counter */
    struct Graph_Node_Struct* next; /**< Pointer to the next entry in the list */
}GraphNode;

/** @brief Structure for storing cells */
typedef struct Node_Struct{
    coordinate x;
    coordinate y;
    coordinate z;
    struct Graph_Node_Struct* ptr;
    struct Node_Struct* next;
}Node;

/* GraphNode Lists related functions */

/** @brief Inserts a GraphNode in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
GraphNode* graphNodeInsert(GraphNode* first, coordinate z, bool state);

/** @brief Removes a GraphNode from the list with value z
 *
 *  @param first_ptr A pointer to the first node of the list
 *  @param z Value of the node to be removed
 *  @return Void.
 */
void graphNodeRemove(GraphNode** first_ptr, coordinate z);

/** @brief Deletes a list of GraphNodes
 *
 *  @param first The first node of the list
 *  @return Void.
 */
void graphNodeDelete(GraphNode* first);

/** @brief Inserts a cell if not yet present and increments its number of live nighbours
 *
 *  @param first_ptr A pointer to the first element of the GraphNode list
 *  @param z The z coordinate of the cell
 *  @param ptr A pointer to the inserted cell if inserted, else set to NULL
 *
 *  @return Whether the cell was inserted in the graph or not
 */
bool graphNodeAddNeighbour(GraphNode** first_ptr, coordinate z, GraphNode** ptr);

/** @brief Sorts a GraphNode list by ascending order of coordinate z
 *
 *  @param first_ptr A pointer to the pointer to the first GraphNode of the list
 *  @return Void.
 */
void graphNodeSort(GraphNode** first_ptr);

/** @brief Removes dead nodes from a GraphNode list
 *
 *  @param first_ptr A pointer to the first node of the list
 *  @return Void.
 */
void graphListCleanup(GraphNode** first_ptr);

/* Node Lists related functions*/

/** @brief Inserts a Node in a Node list
 *
 *  @param first A pointer to the first element of the list
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @param ptr The GraphNode* corresponding to the cell to be inserted
 *  @return The new head of the list.
 */
Node* nodeInsert(Node* first, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief Removes a Node* from a given Node list
 *
 *  @param first_ptr A pointer to the first node of the list
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @return Whether the removal was successful or not.
 */
bool nodeRemove(Node** first_ptr, coordinate x, coordinate y, coordinate z);

/** @brief Frees a Node* list
 *
 *  @param first The first node of the list
 *  @return Void.
 */
void nodeListFree(Node* first);

#endif