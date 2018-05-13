/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 12 / 2018 
*/

#pragma once
#include <time.h> 
#include "filehandler.h"

#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 4092
#define SIZE 512

typedef struct website{
    char base_url[SIZE];
    char port[SIZE];
    char relative_url[SIZE];
    char extension[SIZE];
}website;
 
typedef struct response{
    int code;
    struct tm visit_time;   // Time of visit.
    char time_stamp[SIZE];  // Time of visit in String format.
}response;
/*
    @Desc: Parses the URL which the client enters and builds our data structure (website).
*/
website* parse_url(char *buffer);
/*
    @Desc: Builds a Request.
*/
char* build_get_request(website * site);  
char* build_not_modified_request(website * site, list* list);
/*
    @Desc: Parses the response from GET Request and builds data structure (response).
*/
response* parse_response(char *buffer);