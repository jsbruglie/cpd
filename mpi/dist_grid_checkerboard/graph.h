/**
 * @file graph.h
 * @brief Sparse graph representation for Game Of Life 3D MPI Implementation
 * 
 * @author João Borrego
 * @author Pedro Abreu
 * @author Miguel Cardoso
 * @bug No known bugs.  
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Macros */

#define ALIVE 1     /**< Macro for representing a live cell */
#define DEAD 0      /**< Macro for representing a dead cell */
#define true 1      /**< True Logic value */
#define false 0     /**< False Logic value */
#define REMOVE -1   /**< Used to signal that an entry in list should be removed */
#define UPDATE 1    /**< Used to force a GraphNode insertion to simply update an existing node */

/* Datatypes */

/** 1-Byte Boolean */
typedef unsigned char bool;
/** */
#define COORDINATE_MPI MPI_UNSIGNED_SHORT 

/* Structures */

/** @brief Structure for sending over MPI */
typedef struct _Node{
    uint16_t x;  /**< x coordinate */
    uint16_t y;  /**< y coordinate */
    uint16_t z;  /**< z coordinate */
}Node;

/** @brief Reduced node structure for sending borders over MPI */
typedef struct _RNode{
    uint16_t a;  /**< x or y relative coordinate */
    uint16_t z;  /**< z coordinate */
}RNode;

/** @brief Structure for storing a node of the graph */
typedef struct _GraphNode{
    uint16_t z;                 /**< z coordinate, x and y are implicitly mapped */
    uint8_t state;              /**< State of a node cell (DEAD or ALIVE) */
    uint8_t neighbours;         /**< Neighbour counter */
    struct _GraphNode *next;    /**< Pointer to the next entry in the list */
}GraphNode;

/**
 * @brief Inserts a GraphNode in the list with value z
 * 
 * @details Insertion is done in the begininng of the list
 * 
 * @param first The first node of the list
 * @param z z coordinate of the node to be inserted
 * @param state The node initial state
 * @return The new head of the list
 */
GraphNode *graphNodeInsert(GraphNode *first, int z, int state);

/** 
 * @brief Initialises a clean graph with given dimensions
 *  
 * @param dim_x The graph size in dimension x
 * @param dim_y The graph size in dimension y
 * @return The initialised graph
 */
GraphNode ***initGraph(int dim_x, int dim_y);

/**
 * @brief Inserts a cell if not yet present and increments its number of live nighbours
 * 
 * @return Whether the cell was inserted in the graph or not
 */
bool graphNodeAddNeighbour(GraphNode **first, int z);

/**
 * @brief Visits neighbours that are not in the boundaries
 * 
 * @attention x+1, x-1, y+1, y-1 are guaranteed to be valid indices,
 * since (x,y,z) is considered to be internal, i.e., not on a border
 * 
 * @param local_graph The local graph representation, subspace of the global cubic space
 * @param cube_size The side of the cube that represents 3D space
 * @param x x local coordinate
 * @param y y local coordinate
 * @param z z global coordinate
 */
void visitInternalNeighbours(GraphNode ***local_graph, int cube_size, int x, int y, int z);

/**
 * @brief Visits neighbours on the local graph boundaries
 * 
 * @details  Notifies the neighbours of (x,y,z) of its aliveness
 * 
 * @param local_graph The local graph representation, subspace of the global cubic space
 * @param cube_size The side of the cube that represents 3D space
 * @param dim_x The graph size in dimension x
 * @param dim_y The graph size in dimension y
 * @param x x local coordinate
 * @param y y local coordinate
 * @param z z global coordinate
 */
void visitBoundaryNeighbours(GraphNode ***local_graph, int cube_size, int dim_x, int dim_y, int x, int y, int z);

/**
 * @brief Prints the graph, and sorts each of the lists
 *
 * @attention Should not be called while processing generations,
 * as sorting breaks pointer logic with list
 *
 * @param graph The graph representation
 * @param size The size of the side of the cube that represents the 3D space
 */
void printAndSortActive(GraphNode ***graph, int size);

/** 
 * @brief Sorts a GraphNode list by ascending order of coordinate z
 *
 * @param first_ptr A pointer to the pointer to the first GraphNode of the list
 */
void graphNodeSort(GraphNode **first_ptr);

/**
 * @brief Frees the graph representation from memory
 *
 * @param size The size of the side of the cube that represents the 3D space
 */
void freeGraph(GraphNode*** graph, int dim_x, int dim_y);

/** 
 * @brief Deletes a list of GraphNodes
 *
 * @param first The first node of the list
 */
void graphNodeDelete(GraphNode* first);

/** @brief Cleans up a graph list
 *
 *  @param head A pointer to the pointer of the graph list to be cleaned up
 */
void graphListCleanup(GraphNode** head);

#endif