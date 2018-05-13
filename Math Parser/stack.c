/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 02/10/ 2018
    Description: A generic stack implementation
*/

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"

void initializeStack(stack * st) {
    st -> topNode = NULL; // Initializing top node to NULL
}

void freeStack(stack * st) {
    free(st -> topNode); // Release memory back to OS.
}

/*
    @param st - a pointer to stack which has a pointer to top node (That's all we need!).
           data - can be any type (to be pushed into stack).
*/
void push(stack * st, void * data) {
        Node * newNode = (Node * ) malloc(sizeof(Node));
        newNode -> data = data;
        if (st -> topNode == NULL) { // Empty stack.
            newNode -> prevNode = NULL; // First node in the stack.
            st -> topNode = newNode;
        } else {
            newNode -> prevNode = st -> topNode; // Just update our current's previous node to top Node.
            st -> topNode = newNode;
        }
    }
/*
    @desc - peeks at the top of stack but doesn't pop it.
*/
void * peek(stack * st) {
    if (st -> topNode != NULL)
        return st -> topNode -> data;
    return NULL;
}

/*
    @desc - popping the stack pointed by st.
*/
void * pop(stack * st) {
    void * data = malloc(sizeof(void * ));
    if (st -> topNode != NULL) { // If not empty, 
        data = st -> topNode -> data;
        st -> topNode = st -> topNode -> prevNode;
    }
    return data;
}

/*
    @desc - returns the size of stack.
*/
int size(stack * st) {
    int s = 0;
    Node * temp = st -> topNode;
    while (temp != NULL) {
        temp = temp -> prevNode;
        s++;
    } // while we don't hit the bottom rocks.
    return s;
}