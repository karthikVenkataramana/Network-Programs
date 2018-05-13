/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 02/10/ 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: A client program which interacts with the server. Sends an arithmetic expression and reads the result. 
    Compilation: gcc client.c -o client
    Execution: ./client <HOST NAME> <port no> 
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h> 

void raiseException(char * msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char * argv[]) {
    int fd, n;
    struct sockaddr_in server_address;
    struct hostent * server; // For translating domain name to IP address
    char buffer[128];
    if (argc < 3)
        raiseException("Enter IP and Port Number");
    // Step 1: create the socket.
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        raiseException("Error creating socket");
    // Step 2: Translate domain name to IP address info.
    if ((server = gethostbyname(argv[1])) == NULL) // gets the details of host including IP address
        raiseException("INVALID HOST NAME\n");
    memset((char * ) & server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET; // IPV4 addresses 
    memcpy((char * ) & server_address.sin_addr.s_addr, // Dest
        (char * ) server -> h_addr, // src  
        server -> h_length); // size
    server_address.sin_port = htons(atoi(argv[2]));
    // Step 3: Connect to server.
    if (connect(fd, (struct sockaddr * ) & server_address, sizeof(server_address)) < 0)
        raiseException("ERROR connecting");
    do {
        printf("EXPRESSION: ");
        memset(buffer, '\0', 128);
        fgets(buffer, 127, stdin); // Read expression from the user.
        int len = strlen(buffer); // fgets reads a newline input as well, so removing it.
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        if ((n = write(fd, buffer, strlen(buffer))) < 0)
            raiseException("ERROR write to socket.");
        memset(buffer, '\0', 128);
        if ((n = read(fd, buffer, 127)) < 0)
            raiseException("ERROR reading the result from socket. Try changing port numbers.");
        puts(buffer);
    } while (strcmp(buffer, "bye bye!") != 0);
    return 0;
}