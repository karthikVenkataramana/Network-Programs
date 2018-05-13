/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 12 / 2018 
*/
#include "parser.h"
#include "filehandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
    @Desc: Reads the data from buffer and extracts port number, base and relative URL's.
*/
website* parse_url(char *buffer){
    website* site = (website*) malloc(sizeof(website));
    const char delimeters[3] = " :/";
    int firstToken = TRUE;
    int secondToken = FALSE;
    int thirdToken = FALSE; 
    char*  token = strtok(buffer, delimeters);
    memset(site -> extension, '\0', SIZE);  
    memset(site -> relative_url, '\0', SIZE); 
    memset(site -> port, '\0', SIZE);
    memset(site -> base_url, '\0', SIZE);
    while(token != NULL){
        if(firstToken == TRUE){ 
            if(strcmp(token, "http") == 0)
                strcpy(site -> port, "80");
            else if(strcmp(token, "https") == 0) 
               strcpy(site -> port , "443");
            else{
                printf("PLEASE CHECK THE URL AGAIN! PROXY SERVER SUPPORTS ONLY HTTP AND HTTPS PROTOCOLS FOR NOW :(\n");
                exit(0);
            }
            firstToken = FALSE;
            secondToken = TRUE;
            token = strtok(NULL, delimeters); // NULL continues scanning from where it left of.
        }
        else if(secondToken == TRUE){
            strcpy(site -> base_url, token);
            secondToken = FALSE;
            token = strtok(NULL, ". "); // After relative URL, the file extension contains '.' .
            thirdToken = TRUE;
        }
        else if(thirdToken == TRUE){
            strcpy(site -> relative_url,token); 
            token = strtok(NULL, ". "); 
            thirdToken = FALSE; 
        }
        else{  
            strcpy(site -> extension ,token);
            puts(token);
            token = NULL;   // Got all we want, stop looping.
            break;
        }
    }  
    return site;
} 

/*
    @Desc: Builds the GET Request to be sent to remote web server.
    Usually in the following format:
        GET /<relative_url>  HTTP/1.0
        Host: www.unt.edu
        User-Agent: * 
*/
 
char* build_get_request(website * site) {
    char *getRequest = (char*) malloc(BUFFER_SIZE); // A max of 256 character URL
    memset(getRequest, '\0', BUFFER_SIZE);
    int urllen, cur = 0; 
    strncpy(getRequest, "GET /", 5); 
    cur += 5;
    if(strlen(site -> relative_url) > 0){ 
        urllen = strlen(site -> relative_url);
        strncpy(getRequest + cur, site -> relative_url, urllen);
        cur += urllen;
    }
    if(strlen(site -> extension ) > 0){
        strcat(getRequest, ".");
        cur += 1;
        int extlen = strlen(site -> extension);
        strncpy(getRequest + cur, site -> extension, extlen);
        cur += extlen;
    } 
    strncpy(getRequest + cur, " HTTP/1.0\r\nHost: ",17 );
    cur += 17;
    urllen = strlen(site -> base_url);
    strncpy(getRequest + cur, site -> base_url, urllen);
    cur += urllen;
    strncpy(getRequest + cur, "\r\nUser-Agent: *\r\n\r\n\0", 20); 
    return getRequest;
}
char* build_not_modified_request(website * site, list* list ) {
    int len, time_len;
    char *getRequest = (char*) malloc(BUFFER_SIZE); // A max of 1024 character URL
    memset(getRequest, '\0', BUFFER_SIZE);
    strncpy(getRequest, build_get_request(site), strlen(build_get_request(site)) - 1); 
    struct tm tm;
    strptime(list -> filename, "%Y%m%d%H%M%S", &tm); // Get time from file name years-months-days-hours-minutes-seconds.
    len = strlen(getRequest) ; // overwriting \r\n\0
    strncpy(getRequest + len , "Content-Type: text/html;charset=UTF-8\r\n", 39);
    len += 39;  // Since we are starting from len - 1.
    strncpy(getRequest + len , "If-Modified-Since:  ", 19);
    len += 19;
    time_len  = strlen(list -> time_stamp);
    strncpy(getRequest + len, list -> time_stamp, time_len);
    len += time_len;
    strncpy(getRequest + len, "\r\n\r\n\0", 5);
    getRequest[len + 4] = '\0';
    return getRequest;
}
/*
    @Desc: Parses the response from remote web server and builds our response data structure.
*/
response* parse_response(char *buffer){
    response *rsp = (response *) malloc(sizeof(response));
    char *whole_token_differentiator;
    char *token = strtok_r(buffer, "\r\n", &whole_token_differentiator); // strtok does not work in case of nested strtok's so using strtok_r.
    int firstToken = TRUE;
    while(token != NULL){
        if(firstToken == TRUE){ // In The first Line of response.
            char *sub_token_differentiator;
            char first_line[128];
            strncpy(first_line, token, 127);
            char *subToken = strtok_r(first_line, " ", &sub_token_differentiator); // In the first word usually (HTTP/1.1 something).
            int secondToken = FALSE;
            while(subToken != NULL){
                if(secondToken == FALSE){
                    secondToken = TRUE; 
                }
                else{ // If it is second token. usually our HTTP code lives here.
                    int code;
                    sscanf(subToken, "%d", &code); 
                    rsp -> code = code;
                    break;  // We got what we required. (Go to next line).
                }  
                subToken = strtok_r(NULL, " ", &sub_token_differentiator);
            } 
            firstToken = FALSE;  
        }
        else if(strstr(token, "Date:") != NULL){ // Checking for Date field in header.
            char timestamp[SIZE]; // Not sure if timestamp is more than 50 chars.
            struct tm tm1;
            memset(rsp -> time_stamp, '\0', SIZE);
            strcpy(rsp->time_stamp, token + 6);
            memset(timestamp, '\0', SIZE);
            strcpy(timestamp, token+ 10); // Ignoring, first 10 characters which are Date: Day, .. 
            if(strptime(timestamp, "%d %b %Y %T",&tm1) == '\0') // Day, Month, Year and Time (Imp!) (%d for day, %b for Month,%Y for year, %r for12 hour format, %T for 24 hour format)
                raiseException("\nstrptime failed\n");   
            rsp -> visit_time = tm1;
            break;
        }  
        token = strtok_r(NULL, "\r\n", &whole_token_differentiator);
    }   
    return rsp;
} 

// Driver functions to check parser.c.
// void main(){
//     char url[30];
//     memset(url, '\0', 30);
//     strcpy(url, "https://www.unt.edu");
//     website* site = parse_url(url); 
//     build_not_modified_request(site, "2016 01 04 10 41 23");
//     /*
//     OUTPUT:
//     GET /   HTTP/1.0
//     Host: www.unt.edu
//     User-Agent: *
//     Content-Type: text/html;charset=UTF-8
//     If-Modified-Since:Mon Jan  4 10:41:23 2016

//     */
// }