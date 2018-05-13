The following server emulates the DHCP protocol using UDP.
Multiple clients can connect to server. Client can randomly pick one of the three offered IP addresses.

Server compilation:
gcc server.c -lm -o server 
Execution: ./server 2222

Client compilation:
gcc client.c -o client
Execution: ./client cse01.cse.unt.edu 2222

OUTPUT:
Client:
kp0327@cse03:~/CN/DHCP$ ./client cse01.cse.unt.edu 2222
Sent DHCP Discover Message to cse01.cse.unt.edu
*****************************
cse01.cse.unt.edu says:
DHCP OFFER: yiaddr: 192.168.2.4,192.168.2.5,192.168.2.6,
transaction ID: 38
lifetime: 3600 secs.
****************************

Sent following DHCP Request Message to cse01.cse.unt.edu
 DHCP_Request:yiaddr: 192.168.2.5
 transaction id: 38
lifetime: 3600 secs

*****************************
cse01.cse.unt.edu says:
*****************
yiaddr: 192.168.2.5
transaction ID: 39
lifetime: 3600 secs
*****************

Server: 

kp0327@cse01:~/CN/DHCP$ gcc server.c -lm -o server
kp0327@cse01:~/CN/DHCP$ ./server 2222
gateway: 192.168.2.1
subnet_mask: 255.255.255.0
cse02.cse.unt.edu says:
**********************
DHCP DISCOVER
yiaddr: 0.0.0.0
transaction ID: 455
******************************
SENT DHCP OFFER
 DHCP OFFER: yiaddr: 192.168.2.2,192.168.2.3,192.168.2.4,
transaction ID: 455
lifetime: 3600 secs.
****************************
 cse02.cse.unt.edu says:
DHCP_Request:yiaddr: 192.168.2.2
 transaction id: 455
lifetime: 3600 secs

********************
SENT DHCP ACKNOWLEDGMENT
 *****************
yiaddr: 192.168.2.2
transaction ID: 456
lifetime: 3600 secs
*****************
 Done serving client cse02.cse.unt.edu
################################
