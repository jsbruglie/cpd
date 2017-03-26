#ifndef PAR_GRID_H
#define PAR_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

/** @brief 
 *  
 *  @param 
 */
void visitNeighbours(Node**** graph0, Node**** graph1, int cube_size, omp_lock_t** graph0_lock, omp_lock_t** graph1_lock);

/** @brief 
 *  
 *  @param 
 */
void upgradeGraph(Node**** graph0, Node**** graph1, int cube_size, omp_lock_t** graph1_lock);

/** @brief 
 *  
 *  @param 
 */
void parse_args(int argc, char* argv[], char** file, int* generations);


#endif