/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 02 / 11 / 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: A server which takes a Math expression from client and returns the evaluated result back to it. 
    Compilation: gcc server.c stack. evaluate.c -lm -o server 
    Refs: http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html 
    Execution: ./server <port no> (Any ephermal port range 1024 and 65535). 
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <string.h> 
#include <math.h> 
#include "evaluate.h"

void raiseException(char * msg) {
    perror(msg);
    exit(0);
}

/*
    @desc - prints the char array even after encountering space. (Generally, printf() would terminate after seeing space).
*/
void printExpression(char * msg) {
    int pos = 0;
    for (pos = 0; pos < 128; pos++) {
        if (!isspace( * (msg + pos)))
            printf("%c", * (msg + pos));

    }
}

int main(int argc, char * argv[]) {
    int fd, clientfd, length, n, opt = 1;
    char buffer[128];
    float result;
    struct sockaddr_in server_address, client_address;
    char clientName[30];
    // Step 1: create socket.
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        raiseException("ERROR opening socket");
    memset((char * ) & server_address, '\0', sizeof(server_address));
    length = sizeof(client_address);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(argv[1]));
    // Step 2: Bind socket to an address. 
    if (bind(fd, (struct sockaddr * ) & server_address, sizeof(server_address)) < 0)
        raiseException("ERROR on binding");
    // Step 3: Start listening at the socket.
    listen(fd, 3);
    // Step 4: Accept any incoming connections at this socket. Note that accept returns socket information of client.
    if ((clientfd = accept(fd, (struct sockaddr * ) & client_address, & length)) < 0)
        raiseException("ERROR on accept");
    while (1) {

        memset(buffer, '\0', 128); // Empty buffer.
        if ((n = read(clientfd, buffer, 127)) < 0) // Read Expression from client.
            raiseException("ERROR reading from socket");
        getnameinfo((struct sockaddr * ) & client_address, sizeof(client_address),
            clientName, sizeof(clientName),
            NULL, 0, NI_NAMEREQD); // Gets the name of the client connected to the socket.

        if (strcmp(buffer, "quit") == 0) { // See if the client typed 'quit'.
            if ((n = write(clientfd, "bye bye!", 10)) < 0)
                raiseException("ERROR writing to socket");
            printf("Client %s closed the connection. \n", clientName);
            close(clientfd);
            break;
        } else { // Else, if a valid expression, call our evaluate() method to give us result. 
            char msg[127] = "RESULT: ";
            result = evaluate(buffer);
            if ( (result != -INFINITY) && (result != INFINITY)) {
                snprintf(msg + 8, sizeof(msg), "%f", result); // converts float into char array.
            } else {
                strncpy(msg, errMsg, strlen(errMsg));
                strncpy(msg + strlen(errMsg), "Try Again!", 15);
            }
            printf("Just served expression: ");
            printExpression(buffer);
            printf(" For Client: %s \n", clientName);
            // Got the result in buffer, now just writing it to client.
            if ((n = write(clientfd, msg, 127)) < 0)
                raiseException("ERROR writing to socket");
        }
    }
    return 0;
}