#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Structure for storing a list of server identities
 */
typedef struct _Node{
	int z;               /**< Flag used for scheduling an entry for posterior deletion */ 
    struct _Node* next; /**< Pointer to the next entry in the list */
}ServerID;



#endif