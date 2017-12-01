#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "dbg.h"

#define ECHOMAX 255

char * printMenu(){
    char *menu;
    menu = "Enter option:\n\tLogin[1]/Logout[1]\n\tList players[2]\n\tJoin game[3]\n\tAccept request[4]\n\tExit[5]\n\0";
    return menu;
}

void handleRequest(int request){
    error:
    printMenu();
    // scanf("%i", &request);
    printf("requested %i\n", request);
    check(((request > 0) && (request < 6)), "enter value between 1 and 5");
    switch (request)
    {
    case 1:
        printf("logging in\n");
        //send login request

        break;
    case 2:
        printf("listing players\n");
        break;
    case 3:
        printf("joining game\n");
        break;
    case 4:
        printf("accepting request\n");
        break;
    case 5:
        printf("quitting\n");
        return;
    }
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */
    int request;

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
        check (((recvMsgSize = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                                    (struct sockaddr *)&echoClntAddr, &cliAddrLen)) >= 0),
            "recvfrom() failed");

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        request = atoi(echoBuffer);
        if(request == 5){
            goto quit;
        }
        handleRequest(request);
        /* Send received datagram back to the client */
        check((sendto(sock, printMenu(), strlen(printMenu()), 0,
                   (struct sockaddr *)&echoClntAddr, sizeof(echoClntAddr)) == strlen(printMenu())),
            "sendto() sent a different number of bytes than expected");
    }
    error:
    return 1;
    quit:
    return 0;
}