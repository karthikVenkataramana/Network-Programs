#ifndef ASSIGNEDIP
#define ASSIGNEDIP
typedef struct assigned_ip{
    int cur_index;
    char client_ip[150][17];    // At most 150 clients with an Ip of atmost 16 Characters.
}assigned_ip;

typedef struct current_ip{
    int cur_index;
    char client_ip[3][17];
}current_ip;
#endif