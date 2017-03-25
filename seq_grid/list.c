#include "list.h"

/****************************************************************************/
void insertCell(Node** head, int z, int status){ //ordered list insert
    
    //Node** head = graph[x][y];

    if(*head == NULL){
        Node* new = createNode(z,status);
        new->next = NULL;
        (*head) = new;
        return;
    }else if((*head)->z == z){ //skip duplicates

        (*head)->counter++;

    }else if((*head)->z > z){ //insert at start, z is smaller

        Node* new = createNode(z,status);
        new->next = *head;
        (*head) = new;
    }else{ //insert after, somewhere in the middle or end

        Node* aux;
        for(aux = *head; (aux->next != NULL) && (aux->next->z < z); aux = aux->next){}

        if(aux->next != NULL){
            if(aux->next->z == z){
                aux->next->counter++;
                return;
            }
        }
        Node* new = createNode(z,status);
        new->next = aux->next;  
        aux->next = new;
    }
}

/****************************************************************************/
Node* createNode(int z, int status){
    Node* n = (Node*)malloc(sizeof(Node));
    n->z = z;
    n->status = status;
    n->counter = !status;
    return n;
}

/****************************************************************************/
Node**** initGraph(int size){
    int x, y;
    Node**** graph = (Node****) malloc(sizeof(Node***) * size);
    
    for (x = 0; x < size; x++){
        graph[x] = (Node***) malloc(sizeof(Node**) * size);
        for (y = 0; y < size; y++){
            graph[x][y] = (Node**)malloc(sizeof(Node*) * size);
            memset(graph[x][y], 0, size * sizeof(graph[x][y]));
        }
    }
    return graph;
}