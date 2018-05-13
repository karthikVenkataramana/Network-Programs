/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 14 / 2018 
*/
#pragma once 
 #define MAX_LIST_SIZE 5
#define SIZE 512

typedef struct list{
    char url[SIZE];  // URL of the site
    char filename[SIZE]; // file Name generated.
    char time_stamp[SIZE];  // Each list item has a time_stamp
    int index;  // Index in the list.txt.
}list;

typedef struct maplist{
    list mapping_list[MAX_LIST_SIZE];   // At most 5 webpages in our proxy.
    int current_index;
    int isValid;    // Valid for first 5 webpages since list is null for those pages.
}maplist;


void add_mapping(char *url, char *filename , maplist *map, char* time_stamp);
void reset_index(maplist *map);
void write_to_file(maplist *map); 
void recv_file(int serverfd);
void send_file(char* filename, int clientfd, char *client_name, int type);
list* get_list(char *url, maplist *map);
void print_map_list(maplist* map);
char *get_work_dir();
maplist* read_from_file();
void initialize(maplist * map);