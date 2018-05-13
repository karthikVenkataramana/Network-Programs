/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 12 / 2018 - 03 /15 / 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: A Proxy web server acting as both client and server. 
    Compilation: gcc server.c connections.c parser.c filehandler.c -o server -lssl 
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
#include <openssl/ssl.h>
#include "connections.h" 
#include "parser.h"
#include "filehandler.h"

/*
    @Desc: Handles HTTPS Requests. Sets up connection with remote web server and returns result to the original client.
*/
void ssl_handler(website* site, char *get_request, int webfd, char* url, int clientfd, char *client_name, maplist *map){
    sslSetup(webfd);    // With remote web server
    sslWrite(get_request); // Write GET Request onto SSL Socket. 
    sslRead(site, url, clientfd, client_name, map);   // Client is our original client.
    closeSSL(); 
}
/*
    @Desc: Handles HTTP Requests and does the same as above function. 
*/
void http_handler(website* site, char* get_request, int webfd, int clientfd, char * client_name, char *url,  maplist * map){
    writeToSocket(webfd, get_request);
    http_read(site, url, webfd, clientfd, client_name, map); 
}
 
 
void main_handler(int clientfd, struct sockaddr_in client_address, maplist* map){ 
    char clientName[512],  buffer[BUFFER_SIZE], url[512],  *get_request; 
    getnameinfo((struct sockaddr *)&client_address, sizeof(client_address),
                clientName, sizeof(clientName), NULL, 0, NI_NAMEREQD); // Gets the name of the client connected to the socket.
    read_url(clientfd, buffer, clientName); // Reads data from socket onto buffer.
    printf("***********************************\n");
    if (strcmp(buffer, "") > 0) {                                                        // Client enters a URL.
        memset(url, '\0', 512);
        strncpy(url, buffer, 512);
        website *site = parse_url(buffer);                   // parse the URL from client to get port, base and relative URL's.
        int webfd = clientSetup(site->base_url, site->port); // Set up a socket with that URL (Now Server acts as a client).
        list * file_details = get_list(url, map); 
        if(file_details != NULL){ // Already exists in our cache.
            printf("File already exists in our cache! Checking for modifications....\n");
            get_request = build_not_modified_request(site, file_details);   
        }
        else{
            get_request  = build_get_request(site);    
        }
        if (strcmp(site->port, "443") == 0)  { // A HTTP website is requested by client.
                ssl_handler(site, get_request, webfd, url, clientfd, clientName, map);
        }
            else   { // A HTTPS Request.
                http_handler(site, get_request, webfd, clientfd, clientName, url, map);
        }
        close(webfd);
    } 
}

int main(int argc, char * argv[]) {
    int fd, clientfd, n;
    char dir_path[BUFFER_SIZE],  clientName[BUFFER_SIZE] ;   
    struct sockaddr_in server_address, client_address;
    int length = sizeof(client_address);    
    fd = serverSetup(argv[1]);
    memset(dir_path, '\0', BUFFER_SIZE);
    strcpy(dir_path, get_work_dir());
    mkdir(strcat(dir_path, "/proxy_webpages/"), 0777);
    maplist *map =  read_from_file();
    if(map == NULL){ // No mapping our list.txt file.
        map = (maplist*) malloc(sizeof(maplist)); // Initial values of our map data structure.
        map -> current_index = 0;
        map -> isValid = 1;
    }
    while (TRUE) {  
        if ((clientfd = accept(fd, (struct sockaddr * ) & client_address, & length)) < 0)  // Step 5: Accept any incoming connections at this socket. Note that accept returns socket information of client.
            raiseException("ERROR on accept");
        main_handler(clientfd, client_address, map); 
        close(clientfd);
    }
    free(map);
}