/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 02/10/ 2018 
*/
#pragma once // File guard.
typedef struct Node {
    void * data; // Generic data (char (operators) or float(operators)).
    struct Node * prevNode; // Storing previous node in place of next node, as it easy to deal with stacks this way.
}
Node;

typedef struct stack {
    Node * topNode; // Pointing to topNode.
}
stack;

// The following functions are defined in stack.c
void initializeStack(stack * st);
void freeStack(stack * st);
void push(stack * st, void * data);
void * peek(stack * st);
void * pop(stack * st);
int size(stack * st);