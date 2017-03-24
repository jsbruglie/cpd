#include "lists.h"

int main(int argc, char* argv){

    /* Graph Node Lists */

    printf("Creating graph list and inserting 1,2,3,4,5\n");
    GraphNode* first = NULL;
    first = graphNodeInsert(first, 1);
    first = graphNodeInsert(first, 2);
    first = graphNodeInsert(first, 3);
    first = graphNodeInsert(first, 4);
    first = graphNodeInsert(first, 5);
    GraphNode* git;
    for (git = first; git != NULL; git = git->next){
        printf("z:%d\n", git->z);
    }

    printf("Removing 4\n");    
    graphNodeRemove(&first, 4);
    for (git = first; git != NULL; git = git->next){
        printf("z:%d\n", git->z);
    }
    printf("Removing 1\n");    
    graphNodeRemove(&first, 1);
    for (git = first; git != NULL; git = git->next){
        printf("z:%d\n", git->z);
    }

    graphNodeDelete(first);

    /* Node Lists */

    printf("Creating list and inserting (0,0,0) (0,1,0) (0,0,1) (1,1,1)\n");
    List* list = listCreate();
    listInsert(list, 0, 0, 0, (GraphNode*) 10);
    listInsert(list, 0, 1, 0, (GraphNode*) 20);
    listInsert(list, 0, 0, 1, (GraphNode*) 30);
    listInsert(list, 1, 1, 1, (GraphNode*) 40);
    Node* it;
    for (it = list->first; it != NULL; it = it->next){
        printf("x:%d y:%d z:%d\n", it->x, it->y, it->z);
    }

    printf("Removing (1,1,1)\n");
    listRemove(list, 1, 1, 1);
    for (it = list->first; it != NULL; it = it->next){
        printf("x:%d y:%d z:%d\n", it->x, it->y, it->z);
    }

    printf("Removing (0,1,1) [not present] and (0,1,0)\n");
    listRemove(list, 0, 1, 1);
    listRemove(list, 0, 1, 0);
    for (it = list->first; it != NULL; it = it->next){
        printf("x:%d y:%d z:%d\n", it->x, it->y, it->z);
    }

    listDelete(list);

}