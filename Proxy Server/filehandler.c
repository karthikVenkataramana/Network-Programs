/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 03 / 14 / 2018 
*/
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <errno.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#include "filehandler.h"
#include "parser.h"
#include "connections.h"
/*
    @Desc: Creates a new entry in our map with given url, filename and time_stamp.
*/
void add_mapping(char *url, char *filename, maplist* map, char * time_stamp){
    list *list = get_list(url,map);  
    reset_index(map);
    if(list == NULL && map -> isValid == 1){ // If mapping does not exist, go ahead and add url to the list.
        initialize(map);
        strncpy(map -> mapping_list[map -> current_index].url, url, strlen(url));
        strncpy(map -> mapping_list[map ->current_index].filename, filename, strlen(filename)); 
        strncpy(map -> mapping_list[map -> current_index].time_stamp, time_stamp, strlen(time_stamp)); // Now list DS has time stamp as well.
        map -> mapping_list[map -> current_index].index = map -> current_index;
        map -> current_index = map -> current_index + 1;
  }
    else{ 
        char filePath[SIZE] = "proxy_webpages/"; 
        int index;
        if(list == NULL){ // Need to override different website with different file name
            index = map -> current_index;
            strcat(filePath, map -> mapping_list[index].filename); // Pointing to current index
        }
        else{// override exisitng website with new file name
            strcat(filePath, list -> filename); 
            index = list -> index; // Update the old index values (URL and index would be same).
        }
        if(remove(filePath) == 0)
            printf("Old file at %s deleted\n", filePath);
        else   
            printf("CANNOT DELTED %s\n", filePath);
             
        strcpy(map -> mapping_list[index].filename, filename); // file name is current file name.
        strcpy(map -> mapping_list[index].time_stamp, time_stamp); // Now list DS has time stamp as well.        
    }
    write_to_file(map);
}
/*
    @Desc: Returns the list, if the URL exists in our MAP.
*/
list* get_list(char *url, maplist* map){
    int i = 0;  
    while(i <  MAX_LIST_SIZE){ 
            if(strcmp(map -> mapping_list[i].url, url) == 0){ 
                return &(map -> mapping_list[i]);
            } 
        i++;
    }
    return NULL;
}
// Writes the map structure onto file.
void write_to_file(maplist *map){
    FILE *fp = fopen("list.txt", "w+");
    int i;      
    for(i =0; i < MAX_LIST_SIZE ; i++){  
            if(strcmp(map -> mapping_list[i]. url, "") >0  && strcmp(map -> mapping_list[i].filename, "")>0){
                fputs(map -> mapping_list[i].url, fp);
                fputs("\t",fp);
                fputs(map -> mapping_list[i].filename, fp);
                fputs("\t",fp);
                fputs(map -> mapping_list[i].time_stamp, fp);
                fputs("\n", fp); 
            }
    } 
    fclose(fp);
}
/*
    @Desc: Builds our Map DS from file.
*/
maplist* read_from_file(){
    FILE *fp = fopen("list.txt", "r+");
    char *buffer = NULL;
    if(fp == NULL)  return NULL;
    size_t len = 0;
    ssize_t read;
    maplist* map = (maplist*) malloc(sizeof(maplist));
    map -> current_index = 0;
    map -> isValid = 1; 
    int firstToken = TRUE;
    int secondToken = FALSE;
    int thirdToken = FALSE;

    while ((read = getline(&buffer, &len, fp)) != -1) { 
        int index = map -> current_index;
        initialize(map);
        char*  token = strtok(buffer, "\t");
        while(token != NULL){
              if(firstToken == TRUE){
                strcpy(map -> mapping_list[index].url, token);  // First is URL.
                firstToken = FALSE;
                secondToken = TRUE;
                token = strtok(NULL, "\t");
              }
              else if(secondToken == TRUE){
                strcpy(map -> mapping_list[index].filename, token); // Second is FileName (usually .html).
                secondToken= FALSE;
                thirdToken = TRUE;
                token = strtok(NULL, "\t");
              }
              else if(thirdToken == TRUE){
                 strcpy(map -> mapping_list[index].time_stamp, token); // Third is Time Stamp.
                 map->mapping_list[index].index = index;
                break;
              }
          }
        map -> current_index ++;
    }
    fclose(fp);
    return map;
}
void reset_index(maplist * map){
    if (map -> current_index >= MAX_LIST_SIZE){
        map -> current_index = 0;
        map ->isValid = 0;
    }
}
/*
    @Desc: Sends the filename to clientfd.
*/
void send_file(char* filename, int clientfd, char * client_name, int type){
    struct stat file_stat;
    char file_size[SIZE]; 
    char file_name[BUFSIZ]; 
    strncpy(file_name, filename + strlen("proxy_webpages/"), strlen(filename) - strlen("proxy_webpages/") );
    printf("\nContacting client....\nSending file: %s \n" , filename);
    int fd = open(filename, O_RDONLY);
        if (fd == -1)
        {
                fprintf(stderr, "Error opening file --> %s", strerror(errno));
                exit(EXIT_FAILURE);
        } 
        int len = send(clientfd, file_name, BUFSIZ, 0); // Step 1 . Send file name
        if (len < 0)
        {
              fprintf(stderr, "Error on sending file name --> %s", strerror(errno));
              exit(EXIT_FAILURE);
        }
        off_t offset = 0; 
        int sent_bytes = 0;
        /* Sending file data */
        while (( sent_bytes = sendfile(clientfd, fd, &offset, BUFSIZ)) > 0) ;  // Step 2. send data
        printf("Sent file successfully!\n"); 
        close(fd);
}

