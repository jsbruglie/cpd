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

/** @brief Structure for storing cells */
typedef struct Node_Struct{
    coordinate x;
    coordinate y;
    coordinate z;
    struct Graph_Node_Struct* ptr;
    struct Node_Struct* next;
}Node;

/** @brief List container structure */
typedef struct List_Struct{
    int size;                   /**< Number of elements in list */
    Node* first;                /**< First node of the list */
}List;

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
bool graphNodeAddNeighbour(GraphNode** first, coordinate z, GraphNode** ptr);

/** @brief Sorts a GraphNode list by ascending order of coordinate z
 *
 *  @param first_ptr A pointer to the pointer to the first GraphNode of the list
 *  @return Void.
 */
void graphNodeSort(GraphNode** first_ptr);

/* Node Lists related functions*/

/** @brief Creates a list structure
 *
 *  @return The resulting empty list structure, with size 0.
 */
List* listCreate();

/** @brief Returns the Node list of a given List structure
 *
 *  @param list The list structure pointer
 *  @return A pointer to the Node* list of list.
 */
Node* listFirst(List* list);

/** @brief Inserts a Node* in a given list and updates size
 *
 *  @param list The list we want to insert in
 *  @param x X coordinate of the cell to be inserted
 *  @param y Y coordinate of the cell to be inserted
 *  @param z Z coordinate of the cell to be inserted
 *  @param ptr The GraphNode* corresponding to the cell to be inserted
 *  @return Void.
 */
void listInsert(List* list, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief Inserts a Node in a Node list
 *
 *  @param first A pointer to the first element of the list
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @param ptr The GraphNode* corresponding to the cell to be inserted
 *  @return .
 */
Node* nodeInsert(Node* first, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief Removes a Node* from a given list structure and updates size
 *
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @return Void.
 */
void listRemove(List* list, coordinate x, coordinate y, coordinate z);

/** @brief Removes a Node* from a given Node list
 *
 *  @param first_ptr A pointer to the first node of the list
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @return Whether the removal was successful or not.
 */
bool nodeRemove(Node** first_ptr, coordinate x, coordinate y, coordinate z);

/** @brief Deletes a List strucutre
 *
 *  @param list The list to be deleted
 *  @return Void.
 */
void listDelete(List* list);

/** @brief Removes scheduled entries from a List structure
 *
 *  Removes entries in Node* list with x = REMOVE
 *
 *  @param list The list to be cleaned
 *  @return Void.
 */
void listCleanup(List* list);

#endif