#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg.h"

#define MAX 255
#define MAX_USERS 20
//list of loggedIn users
char loggedInUsers[MAX_USERS][MAX];
int loggedInCount = 0;

char* printMenu(){
    char *menu;
    menu = "Enter option:\n\tLogin[1]/Logout[1]\n\tList players[2]\n\tSend request[3]\n\tAccept request[4]\n\tExit[5]\n\0";
    return menu;
}

//use when doing a request/reply 
void requestData(char *msg, int sock, struct sockaddr_in clntAddress, unsigned int cliAddrLen, char *result)
{
    int recvMsgSize;
    check(sendto(sock, msg, strlen(msg), 0,
           (struct sockaddr *)&clntAddress, sizeof(clntAddress))==strlen(msg), "sent different number of bytes than expected");
    printf("\nsent menu to client\n");
    
    check(((recvMsgSize = recvfrom(sock, result, MAX, 0,
                                                    (struct sockaddr *)&clntAddress, &cliAddrLen)) >= 0),
                           "recvfrom() failed size= %i", recvMsgSize);
    
    error:
        return;
}

// void requestData(char *msg, char* reply, int sock, struct sockaddr_in clntAddress, unsigned int cliAddrLen, char *result)
// {
//     int recvMsgSize;
//     check(sendto(sock, msg, strlen(msg), 0,
//                  (struct sockaddr *)&clntAddress, sizeof(clntAddress)) == strlen(msg),
//           "sent different number of bytes than expected");
//     printf("\nsent menu to client\n");

//     check(((recvMsgSize = recvfrom(sock, result, MAX, 0,
//                                    (struct sockaddr *)&clntAddress, &cliAddrLen)) >= 0),
//           "recvfrom() failed size= %i", recvMsgSize);
//     check(sendto(sock, reply, strlen(msg), 0,
//                  (struct sockaddr *)&clntAddress, sizeof(clntAddress)) == strlen(msg),
//           "sent different number of bytes than expected");

// error:
//     return;
// }

//use when just sending 
void sendData(char *msg, int sock, struct sockaddr_in clntAddress)
{
    int recvMsgSize;
    check(sendto(sock, msg, strlen(msg), 0,
                 (struct sockaddr *)&clntAddress, sizeof(clntAddress)) == strlen(msg),
          "sent different number of bytes than expected");

error:
    return;
}

void handleRequest(int request, int sock, struct sockaddr_in clntAddress, unsigned int cliAddrLen)
{
    char data[MAX];
    printf("requested %i\n", request);
    check(((request > 0) && (request < 6)), "enter value between 1 and 5");
    switch (request)
    {
    case 1:
        printf("logging in user");
        //send login request
        int recvMsgSize;
        requestData("enter username\n\0", sock, clntAddress, cliAddrLen, data);
        printf("\nrecieved %s of length %lu\n", data, strlen(data));
        memcpy(&loggedInUsers[loggedInCount], data, strlen(data));
        printf("added user %s", data);
        check(strcmp(loggedInUsers[loggedInCount], data) == 0, "copy did not work");
        loggedInCount += 1;
        printf("%i users logged in\n", loggedInCount);
        sendData("logged in\n\0", sock, clntAddress);
        break;
    case 2:
        printf("listing players\n");
        int i;
        char sendList[1000];
        strcat(sendList ,"\nLogged in users:\n");
        for(i = 0; i < loggedInCount; i++){
            strcat(sendList, loggedInUsers[i]);
            printf("\t%i: %s\n", i, loggedInUsers[i]);
        }
        strcat(sendList, "\n\0");
        sendData(sendList, sock,clntAddress);
        break;
    case 3:
        printf("sending request\n");
        requestData("enter username to request\n\0", sock, clntAddress, cliAddrLen, data);
        sendData("requesting\n\0",sock, clntAddress);
        break;
    case 4:
        printf("accepting request\n");
        // fork();
        // execl("./TicTacToe", "", (char *)NULL);
        sendData("start game\0", sock, clntAddress);
        break;
    case 5:
        printf("quitting\n");
        sendData("quit\0", sock, clntAddress);
        return;
    }
    error:
        return;
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[MAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    int request;
    char* data;

    // int i;
    // //allocate space for storing logged in users
    // for(i = 0; i < 200; i++){
    //     loggedInUsers[i] = malloc(char;
    // }

    if (argc != 2) /* Test for correct number of parameters */
    {
        fprintf(stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]); /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    check( ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0),
        "socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    check( (bind(sock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) >= 0), "bind() failed");

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        //initial client request
        check (((recvMsgSize = recvfrom(sock, echoBuffer, MAX, 0,
                                    (struct sockaddr *)&echoClntAddr, &cliAddrLen)) >= 0),
            "recvfrom() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        //get request number
        request = atoi(echoBuffer);
        //handle the request
        handleRequest(request, sock, echoClntAddr, cliAddrLen);
        //send the menu to client after completing request
        check((sendto(sock, printMenu(), strlen(printMenu()), 0,
                   (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) == strlen(printMenu())),
            "sendto() sent a different number of bytes than expected");
    }
    error:
    return 1;
    quit:
    return 0;
}