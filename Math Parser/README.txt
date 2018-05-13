The Math parser is most comprehensive and almost as powerful as those on the internet.

The Math expression parser has the following capabilities.
 1) Recognize and ignore spaces.
 2) Handle multiple and complex expressions.
 3) Currently supports:  + ,- ,*, /, log , sqrt and exponent 
 4) Handle floating numbers as well.
 5) If the expression has errors, the parser let's user know where the error might be.

Please download the zip file and place all the files in a single folder.

Compilation:

Server:  gcc server.c stack.c evaluate.c -lm -o server 
Client: gcc client.c -o client

Execution
Server: ./server 3333 (Any port number)
Client: ./client (server domain address) 3333 
For example, if you run server on cse01.cse.unt.edu, Then use: ./client cse01.cse.unt.edu 3333

 
Example Outputs:


Client:
/*
    kp0327@cse02:~/CN/Math Server$ gcc client.c -o client
    kp0327@cse02:~/CN/Math Server$ ./client cse01.cse.unt.edu 3333
    EXPRESSION: 1 + (2 *3) + (3 ^ 5) + (sqrt(37.90) *44)
    RESULT: 520.877075
    EXPRESSION: log(45) * (e ^ e) + (3 ^ 5) + (sqrt(37.90) *44)
    RESULT: 538.930298
    EXPRESSION: 1 + 5 * 3 + 2
    RESULT: 18.000000
    EXPRESSION: sqrt(2) + log(-2)
    Negative number to LOG!
    Try Again!
    EXPRESSION: (5*6) + (5+)
    Has Operators at wrong places
    Try Again!
    EXPRESSION: sqrt(2) + log(2)
    RESULT: 1.715244
    EXPRESSION: ((((( sqrt(2))))))
    RESULT: 1.414214
    EXPRESSION: (14.5  * ((23.22 * 7.598923) / 66.67) ) / (e ^(-2))
    RESULT: 283.557220
    EXPRESSION: ())e ^ 2
    Imbalanced paranthesis!
    Try Again!
    EXPRESSION: ()e ^ (-2)
    RESULT: 0.135335
    EXPRESSION: e ^ (3)
    RESULT: 20.085535
    EXPRESSION: (e^2) + (10 - 5)
    RESULT: 12.389055
    EXPRESSION: quit
    bye bye!
    kp0327@cse02:~/CN/Math Server$

*/


SERVER:

/*
    kp0327@cse01:~/CN/Math Server$ gcc server.c stack.c evaluate.c -lm -o server
    kp0327@cse01:~/CN/Math Server$ ./server 3333
    Just served expression: 1+(2*3)+(3^5)+(sqrt(37.90)*44) For Client: cse02.cse.unt.edu
    Just served expression: log(45)*(e^e)+(3^5)+(sqrt(37.90)*44) For Client: cse02.cse.unt.edu
    Just served expression: 1+5*3+2 For Client: cse02.cse.unt.edu
    Just served expression: sqrt(2)+log(-2) For Client: cse02.cse.unt.edu
    Just served expression: (5*6)+(5+) For Client: cse02.cse.unt.edu
    Just served expression: sqrt(2)+log(2) For Client: cse02.cse.unt.edu
    Just served expression: (((((sqrt(2)))))) For Client: cse02.cse.unt.edu
    Just served expression: (14.5*((23.22*7.598923)/66.67))/(e^(-2)) For Client: cse02.cse.unt.edu
    Just served expression: ())e^2 For Client: cse02.cse.unt.edu
    Just served expression: ()e^(-2) For Client: cse02.cse.unt.edu
    Just served expression: e^(3) For Client: cse02.cse.unt.edu
    Just served expression: (e^2)+(10-5) For Client: cse02.cse.unt.edu
    Client cse02.cse.unt.edu closed the connection.

*/

Reach me out at karthikVenkataramanaPemmaraju@my.unt.edu for details.

Thanks for grading!
