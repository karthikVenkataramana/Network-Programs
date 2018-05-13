/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 04 / 23/ 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: Emulates DHCP server. 
    Compilation: gcc server.c -lm -o server 
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
#include "assigned_ip.h"
#include "iprange.c"
 
int isNumeric(char *str) 
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return -1;
        str++;
    }
    return 1;
}

int get_id(char* str){
    char *token = strtok(str, " ");
    while (token != NULL){
        if(isNumeric(token) == 1)
            break;
        token = strtok(NULL, " ");
    }
    return atoi(token);
}

char * get_ip(char *buffer){
    char *temp = malloc(18);
    memset(temp, '\0', 18);
    char * token = strtok(buffer, " ");
    int first = 0;  // the yiaddr part. 
    while(token != NULL){
        if(first == 1){
            strncpy(temp, token, strlen(token));
            break;
        }
        else if(first == 0){
            first = 1;
            memset(token, '\0' , strlen(token));
            token = strtok(NULL, " ");
        }   
    } 
    return temp;
}

int main(int argc, char * argv[]) {
    int fd, clientfd, length, n, transaction_id;
    char buffer[256]; char transaction[4];
    float result;
    struct sockaddr_in server_address, client_address;
    char clientName[30]; char gateway[17], subnet_mask[17], ip[17];
    
    assigned_ip* asp = (assigned_ip*) malloc(sizeof(assigned_ip));
    current_ip* csp = (current_ip*)malloc(sizeof(current_ip));

    asp->cur_index = 0;
    memset(gateway, '\0', 17);
    printf("gateway: ");
    fgets(gateway, 17, stdin);
    int len = strlen(gateway);
    if (gateway[len - 1] == '\n')
            gateway[len - 1] = '\0';
    memset(subnet_mask,'\0', 17 );
    printf("subnet_mask: ");
    fgets(subnet_mask, 17, stdin);
    len = strlen(subnet_mask);
    if (subnet_mask[len - 1] == '\n')
        subnet_mask[len - 1] = '\0';
    // Step 1: create socket.
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        raiseException("ERROR opening socket");
    memset((char * ) & server_address, '\0', sizeof(server_address));
    length = sizeof(client_address);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(argv[1]));
    // Step 2: Bind socket to an address. 
    if (bind(fd, (struct sockaddr * ) & server_address, sizeof(server_address)) < 0)
        raiseException("ERROR on binding");
    while (1) {
        memset(buffer, '\0', 256); // Empty buffer.
        // 1. DHCP DISCOVER
        if ( (n = recvfrom(fd, buffer, 255, 0, (struct sockaddr *) &client_address, &length) ) < 0) // Read Expression from client.
            raiseException("ERROR reading from socket");
        getnameinfo((struct sockaddr * ) & client_address, sizeof(client_address),
            clientName, sizeof(clientName), NULL, 0, NI_NAMEREQD); // Gets the name of the client connected to the socket.
        printf( "%s says: \n%s\n", clientName, buffer);
        // 2. DHCP OFFER.
        // get the ID from buffer.
        transaction_id = get_id(buffer);
        get_ip_address(gateway, subnet_mask, asp, csp);
        memset(buffer, '\0', 256); // Empty buffer.
        strcat(buffer, "DHCP OFFER: yiaddr: ");
        int in = 0;
        while(in < 3){
            strcat(buffer, csp -> client_ip[in]);
            strcat(buffer, ",");
            in++;
        }   // Added all ip offers to buffer. 
        strcat(buffer, "\ntransaction ID: ");
        sprintf(transaction, "%d", transaction_id);
        strcat(buffer, transaction);
        strcat(buffer, "\nlifetime: 3600 secs.\n****************************\n");
        printf("******************************\nSENT DHCP OFFER \n %s ", buffer);
        if ((n = sendto(fd, buffer, 255, 0, (struct sockaddr *) &client_address, sizeof(client_address) )  )  < 0)
            raiseException("ERROR write to socket."); 
        // 3. DHCP request
        memset(buffer, '\0', 256);
        length = sizeof(client_address);
        if ( (n = recvfrom(fd, buffer, 255, 0, (struct sockaddr *) &client_address, &length) ) < 0) // Read Expression from client.
            raiseException("ERROR reading from socket");
        printf( "%s says: \n%s\n", clientName, buffer);
        printf("********************\n");
        strcpy(ip, get_ip(buffer)); 
        strncpy(asp->client_ip[asp->cur_index], ip, sizeof(asp->client_ip[asp->cur_index]));
        asp->cur_index = asp->cur_index + 1;
        // 4. DHCP ACK
        memset(buffer, '\0', 256);
        strcat(buffer, "*****************\nyiaddr: ");
        strcat(buffer, ip);
        memset(transaction, '\0', 4);
        transaction_id++;
        sprintf(transaction, "%d", transaction_id);
        strcat(buffer, "\ntransaction ID: ");
        strcat(buffer, transaction);
        strcat(buffer, "\nlifetime: 3600 secs\n*****************\n");
        printf("SENT DHCP ACKNOWLEDGMENT \n %s ", buffer);
        if ((n = sendto(fd, buffer, 255, 0, (struct sockaddr *) &client_address, sizeof(client_address) )  )  < 0)
            raiseException("ERROR write to socket.");
        printf("Done serving client %s\n", clientName);
        printf("################################\n");
    }
    return 0;
}