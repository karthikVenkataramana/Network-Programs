/*
    Author: Karthik Venkataramana Pemmaraju
    Date: 04/22/2018
    Description: Calculates the range of Ip addresses. 
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <math.h>
#include "assigned_ip.h"

#define INVALID_MASK 100
void raiseException(char * msg) {
    perror(msg);
    exit(0);
}
int is_present(char *ip, assigned_ip* asp){
    int i =0; 
    for(i = 0; i < asp->cur_index; i++){
        if( strcmp(asp->client_ip[i], ip) == 0)
            return 1;
    }
    return -1;
}
void get_ip_address(char* gateway, char* subnet_mask, assigned_ip* asp, current_ip* csp){
    int mask_bits = number_of_mask_bits(subnet_mask);
    char temp_gateway[20];
    memset(temp_gateway ,'\0', 20);
    strncpy(temp_gateway, gateway, strlen(gateway));
    if(mask_bits == 100){ 
        raiseException("Invalid subnet mask \n");
    }
    int cur, t; 
    char buf[4];
    int max = pow(2,  32 - mask_bits); 
    char *p = strrchr(temp_gateway, '.');
    char temp[17];
    strncpy(temp, temp_gateway, 17);
    if (p && *(p + 1))
        cur = atoi(p + 1);  // Gets the transaction number.
    cur++;
    csp -> cur_index = 0; 
    while(cur < max && csp -> cur_index < 3){
        memset(temp + (strlen(temp_gateway) - strlen(p+1)) , '\0',  strlen(p+1) );
        memset(buf, '\0', 4);
        t = cur;
        sprintf(buf, "%d", cur);    // cur has the last number.
        cur = t;
        memset(csp->client_ip[csp->cur_index], '\0', 17);
        strcat(temp, buf);
        if(is_present(temp,asp) == -1){ // If it is not already present, then only add.
            strncpy(csp -> client_ip[csp -> cur_index], temp, 16); 
            csp -> cur_index = csp->cur_index + 1;
        } 
        cur++;
    }
} 

int number_of_mask_bits(char* subnet_mask){
    char octet[3]; 
    char temp[20];
    int index = 0; 
    int octet_index = 0; 
    int sum = 0;
    memset(temp, '\0', 20);
    strncpy(temp, subnet_mask, strlen(subnet_mask));
    strcat(temp, ".");
    int size = strlen(temp);
    while(index < size){
        char c = temp[index];
        if(c != '.'){
            if(!isdigit(c))
                return INVALID_MASK;
            else
                octet[octet_index] = c;
            octet_index++;
        }
        else{ 
            int number_of_set_bits = helper(atoi(octet));
            if(number_of_set_bits >= INVALID_MASK){ 
                return INVALID_MASK;
            }
                
            else
                sum += number_of_set_bits;
            memset(octet, '\0', 3);
            octet_index = 0;
        }
        index++;
    }
    if(sum >= 30){
        printf("Not enough IP addresses to support! :(\n");
        return INVALID_MASK;
    }
    return sum;
}

long binary(int n){
    if(n ==0)
        return 0;
    else
        return (n % 2 + 10 * binary(n/2));
}

int helper(int n){
    long binary_number = binary(n);
    char bin[8]; 
    char num_digits[8];
    int index = 0;  
    int sum = 0;
    int NOTFIRST = 0;  
    memset(num_digits, '\0', 8);
    sprintf(num_digits, "%ld", binary_number);
    memset(bin, '0', 8);// Prepend zeroes to the string.
    sprintf(bin + (8 - strlen(num_digits) ) , "%ld", binary_number);
    for(index = 0; index < strlen(bin); index++){
        if(bin[index] == '1'){ 
            if(NOTFIRST == 1)
                return 100;
            sum++;
        }
        else{
            NOTFIRST  = 1;
        }
    }
    return sum;
}
// void main(){
//     char subnet_mask[17] = "255.255.255.0";
//     char gateway[17] = "192.168.12.1";
//     // current_ip* csp = (current_ip*)malloc(sizeof(current_ip));
//     // printf("MAIN: %ld\n" , helper(247) ); 
  
//     get_ip_address(gateway, subnet_mask, NULL, csp);
// }