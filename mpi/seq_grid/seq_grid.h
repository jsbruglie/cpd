#ifndef SEQUENTIAL_GRID_H
#define SEQUENTIAL_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "lists.h"
#include "debug.h"

#define ALIVE 1          /**< Macro for representing a live cell */
#define DEAD 0           /**< Macro for representing a dead cell */

#define REMOVAL_PERIOD 5 /**< Number of generations until you cleanup the dead nodes from graph */
#define BUFFER_SIZE 100  /**< Maximum length for a single infile line */

typedef unsigned char bool;

/** @brief Notifies the neighbours of (x,y,z) of its aliveness and adds them to list
 *
 *  @param graph The graph representation
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @param list The list for keeping track of live cells and respective neighbours
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 */
void visitNeighbours(GraphNode*** graph, int cube_size, coordinate x, coordinate y, coordinate z);

/** @brief Initializes the graph representation structure
 *
 *  @param size The size of the side of the cube that represents the 3D space
 */
GraphNode*** initGraph(int size);

/** @brief Frees the graph representation from memory
 *
 *  @param size The size of the side of the cube that represents the 3D space
 */
void freeGraph(GraphNode*** graph, int size);

/** @brief Prints the graph, and sorts each of the lists
 *
 *  @attention Should not be called while processing generations,
 *  as sorting breaks pointer logic with list
 *
 *  @param graph The graph representation
 *  @param size The size of the side of the cube that represents the 3D space
 */
void printAndSortActive(GraphNode*** graph, int cube_size);

/** @brief Parse command line arguments
 *
 *  @param argc Number of arguments
 *  @param argv Argument string
 *  @param file Output
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

/** @brief Parse input file contents
 *
 *  @param file Filename string
 *  @param list List for keeping track of live cells and neighbours
 *  @param cube_size The size of the side of the cube that represents the 3D space
 */
GraphNode*** parseFile(char* file, int* cube_size);

#endif
