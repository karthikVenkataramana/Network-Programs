/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 12 / 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: A client interacting Web server using proxy. 
    Compilation: gcc client.c -o client
    Execution: ./client <HOST NAME> <port no> 
*/

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "connections.h"
#include "parser.h" 
#include "filehandler.h"


void create_directory_with_ip(int fd){
    char *name = get_my_ip_address(fd); 
    mkdir(strcat(strcat(get_work_dir() ,"/"), name), 0777);
}

int main(int argc, char *argv[]) {
    int fd, n;
    char buffer[BUFFER_SIZE], webpage[BUFFER_SIZE], *myname;
    if (argc < 3)
        raiseException("USAGE: ./executable <url> <port>");
    fd = clientSetup(argv[1], argv[2]);  
    create_directory_with_ip(fd);
    do {
        printf("url: ");
        memset(buffer, '\0', BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE , stdin); // Read URL from the client.
        int len = strlen(buffer); // fgets reads a newline input as well, so removing it.
        if(buffer[len - 1] == '\n')
            buffer[len - 1] = '\0'; 
        writeToSocket(fd, buffer);  // Transmit URL to server.
        recv_file(fd);
        break;
    } while (strcmp(buffer, "bye-bye!") != 0);
}