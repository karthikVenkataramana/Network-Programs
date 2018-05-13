#pragma once
#include <openssl/ssl.h>
#include "parser.h"
#include "filehandler.h"
/*
    @Desc: Sets up TCP socket connection with remote server at url with specified port. 
*/
int clientSetup(char *url, char *port);
/*
    @Desc: Raises an exception and prints specified 'msg' 
*/
void raiseException(char * msg);
/*
    @Desc: Sets up TCP socket, binds and listens to specified port. 
*/
int serverSetup(char *port);

int read_url(int fd, char *buffer, char *clientname);
void writeToSocket(int fd, char* buffer);
/*
    @Desc: Sets up SSL connection on top of TCP socket(specified by server fd). 
*/
void sslSetup(int serverfd);

// Included all the core logic in here. Need to better segregate in future!
void sslRead(website * site, char *url, int clinetfd, char *client_name, maplist *map);

void sslWrite(char *buffer);

void closeSSL();

char* get_my_ip_address(int fd); 
int n;  // A placeholder for errors in read / write. 
SSL_CTX *ssl_ctx;   // OpenSLL connection variables.
SSL *conn ;
void http_read(website * site, char *url, int webfd, int clientfd, char *client_name, maplist *map);