/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 04 / 23 / 2018
    Email: karthikVenkataramanaPemmaraju@my.unt.edu
    Description: A client program which interacts with the server.  
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
#include <math.h>
#include <time.h>

void raiseException(char * msg) {
    perror(msg);
    exit(0);
}
void removeSubstring(char *s,const char *toremove) {
  while( s=strstr(s,toremove) )
  memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

char* choose_ip(char *buffer){
    char *ip = malloc(18);
    memset(ip, '\0', 18);
    srand(time(NULL));
    int random_number = rand() % 3 + 1;
    int cur = 0;
    char* token = strtok(buffer, ","); //  First 45 chars are of no use to us.
    while(token != NULL && cur < 3){
        cur++;
        if(cur != random_number ){
            token = strtok(NULL, ",");
        }
        else
            break;
    }
    removeSubstring(token, "DHCP OFFER: yiaddr: ");
    strncpy(ip, token , 17);
    return ip;
}
int main(int argc, char * argv[]) {
    int fd, n;
    struct sockaddr_in server_address;
    struct hostent * server; // For translating domain name to IP address
    char buffer[256], random[3], ip_add[17];
    srand(time(NULL));
    int random_number = rand() %1000;
    memset(random, '\0', 3);
    sprintf(random, "%d", random_number);
    if (argc < 3)
        raiseException("Enter IP and Port Number");
    // Step 1: create the UDP socket.
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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
    memset(buffer, '\0', 256);
    strcat(buffer, "**********************\nDHCP DISCOVER\nyiaddr: 0.0.0.0\ntransaction ID: ");
    strcat(buffer, random);
    int len = sizeof(server_address);
    if ((n = sendto(fd, buffer, 255, 0, (struct sockaddr *) &server_address, len)  )  < 0)
            raiseException("ERROR write to socket.");
    printf("Sent DHCP Discover Message to %s \n*****************************\n", argv[1]);
    memset(buffer, '\0', 256); 
    // STEP 2: RECEVING DHCP OFFER
    if ( (n = recvfrom(fd, buffer, 255, 0, (struct sockaddr *) &server_address, &len ) ) < 0) // Read Expression from client.
            raiseException("ERROR reading from socket");
    printf("%s says:\n%s\n", argv[1], buffer);
    memset(ip_add, '\0', 17);
    strcpy(ip_add, choose_ip(buffer));
    memset(buffer, '\0', 256);
    // 3. DHCP Request
    strcpy(buffer, "DHCP_Request:yiaddr: ");
    strncpy(buffer + 21 ,  ip_add, strlen(ip_add)); 
    memset(random, '\0', 3);
    sprintf(random, "%d", random_number);
    strcat(buffer," \n transaction id: ");
    strcat(buffer, random);
    strcat(buffer, "\nlifetime: 3600 secs\n"); 
    len = sizeof(server_address);
    if ((n = sendto(fd, buffer, 255, 0, (struct sockaddr *) &server_address, len)  )  < 0)
            raiseException("ERROR write to socket");
    printf("Sent following DHCP Request Message to %s \n %s\n*****************************\n", argv[1], buffer);
    //4. DHCP Ack
    memset(buffer, '\0', 256);
    if ( (n = recvfrom(fd, buffer, 255, 0, (struct sockaddr *) &server_address, &len ) ) < 0) // Read Expression from client.
            raiseException("ERROR reading from socket");
    printf("%s says:\n%s\n", argv[1], buffer);

    return 0;
}