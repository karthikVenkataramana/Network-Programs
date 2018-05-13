/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 15 / 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu 
*/ 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
 #include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h> 
#include <sys/stat.h>
#include "connections.h"
#include "parser.h"


/*
    @Desc: Provides basic infrastructure for client and return socket file descriptor.
*/
int clientSetup(char* url, char* port){
    int fd, n;
    struct sockaddr_in server_address;
    struct hostent * server; // For translating domain name to IP address
    // Step 1: create the socket.
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        raiseException("Error creating socket");
    // Step 2: Translate domain name to IP address info.
    if ((server = gethostbyname(url)) == NULL) // gets the details of host including IP address
        raiseException("INVALID HOST NAME\n");
    memset((char * ) & server_address, '\0', sizeof(server_address));
    server_address.sin_family = AF_INET; // IPV4 addresses 
    memcpy((char * ) & server_address.sin_addr.s_addr, // Dest
        (char * ) server -> h_addr, // src  
        server -> h_length); // size
    server_address.sin_port = htons(atoi(port));
    // Step 3: Connect to server.
    if (connect(fd, (struct sockaddr * ) & server_address, sizeof(server_address)) < 0)
        raiseException("ERROR connecting");
    return fd;
}
  
int serverSetup(char *port){
    int fd, opt = 1; 
    struct sockaddr_in server_address;
    // Step 1: create socket.
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // SOCK_STREAM FOR TCP, SOCK_DATAGRAM FOR UDP
        raiseException("ERROR opening socket");
    memset((char * ) & server_address, '\0', sizeof(server_address));
    // Step 2: Force use the socket and allow multiple connections
    if( setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  sizeof(opt)) < 0 ) {
        raiseException("setsockopt");   
    }  
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(atoi(port));
    // Step 3: Bind socket to an address. 
    if (bind(fd, (struct sockaddr * ) & server_address, sizeof(server_address)) < 0)
        raiseException("ERROR on binding");
    // Step 4: Start listening at the socket.
    listen(fd, 3);
    return fd;
}

int read_url(int fd, char *buffer,char * clientname){
    memset(buffer, '\0', BUFFER_SIZE); // Empty the buffer.
    if ((n = read(fd, buffer , BUFFER_SIZE - 1)) < 0) { // Read Expression from socket.
        raiseException("Error reading URL from client \n");
    } 
    printf("\nClient: %s requested url: %s\n", clientname, buffer);
    return n;
}

void writeToSocket(int fd, char* buffer){
    if ((n = write(fd, buffer, strlen(buffer))) < 0)
        raiseException("ERROR write to socket.");
}
void sslSetup(int serverfd){
    SSL_load_error_strings ();
    SSL_library_init ();    // Load required libraries.
    ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    conn = SSL_new(ssl_ctx);    // create an SSL connection and attach it to the socket
    SSL_set_fd(conn, serverfd); 
    if ((n = SSL_connect(conn)) != 1){  
       raiseException("Cannot connect using SSL");
    }
}
void sslWrite(char *buffer){
    if ((n = SSL_write(conn, buffer, strlen(buffer))) < 0){ 
        raiseException("Error in SSL Write ");
    }
}

