#include "hash.h"

int main(int argc, char* argv){

    /* hash */

    printf("Creating hashtable and inserting two points (1,2,3) and (3,2,1)\n");
    
    Hashtable* htable = createHashtable(20);

    hashtableWrite(htable, 1, 2, 3, NULL);
    hashtableWrite(htable, 3, 2, 3, NULL);
    hashtableWrite(htable, 3, 2, 1, NULL); //This cause a colision

    printHashtable(htable);
    printf("Occupied: %d\n", htable->occupied);

    hashtableRemove(htable, 1, 2, 3);
    printf("Hashtable after removing 1,2,3\n");

    printf("Occupied: %d\n", htable->occupied);
    printHashtable(htable);

    hashtableRemove(htable, 3,3,3); //Node does not exist
    hashtableWrite(htable, 3,2,3, NULL); //Node already exists

    printf("Hashtable after removing inserting already existant element and removing unexistant element\n");
    printHashtable(htable);
    
    hashtableFree(htable);

}