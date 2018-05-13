/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 02/10/ 2018
    Description: Performs evaluation of expression and contains helper methods.
    Individual compilation: gcc evaluate.c stack.c -c -lm  
    Refs: partially adapts https://www.geeksforgeeks.org/expression-evaluation/ 

*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <math.h> 
#include <float.h>  
#include "evaluate.h"

#define E 2.71828182846 // 'e' value

int i = 0; // To keep track at which character in token we are at.

/*
	@param: op - Operator to find precedence.
	@returns: Precedence of the operator as defined by C99 standard.
*/
int precedence(char op) { 
    if (op == 'g' || op == 's' || op == '^') // Highest precedence
        return 2;
    if (op == '*' || op == '/') // C99 Standard precedence.
        return 3;
    if (op == '+' || op == '-') // least precedence.
        return 4;
    return -1;
}

/*
    @desc: Pretty much self-explanatory. Pops or peeks into the respective stacks.
*/
float popOperandStack(stack * operands) {
    return (* ((float * ) pop(operands)));
}

char popOperatorStack(stack * operators) {
    return ( * ((char * ) pop(operators)));
}

char peekOperatorStack(stack * operators) {
    return ( * ((char * ) peek(operators)));
}

float peekOperandStack(stack * operands) {
    return ( * ((float * ) peek(operands)));
}

/*
	@param: op1, op2  - Two operators to compare precedence.
	@returns: 1 if op1 has higher or equal precedence than op2, -1 otherwise.
*/
int hasHigherPrecedence(char op1, char op2) {
    if (precedence(op1) <= precedence(op2)) // op1 has higher precedence than op2.
        return 1;
    return -1;
}

/*
	@param: op - Operator.
            c1 - first operand (float)
            c2 - second operand
	@returns: Returns result of performing op on c1 and c2.
*/
float performOp(char op, float c1, float c2) {
    float result = -INFINITY;
    switch (op) {
    case '+':
        result = c1 + c2;
        break;
    case '-':
        result = c1 - c2;
        break;
    case '*':
        result = c1 * c2;
        break;
    case '/':
        result = c1 / c2;
        break;
    case '^':
        result = pow(c1, c2);
        break;
    case 'g':
        if (c2 > 0)
            result = log(c2) / log(10); // log(x) in math.h gives you natural log, but we want log to base 10.
        else
            errMsg = "Negative number to LOG!\n";
        break;
    case 's':
        if (c2 >= 0)
            result = sqrt(c2);
        else
            errMsg = "Negative number to Square Root!\n";
        break;
    default: 
        break; // Not supported operation.
    }
    return result;
}

/*
    @param - token - The token we are parsing.
    @return - Finds the number in token and returns it (Also, internally updates i (position in the token)).
*/
float findNumber(char * token) {
    int len = strlen(token); // Atmost, the length of token.
    int j = 0;
    float number = -INFINITY; 
    char * result = (char * ) malloc(sizeof(char) * (len + 1)); // Result will have the number in char array.
    while (isdigit( * (token + i)) || (( * (token + i)) == '.')) { // Read Floats also.
        char n = * (token + i); * (result + j) = n;
        j++;
        i++;
    }
    sscanf(result, "%f", & number); // Get's the float value from char array.
    return number;
}

/*
    @param - operands - A pointer to the operands stack.
           - op - Char operator
    @return - returns -Inf if result cannot be pushed due to parsing errors. 
*/
float pushResult(stack * operands, char op) {

    if ((size(operands) < 2) && (!((op == 'g') || (op == 's')))) { // Cause only log and sqrt can be applied on single operand.
        errMsg = "Has Operators at wrong places\n";
        return -INFINITY;
    }
    float c1 = popOperandStack(operands);
    float c2 = 0;
    if (!((op == 'g') || (op == 's'))) // Pop second operand, only if it is not log or sqrt operation.
        c2 = popOperandStack(operands);
    float * result = (float * ) malloc(sizeof(float));
    *result = performOp(op, c2, c1); // Important! Why? cause order matters! (a / b != b /a )  
    push(operands, (void * )(result)); // Pushed success!
    return *result;
}

/*
    @param - operands - pointer to operands stack.
             token - A token to parse.
    @desc - Reads a negative number from token and pushes it to operand stack.
*/
void readNegativeNumber(stack * operands, char * token) {
    float * number = (float * ) malloc(sizeof(float));
    * number = findNumber(token);
    if (* number != 0) { // Found a negative number. Gotcha!
        * number = -( * number); 
        push(operands, (void * )(number));
    }
}