void recv_file(int serverfd){   // user function to receive file.
    char buffer[BUFSIZ];
    char filename[BUFSIZ];
    int file_size,len; 
    int remain_data = 0;
    /* Receiving file size */ 
    char directory_name[BUFFER_SIZE];
    strncpy(directory_name, get_my_ip_address(serverfd), strlen(get_my_ip_address(serverfd)));
    memset(filename, '\0', BUFSIZ); // Empty the buffer.
    if ((n = read(serverfd, filename ,  BUFSIZ)) < 0) { //  Step 1. receive the file name from the server.
        raiseException("Error reading URL from client \n");
    }    
    strcat(directory_name, "/");
    strcat(directory_name, filename ); 
    FILE *received_file = fopen(directory_name, "w+");  
    if (received_file == NULL) {
                fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));
                exit(EXIT_FAILURE);
     } 
    while (((len = recv(serverfd, buffer, BUFSIZ, 0)) > 0) ) {
            fwrite(buffer, sizeof(char), len, received_file);
            memset(buffer, '\0', BUFSIZ); 
    }
    printf("Received webpage at: %s \n", directory_name);
    fclose(received_file);
} 
/*
    @Desc: Gets the current working directory.
*/
char* get_work_dir(){
    char cwd[BUFFER_SIZE], *ret;
    ret = (char*) malloc(BUFFER_SIZE);
    memset(ret, '\0', BUFFER_SIZE);
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       strncpy(ret, cwd, strlen(cwd));
   }
   else
       perror("getcwd() error");
    return ret;
}
void initialize(maplist * map){
    memset(map -> mapping_list[map -> current_index].filename , '\0', SIZE );
    memset(map -> mapping_list[map -> current_index].time_stamp , '\0', SIZE ); 
    memset(map -> mapping_list[map -> current_index].url , '\0', SIZE );  
}

// void main(){
//     char *url = "https://www.google.com";
//     printf("MAP EXISTS? %d \n", mapping_exists(url) );
//     printf("%d - CI %d \n", add_mapping(url, fileName), current_index);
//     printf("MAP EXISTS? %d \n", mapping_exists(url) );
//     printf("%d - CI %d \n", add_mapping(url, fileName), current_index);
//     printf("MAP EXISTS? %d \n", mapping_exists(url) );
//     list *m;
// }