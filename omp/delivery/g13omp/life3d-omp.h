/** @file life3d-omp.h
 *  @brief Function prototypes for life3d-omp.c
 *
 *  Parallel implementation with 2D Matrix of lists,
 *  with a brute force iteration
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#ifndef LIFE_3D_OMP_H
#define LIFE_3D_OMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "lists-omp.h"
#include "debug.h"

#define ALIVE 1             /**< Macro for representing a live cell */
#define DEAD 0              /**< Macro for representing a dead cell */

#define REMOVAL_PERIOD 5    /**< Number of generations between graph cleanup calls (removal of dead nodes) */
#define BUFFER_SIZE 100     /**< Maximum length for a single infile line */

typedef unsigned char bool;

/** @brief Notifies the neighbours of (x,y,z) of its aliveness and adds them to list
 *
 *  @param graph The graph representation
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @param list The list for keeping track of live cells and respective neighbours
 *  @param x X coordinate
 *  @param y Y coordinate
 *  @param z Z coordinate
 *  @return Void.
 */
void visitNeighbours(GraphNode*** graph, omp_lock_t** graph_lock, int cube_size, coordinate x, coordinate y, coordinate z);

/** @brief Initializes the graph representation structure
 *  
 *  @param size The size of the side of the cube that represents the 3D space
 *  @return The initialized, yet empty, graph representation.
 */
GraphNode*** initGraph(int cube_size);

/** @brief Frees the graph representation from memory
 *  
 *  @param cube_size The size of the side of the cube that represents the 3D space
 */
void freeGraph(GraphNode*** graph, int cube_size);

/** @brief Prints the graph, and sorts each of the lists
 *
 *  @param graph The graph representation    
 *  @param size The size of the side of the cube that represents the 3D space
 *  @return Void.
 */
void printAndSortActive(GraphNode*** graph, int cube_size);

/** @brief Parse command line arguments
 *
 *  @attention `input_name` will be dynamically allocated inside and must be freed 
 *
 *  @param argc Number of arguments
 *  @param argv Argument strings
 *  @param input_name The name of the input file
 *  @param A pointer to the number of generations to be processed
 *  @return Void.    
 */
void parseArgs(int argc, char* argv[], char** file, int* generations);

/** @brief Parse input file contents 
 *
 *  @param file Filename string
 *  @param list List for keeping track of live cells and neighbours
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @return The filled `GraphNode` graph representation.
 */
GraphNode*** parseFile(char* file, int* cube_size);    

#endif
