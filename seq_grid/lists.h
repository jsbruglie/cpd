#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

#define ALIVE 1
#define DEAD 0

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

/* NodeGraph Lists related functions */

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
 *  @return The head of the updated list.
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
 *  @return Whether the cell was inserted in the graph or not
 */
bool graphNodeAddNeighbour(GraphNode** first, coordinate z);

/** @brief Sorts a GraphNode list by ascending order of coordinate z
 *
 *  @param first_ptr A pointer to the pointer to the first GraphNode of the list
 *  @return Void.
 */
void graphNodeSort(GraphNode** first_ptr);

#endif