The Proxy server performs the following functions.
    1) Can differentiate between HTTP and HTTPS websites (Other protocols are not currently supported).
    2) Stores list of cached webpages in a file (list.txt).
    3) The cached webpages are created in the directory web_pages/
    4) The responses from remote server are processed by proxy server and sent to Client in the directory <Client - IP>/ (Ex: 129.120.151.95/)
    5) It is to be noticed that proxy server maintains atmost 5 webpages in the folder web_pages/ .
    6) Most of the commercial websites do not want to cache their websites.( they do not want to be cached).
    7) Only first and second websites in the sample list(provided below) respond with last modified header info.
    8) The server reads from list.txt for any previously cached webpages.
    9) The file name is stored in YYYYMMDDhhmmss format.

To verify if correct webpage is retrieved, you can open the HTML files in proxy_webpages/ folder with any web browser.

Sample list of HTTPS websites:
https://www.unt.edu
http://www.cse.unt.edu/~tarau/
https://www.google.com
https://www.yahoo.com
https://www.ebay.com 
https://www.facebook.com

Sample list of HTTP websites:
http://go.com/
http://www.bbc.com/
http://abcnews.go.com/ 
http://youth.cn/
http://www.aliexpress.com/

Please unzip all the files and keep them in a single folder.

For Compilation  and Execution:
Server:
gcc server.c connections.c parser.c filehandler.c -o server -lssl
./server <Port> (Any ephermal port number) (Ex: ./server 2222)

Client:
gcc client.c connections.c filehandler.c parser.c -o client -lssl
./client <server name> <server Port> (Ex: ./client cse01.cse.unt.edu 2222)
 
Reach me out at karthikVenkataramanaPemmaraju@my.unt.edu for details.

Thanks for grading!

/*
Example output:
Server:
kp0327@cse01:~/CN/Proxy Server$ gcc server.c connections.c parser.c filehandler.c -o server -lssl
kp0327@cse01:~/CN/Proxy Server$ ./server 1111

Client: cse02.cse.unt.edu requested url: https://www.unt.edu
***********************************
Contacting https://www.unt.edu ...
Response Code: 200
Succesfully received webpage from https://www.unt.edu
Saved Webpage at: proxy_webpages/20180318211748.html

Contacting client....
Sending file: proxy_webpages/20180318211748.html
Sent file successfully!
-------------------------------------------------------------------------------


Client: cse02.cse.unt.edu requested url: https://www.unt.edu
***********************************
File already exists in our cache! Checking for modifications....
Contacting https://www.unt.edu ...
Response Code: 304
https://www.unt.edu is not modified, retrieving ...

Contacting client....
Sending file: proxy_webpages/20180318211748.html
Sent file successfully!

Client:
kp0327@cse02:~/CN/Proxy Server$ gcc client.c connections.c filehandler.c parser.    c -o client -lssl
kp0327@cse02:~/CN/Proxy Serv./client cse01.cse.unt.edu 1111   
url: https://www.unt.edu
Received webpage at: 129.120.151.95/20180318211748.html
kp0327@cse02:~/CN/Proxy Server$ ./client cse01.cse.unt.edu 1111
url: https://www.unt.edu
Received webpage at: 129.120.151.95/20180318211748.html

*/