void  sslRead(website * site, char *url, int clientfd, char *client_name, maplist *map){
    response *rsp;
    FILE *fp;
    char file_name[SIZE];
    char filepath[SIZE];
    char buffer[BUFFER_SIZE];
    int header = TRUE;
    memset(filepath, '\0', BUFFER_SIZE);  
    strncpy(filepath, "proxy_webpages/",15); 
    memset(buffer, '\0', BUFFER_SIZE);
    printf("Contacting %s ...\n", url); 
    while ((n = SSL_read(conn, buffer, BUFFER_SIZE) > 0)) { // Read in 4098 Bit chunks
        if (header == TRUE) {
            char *temp_buffer = (char *)malloc(BUFFER_SIZE);
            strncpy(temp_buffer, buffer, BUFFER_SIZE); 
            rsp = parse_response(temp_buffer);  
            printf("Response Code: %d\n", rsp -> code); // Response code is built in parse_response() function/
            if(rsp -> code == 304){ // Not modified response.
                printf("%s is not modified, retrieving ...\n", url);
                list * list = get_list(url, map);
                int len = strlen(list -> filename); 
                strncpy(file_name , list -> filename, len + 1); 
                file_name[strlen(file_name) ] = '\0';
                strcat(filepath, file_name);  
                send_file(filepath,clientfd,client_name, 0);  
                return;
            }
            strftime(file_name, 32, "%Y%m%d%H%M%S", &(rsp->visit_time));
            strcat(file_name, ".");
            if(strcmp(site -> extension, "") > 0)
            {
                strcat(file_name,  site -> extension);  // If exists, use the extension.
            }
            else{
                strcat(file_name,  "html");
            }
            char *content = strstr(buffer, "\r\n\r\n"); // Message body of GET Request is seperated from header using \r\n\r\n
            int pos = content - buffer + strlen("\r\n\r\n") -1; 
            strcat(filepath,file_name); 
            fp = fopen(filepath, "w+");
            fputs(buffer + pos , fp);
            header = FALSE;
        }
        else
            fputs(buffer, fp);
        memset(buffer, '\0', BUFFER_SIZE);
    } 
    fclose(fp);
    printf("Succesfully received webpage from %s\nSaved Webpage at: %s\n", url, filepath);
    send_file(filepath,clientfd,client_name, 0); 
    add_mapping(url, file_name, map, rsp -> time_stamp);
    printf("-------------------------------------------------------------------------------\n\n");
}   
 

void closeSSL(){
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx); 
}
/*
    @Desc: Get's ip address of the caller. Useful in building directory name.
*/
char* get_my_ip_address(int fd){
    char *my_ip = (char *)malloc(32);
    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    strcpy(my_ip,inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)); 
    return my_ip;
}
/*
 This function is similar to SSLRead except for now we are reading HTTP. Didn't get enough time to concise the code.
*/
void http_read(website * site, char *url, int webfd, int clientfd, char *client_name, maplist *map){
 response *rsp;
    FILE *fp;
    char file_name[SIZE];
    char filepath[BUFFER_SIZE];
    int header = TRUE;
    char buffer[BUFFER_SIZE];
    memset(filepath, '\0', BUFFER_SIZE);  
    memset(buffer, '\0', BUFFER_SIZE);
    strncpy(filepath, "proxy_webpages/",15); 
    printf("Contacting %s ...\n", url); 
    while ((n = read(webfd, buffer, BUFFER_SIZE) > 0)) { // Read in 256 Bit chunks
        if (header == TRUE) {
            char *temp_buffer = (char *)malloc(BUFFER_SIZE);
            strncpy(temp_buffer, buffer, BUFFER_SIZE); 
            rsp = parse_response(temp_buffer);  
            printf("Response Code: %d\n", rsp -> code);
            if(rsp -> code == 304){ // Not modified response.
                printf("%s is not modified, retrieving ...\n", url);
                list * list = get_list(url, map);
                int len = strlen(list -> filename); 
                strncpy(file_name , list -> filename, len + 1); 
                file_name[strlen(file_name) ] = '\0';
                strcat(filepath, file_name);  
                send_file(filepath,clientfd,client_name, 0);  
                return;
            }
            strftime(file_name, 32, "%Y%m%d%H%M%S", &(rsp->visit_time));
            strcat(file_name, ".");
            if(strcmp(site -> extension, "") > 0)
            {
                strcat(file_name,  site -> extension);
            }
            else{
                strcat(file_name,  "html");
            }
            char *content = strstr(buffer, "\r\n\r\n"); // Message body of GET Request is seperated from header using \r\n\r\n
            int pos = content - buffer + strlen("\r\n\r\n") -1; 
            strcat(filepath,file_name); 
            fp = fopen(filepath, "w+");
            fputs(buffer + pos , fp);
            header = FALSE;
        }
        else
            fputs(buffer, fp);
        memset(buffer, '\0', BUFFER_SIZE);
    } 
    fclose(fp);
    printf("Succesfully received webpage from %s\nSaved Webpage at: %s\n", url, filepath);
    send_file(filepath,clientfd,client_name, 0); 
    add_mapping(url, file_name, map, rsp -> time_stamp);
    printf("------------------------------------------------------------------------------------\n\n");
}

void raiseException(char * msg) {
    perror(msg);
    exit(0);
}