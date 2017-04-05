/** @file hash.h
 *  @brief Function prototypes for hash.c
 *
 *  Function prototypes for Hashtable structure
 *
 *  @author Pedro Abreu
 *  @author João Borrego
 *  @author Miguel Cardoso
 */

#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <inttypes.h>

#include "hash_lists.h"

typedef int coordinate;

/** @brief Structure for storing pointers to nodes for quick access */
typedef struct _Hashtable{
    Node** table;               /**< Array of Node* lists */
    int size;                   /**< Size of the hashtable */
    int elements;               /**< Total number of elements in the hashtable */
    int occupied;               /**< Number of occupied buckets of the hashtable */
    omp_lock_t* table_locks;    /**< Array of locks, one per Node* list */
    omp_lock_t elements_lock;   /**< Lock for elements field */
    omp_lock_t occupied_lock;   /**< Lock for occupied field */
}Hashtable;

/** @brief Creates the hashtable structure given a size
 *
 *  @param size The size of the hashtable
 *  @return A pointer to the created hashtable.
 */
Hashtable* createHashtable(int size);

/** @brief Hash function for hashtable
 *
 *  As seen in <a href="http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf">
 *  Optimized Spatial Hashing for Collision Detection of Deformable Objects</a>
 *
 *  @return The hash value. 
 */
int hash(int size, coordinate x, coordinate y, coordinate z);

/** @brief Inserts a GraphNode in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
void hashtableWrite(Hashtable* hashtable, coordinate x, coordinate y, coordinate z, GraphNode* ptr);

/** @brief Inserts a GraphNode in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
void hashtableRemove(Hashtable* hashtable, coordinate x, coordinate y, coordinate z);

/** @brief Inserts a GraphNode in the list with value z
 *
 *  @param first The first node of the list
 *  @param z Value of the node to be inserted
 *  @return The head of the updated list.
 */
void hashtableFree(Hashtable* hashtable);

// DEBUG
void printHashtable(Hashtable* hashtable);

#endif