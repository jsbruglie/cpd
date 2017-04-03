/** @file   par_grid_list.h
 *  @brief  Parallel 2D Matrix with lists. 
 *
 *  Parallel OpenMP implementation of seq_grid_list  
 *  2D Matrix of lists is used as the graph representation.
 *  A separate list is used for keeping track of live cells and neighbours.
 *  This list provides fast direct access to the nodes, by storing pointers to them.
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#ifndef PARALLEL_LIST_H
#define PARALLEL_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "lists.h"
#include "debug.h"

#define ALIVE 1
#define DEAD 0
#define BUFFER_SIZE 100

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
void visitNeighbours(GraphNode*** graph, omp_lock_t** graph_lock, int cube_size,
                        List* list, omp_lock_t* list_lock,
                        coordinate x, coordinate y, coordinate z);

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
 *  SHOULD NOT BE CALLED DURING THE CALCULATION OF A GENERATION:
 *  SORTING WILL BREAK POINTER LOGIC
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
GraphNode*** parseFile(char* file, List* list, int* cube_size);    

#endif