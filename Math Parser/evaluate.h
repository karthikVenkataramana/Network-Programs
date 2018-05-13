#pragma once // Oh, the evil file guards are necessary.
#include "stack.h" 

/*
    Has the following functions defined in evaluate.c 
    More detailed description in the corresponding '.c' file.
*/
char* errMsg;
int precedence(char op);
float popOperandStack(stack * operands) ;
char popOperatorStack(stack * operators);
char peekOperatorStack(stack * operators) ;
float peekOperandStack(stack * operands) ;
int hasHigherPrecedence(char op1, char op2);
float performOp(char op, float c1, float c2);
float findNumber(char * token);
float pushResult(stack * operands, char op);
void readNegativeNumber(stack * operands, char * token);
float evaluate(char st[]);