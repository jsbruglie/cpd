/** @file par_grid_hash.h
 *  @brief Function prototypes for par_grid_hash.c
 *
 *  Parallel version with 2D Matrix with lists graph representation
 *  with a hashtable for quick access to live nodes and neighbours
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#ifndef PARALLEL_GRID_HASH_H
#define PARALLEL_GRID_HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "hash.h"
#include "debug.h"

#define ALIVE 1             /**< Macro for representing a live cell */
#define DEAD 0              /**< Macro for representing a dead cell */
#define HASH_RATIO 0.05     /**< HashTable_Size / #Initially_Live_Cells */

#define BUFFER_SIZE 100     /**< Maximum length for a single infile line */
#define GEN_BUFFER_SIZE 20  /**< Maximum length for generation string */
#define OUT_EXT "out"       /**< Output file extensions */

#define X 0 /**< Macro for first coordinate (x) in an array of coordinates */                
#define Y 1 /**< Macro for second coordinate (y) in an array of coordinates */
#define Z 2 /**< Macro for third coordinate (z) in an array of coordinates */

typedef unsigned char bool;

/** @brief Initializes the graph representation structure
 *  
 *  @param size The size of the side of the cube that represents the 3D space
 *  @return The initialized, yet empty, graph representation.
 */
GraphNode*** initGraph(int size);

/** @brief Frees the graph representation from memory
 *  
 *  @param size The size of the side of the cube that represents the 3D space
 *  @return Void.
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

/** @brief Prints the graph to an output file
 *
 *  @attention Must not be called between the calculation of generations,
 *  as it breaks the hashtable references to graph nodes!
 *
 *  @param graph The graph representation    
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @param input_name The name of the input file
 *  @param generations Number of processed generations
 *  @return Void.
 */
void printSortedGraphToFile(GraphNode*** graph, int cube_size, char* input_name, int generations);

/** @brief Generates the output filename string
 *
 *  Output filename follows the template `[INPUT_FILE].[GENERATIONS].out` by default
 *
 *  @attention Output string is dynamically allocated and should be freed after use!
 *
 *  @param input_name The name of the input file
 *  @param generations Number of processed generations
 *  @return The output filename string.
 */
char* generateOuputFilename(char* input_name, int generations);

/** @brief Returns a pointer to the last '.' (dot) in a string
 *
 *  @param str The string containing one or more '.' (dots) 
 *  @return A pointer to the position with the last dot in a string.
 */ 
char* findLastDot(char* str);

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
void parseArgs(int argc, char* argv[], char** input_name, int* generations);

/** @brief Parse input file contents to graph and hashtable 
 *
 *  @param file Filename string
 *  @param list List for keeping track of live cells and neighbours
 *  @param cube_size The size of the side of the cube that represents the 3D space
 *  @return The filled `GraphNode` graph representation.
 */
GraphNode*** parseFile(char* file, Hashtable* hashtable, int* cube_size); 

/** @brief Returns the number of live cells at the start 
 *
 *  Counts the number of lines in the input file and sibtracts the first, which specifies the cube size
 *
 *  @param file The input file name
 *  @return The number of live cells at the start.
 */
int getAlive(char* file);

#endif
