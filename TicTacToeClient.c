#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "dbg.h"

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage);  /* External error handling function */

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[ECHOMAX+1];      /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    
    // if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    // {
    //     fprintf(stderr,"Usage: %s <Server IP> <Port>\n", argv[0]);
    //     exit(1);
    // }
    check(argc == 3, "Usage: %s <Server IP> <Port>\n", argv[0]);

        servIP = argv[1];         /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]); /* Second arg: port number */
    /* Check input length */
    echoStringLen = strlen("1");
    // check(echoStringLen < ECHOMAX, "Request too long %i", echoStringLen);
    
    /* Create a datagram/UDP socket */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    check (sock > 0,"socket() failed");
    
    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

    //send the initial request
    check((sendto(sock, "1", echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == echoStringLen), "sendto() sent a different number of bytes than expected");
    /* Recv a response menu */
    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                             (struct sockaddr *)&fromAddr, &fromSize);

    check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
    printf("Received:\n%s\n", echoBuffer);

    //request/response cycle
    for(;;){
        //get request number from user
        scanf("%s", echoString);
        echoStringLen = strlen(echoString);
        //send request number to server
        check((sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == echoStringLen), "sendto() sent a different number of bytes than expected");
        /* Recv a response */
        fromSize = sizeof(fromAddr);
        respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,(struct sockaddr *) &fromAddr, &fromSize);
        check ((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr) ,"Error: received a packet from unknown source.\n");
                    /* null-terminate the received data */
            echoBuffer[respStringLen] = '\0';
            printf("Received:\n%s\n", echoBuffer);
        //act on response
        if(strncmp(echoBuffer, "quit", 4)==0){
            printf("quitting\n");
            close(sock);
            exit(0);
        }
        else if (strncmp(echoBuffer, "Logged in users", 15))
        {
            printf("%s", echoBuffer);
            memset(echoBuffer, 0, strlen(echoBuffer));
        }
        else if (strncmp(echoBuffer, "logged in",9 )){
            printf("%s", echoBuffer);
            memset(echoBuffer, 0, strlen(echoBuffer));
        }
        else if (strncmp(echoBuffer, "requesting",10)){
            printf("%s", echoBuffer);
            memset(echoBuffer, 0, strlen(echoBuffer));
        }
        else if (strncmp(echoBuffer, "start game",10)){
            printf("%s", echoBuffer);
            memset(echoBuffer, 0, strlen(echoBuffer));
        }
        else{
            printf("invalid response recieved\n");
            memset(echoBuffer, 0, strlen(echoBuffer));
        }

        //restart cycle by recieving menu again
        respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0,
                                     (struct sockaddr *)&fromAddr, &fromSize);

        check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
        printf("Received:\n%s\n", echoBuffer);
    }
error:
    close(sock);
    exit(0);
}