/*
    Heart of our evaluation expression.
    @param - st - A char array, usually an expression to parse.
           - errMsg - Sets the error message. 
    @return - Returns the evaluated result! - Inf if the expression cannot be parsed.
*/
float evaluate(char st[]) {
    const char logarithm[4] = "log"; 
    const char square_root[5] = "sqrt";
    const char exponent = 'e'; 
    char * ret; 
    float result = -INFINITY; 
    // Three Stacks (1 - Operands , 2 - Operators, 3 - Paranthesis)
    stack * operands = (stack * ) malloc(sizeof(stack));
    stack * operators = (stack * ) malloc(sizeof(stack));
    stack * paranthesis = (stack * ) malloc(sizeof(stack));
    // Initialize all the stacks
    initializeStack(operands);
    initializeStack(operators);
    initializeStack(paranthesis);
    char * token = strtok(st, " "); // Space as a delimeter, Eliminate spaces first!
    while (token != NULL) {
        i = 0; // browse characters in the token from START!
        while ( * (token + i) != '\0') { // while not the end of token
            char c = * (token + i); // Get character
            char *temp = (char * ) malloc(sizeof(char)); 
            *temp = c; // copying 'c' into temp value.

            // Evaluating the paranthesis in a different stack.
            if (c == '(')
                push(paranthesis, (void * ) temp); // Push if '('
            else if (c == ')') {
                if (size(paranthesis) > 0)
                    pop(paranthesis);   // pop if ')'
                else {
                    errMsg = "Imbalanced paranthesis! \n";
                    return -INFINITY;
                }
            }

            // MAIN - LOGIC FROM HERE!
            if (isalpha(c) || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^') { // Valid character
                if (c == exponent) { // Case 'E'
                    float * r = (float * ) malloc(sizeof(float)); *r = E; // Push macro 'E' into operands stack.
                    push(operands, (void * ) r);
                    i++;
                } 
                else if (c == '(') { // Opening paranthesis (Simply push it into operators stack).
                    push(operators, (void * ) temp);
                    i++;
                } 
                else if (c == ')') { // Closing paranthesis.
                    // This requires some explanation. 
                    int x = 0;  // canary 
                    while (size(operators) > 0) { // Since popping empty stack raises a seg fault.
                        char poppedOp = popOperatorStack(operators);
                        if (poppedOp != '(') { // Till we find '(', ITERATIVELY apply operations
                            if (pushResult(operands, poppedOp) == -INFINITY)
                                return (-INFINITY);
                            x++; // Increment our canary variable.
                        }
                        else { 
                            if (x == 0) { // ')' preceeded by '(' (canary is not modified)
                                if (size(operators) > 0) { // There exists either log or sqrt (checking to avoid seg fault)
                                    poppedOp = popOperatorStack(operators);
                                    if (poppedOp != '(') { //  If it's not '(', apply operation
                                        if (pushResult(operands, poppedOp) == -INFINITY)
                                            return (-INFINITY);
                                    }
                                }
                            }
                            break;
                        }
                    }
                    i++; // parsed a character in token (update our position)!
                } 
                else if (c == '+' || c == '*' || c == '/' || c == '^') { // Arithmetic operators except our special '-'
                    if (size(operators) >= 1) { // To avoid seg faulting.
                        while (size(operators) > 0 && (peekOperatorStack(operators) != '(')) { // Find operator in stack with higher precedence than the parsing character token.
                            char op = peekOperatorStack(operators);
                            if (hasHigherPrecedence(op, c) == 1) { // top operator has more precedence than new operator.	
                                op = popOperatorStack(operators);
                                if (pushResult(operands, op) == -INFINITY)
                                    return (-INFINITY);
                            }
                            else
                                break;
                        }
                        char* temp = (char * ) malloc(sizeof(char)); * temp = c;
                        push(operators, temp); // Finally, push the parsing character operator.
                    } 
                    else {
                        char * temp = (char * ) malloc(sizeof(char)); * temp = c;
                        push(operators, (void * )(temp));
                    }
                    i++;
                } 
                else if (c == '-') { // Special because can either mean subtraction or negative number.
                
                    i++; // number starts after negative sign.
                    int prevIndexBeforeMinus = i - 2;
                    if (prevIndexBeforeMinus >= 0) { // As long as we are in bounds.
                        if ( * (token + prevIndexBeforeMinus) == '(') { // A negative number usually has ( before - operator.
                            readNegativeNumber(operands, token);
                        } else { // Just a subtraction operator :(
                            push(operators, (void * ) temp);
                        }
                    } 
                    else if ( * (token + i) == '\0') { // An empty space after - operator (Our tokenizer parsed into seperate tokens).
                        push(operators, (void * ) temp);
                    } 
                    else { // Cases like -12 + 6 (starting with negative number)
                        readNegativeNumber(operands, token);
                    }
                } 
                else {
                    // Done with almost all of them, except for log and sqrt.
                    ret = strstr((token + i), logarithm); // strstr() finds if substring is present in the string starting from token + i.
                    if (ret == NULL) { // No log in expression.
                        ret = strstr((token + i), square_root); // Now checking for SQRT.
                        if (ret != NULL) { // Found SQRT! 
                            * temp = 's';
                            push(operators, (void * )(temp));
                            i += 4; // SQRT is 4 characters!
                        } else {
                            // Invalid char! (NOT SQRT OR LOG, THEN WHAT ? HMMMMM ...)
                            errMsg = "Operation not supported.\n";
                            return INFINITY;
                        }
                    } else { // Found LOG!	
                        * temp = 'g';
                        push(operators, (void * )(temp));
                        i += 3; // log is 3 characters.
                    }
                }
            } 
            else if (isdigit(c)) { // If it's a number then push to operands stack.
                float * number = (float * ) malloc(sizeof(number));
                * number = findNumber(token);
                push(operands, (void * ) number);
            } 
            else {
                errMsg = "Check Expn \n";
                return INFINITY;
            }
        } // Done parsing the current token! Let's start with next token.
        token = strtok(NULL, " "); // NULL starts scanning from where it has left off. 
    }

    // Done with parsing, let's wrap up the rest.
    if (size(paranthesis) != 0) {  // more '(' than ')'
        errMsg = "Imbalanced paranthesis! \n";
        return -INFINITY;
    }
    // Apply remaining operations in operator stack iteratively.
    while (size(operators) > 0) {
        char op = popOperatorStack(operators);
        if (pushResult(operands, op) == (-INFINITY)) {
            return -INFINITY;
        }
    }
    // Negative number problems ...
    while (size(operands) > 1) {
        if (pushResult(operands, '+') == (-INFINITY)) { // If two or more numbers in the stack, then probably might be negative numbers, so just adding them up!  (Ex: -12 -6).
            return -INFINITY;
        }
    }

    result = popOperandStack(operands); // Top element in the stack is answer.

    freeStack(operators); // Free memory after use.
    freeStack(operands);
    freeStack(paranthesis);

    return result; 
}