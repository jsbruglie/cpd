#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++ MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BUFFER_SIZE 200
#define REMOVAL_PERIOD 5 /**< Number of generations until you cleanup the dead nodes from graph */

#define ALIVE 1 /**< Macro for representing a live cell */
#define DEAD 0 /**< Macro for representing a dead cell */
#define true 1
#define false 0
#define REMOVE -1   /**< Used to signal that an entry in list should be removed */
#define UPDATE 1    /**< Used to force a GraphNode insertion to simply update an existing node */

/*+++++++++++++++++++++++++++++++++++++++++++++++++ BLOCK MACROS +++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define BLOCK_LOW(rank,numprocs,size) 	((rank)*(size)/(numprocs))
#define BLOCK_HIGH(rank,numprocs,size) 	(BLOCK_LOW((rank)+1,numprocs,size)-1)
#define BLOCK_SIZE(rank,numprocs,size) 	(BLOCK_HIGH(rank,numprocs,size)-BLOCK_LOW(rank,numprocs,size)+1)
#define BLOCK_OWNER(element,numprocs,size) ((numprocs*(element+1)-1)/size)

/*+++++++++++++++++++++++++++++++++++++++++++++++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++*/
typedef unsigned char bool;

// TODO - Create a special MPI structure for frontier sending that only has y and z
/** @brief Structure for sending over MPI */
typedef struct _node{
    int z;
    int y;
    int x;
}node;

/** @brief Structure for storing a node of the graph */
typedef struct _GraphNode{
    int z;                   /**< z coordinate, x and y are implicitly mapped */
    int state;                     /**< State of a node cell (DEAD or ALIVE) */
    unsigned char neighbours;       /**< Neighbour counter */
    struct _GraphNode* next; /**< Pointer to the next entry in the list */
}GraphNode;

/** @brief Inserts a GraphNode in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
GraphNode* graphNodeInsert(GraphNode* first, int z, int state);

/** @brief Initializes a clean graph when given only size
 *
 *  @param size The size of the grid of lists the graph will be
 *  @return The created graph.
 */
GraphNode*** initGraph(int dim_x, int dim_y);

/** @brief Initializes a clean graph when given only size
 *
 *  @param size The size of the grid of lists the graph will be
 *  @return The created graph.
 */
GraphNode*** initLocalGraph(int bsize, int size);

/** @brief Inserts a cell if not yet present and increments its number of live nighbours
 *
 *  @return Whether the cell was inserted in the graph or not
 */
bool graphNodeAddNeighbour(GraphNode** first, int z);

/** @brief Notifies the neighbours of (x,y,z) of its aliveness and adds them to list
 *
 *  @param graph The graph representation
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @param list The list for keeping track of live cells and respective neighbours
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 */
void visitNeighbours(GraphNode*** graph, int cube_size, int x, int y, int z);

/** @brief Prints the graph, and sorts each of the lists
 *
 *  @attention Should not be called while processing generations,
 *  as sorting breaks pointer logic with list
 *
 *  @param graph The graph representation
 *  @param size The size of the side of the cube that represents the 3D space
 */
void printAndSortActive(GraphNode*** graph, int size);

/** @brief Sorts a GraphNode list by ascending order of coordinate z
 *
 *  @param first_ptr A pointer to the pointer to the first GraphNode of the list
 *  @return Void.
 */
void graphNodeSort(GraphNode** first_ptr);

/** @brief Frees the graph representation from memory
 *
 *  @param size The size of the side of the cube that represents the 3D space
 */
void freeGraph(GraphNode*** graph, int dim_x, int dim_y);

/** @brief Deletes a list of GraphNodes
 *
 *  @param first The first node of the list
 *  @return Void.
 */
void graphNodeDelete(GraphNode* first);

/** @brief Cleans up a graph list
 *
 *  @param head A pointer to the pointer of the graph list we want to clean up
 *  @return Void.
 */
void graphListCleanup(GraphNode** head);


#endif
