#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "dbg.h"

#define MAX 255 /* Longest string to echo */

void DieWithError(char *errorMessage); /* External error handling function */

void requestData(char *msg, int sock, struct sockaddr_in servAddress, unsigned int servAddrLen, char *result)
{
    char response[MAX];
    int recvMsgSize;
    check(sendto(sock, msg, strlen(msg), 0,
                 (struct sockaddr *)&servAddress, sizeof(servAddrLen)) == strlen(msg),
          "sent different number of bytes than expected");
    printf("\nsent menu to client\n");

    check(((recvMsgSize = recvfrom(sock, result, MAX, 0,
                                   (struct sockaddr *)&servAddress, &servAddrLen)) >= 0),
          "recvfrom() failed size= %i", recvMsgSize);
    result[recvMsgSize] = '\0';
    strcpy(response, result);

error:
    return;
}

int main(int argc, char *argv[])
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;     /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                    /* IP address of server */
    char *echoString;                /* String to send to echo server */
    char buffer[MAX + 1];            /* Buffer for receiving echoed string */
    int echoStringLen;               /* Length of string to echo */
    int respStringLen;               /* Length of received response */
    char data[MAX];
    int dataLen;

    // if ((argc < 3) || (argc > 4))    /* Test for correct number of arguments */
    // {
    //     fprintf(stderr,"Usage: %s <Server IP> <Port>\n", argv[0]);
    //     exit(1);
    // }
    check(argc == 3, "Usage: %s <Server IP> <Port>\n", argv[0]);

    servIP = argv[1];             /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]); /* Second arg: port number */
    /* Check input length */
    echoStringLen = strlen("1");
    // check(echoStringLen < MAX, "Request too long %i", echoStringLen);

    /* Create a datagram/UDP socket */
    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    check(sock > 0, "socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);      /* Server port */

    //send the initial request
    check((sendto(sock, "1", echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == echoStringLen), "sendto() sent a different number of bytes than expected");

    //recieve the initial response
    fromSize = sizeof(fromAddr);
    respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

    check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
    // printf("Received:\n%s\n", buffer);
    //null terminate
    buffer[respStringLen] = '\0';
    //the first task is is to login
    if (strncmp(buffer, "enter username", 16) == 0)
    {
        //print the server's request
        printf("%s\n", buffer);
        //get the username from client
        scanf("%s", data);
        dataLen = strlen(data);
        printf("sending %s", data);
        //send the username
        check((sendto(sock, data, dataLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == dataLen), "sendto() sent a different number of bytes than expected");
        //get the login confirmation from server
        respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
        check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
        /* null-terminate the received data */
        buffer[respStringLen] = '\0';
        //display
        printf("%s", buffer);
        //clear out the buffer
        memset(buffer, 0, respStringLen);
    }

    while (1)
    {
        /* Recv a response menu */
        fromSize = sizeof(fromAddr);
        respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);

        check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
        printf("Received:\n%s\n", buffer);

        //get request number from user
        scanf("%s", echoString);
        echoStringLen = strlen(echoString);
        //send request number to server
        check((sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == echoStringLen), "sendto() sent a different number of bytes than expected");
        /* Recv a response to the request*/
        fromSize = sizeof(fromAddr);
        respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
        check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
        /* null-terminate the received data */
        buffer[respStringLen] = '\0';
        printf("Received:\n%s\n", buffer);
        //act on response, note that some cases require sending more data to server
        if (strncmp(buffer, "enter username", 16) == 0)
        {
            //print the server's request
            printf("%s", buffer);
            //get the username from client
            scanf("%s", data);
            dataLen = strlen(data);
            printf("sending %s", data);
            //send the username
            check((sendto(sock, data, dataLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == dataLen), "sendto() sent a different number of bytes than expected");
            //get the login confirmation from server
            respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
            check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
            /* null-terminate the received data */
            buffer[respStringLen] = '\0';
            //display
            printf("%s\n", buffer);
            //clear out the buffer
            memset(buffer, 0, respStringLen);
        }
        else if (strncmp(buffer, "quit", 4) == 0)
        {
            //in this case, we can just quit the client
            printf("quitting\n");
            close(sock);
            exit(0);
        }
        else if (strncmp(buffer, "\nLogged in users", 16) == 0)
        {
            //in this case, no furter action is required by the user
            printf("%s", buffer);
        }
        else if (strncmp(buffer, "requesting", 10) == 0)
        {
            //print the server's data request
            printf("%s", buffer);
            //get data to respond to request
            scanf("%s", data);
            dataLen = strlen(data);
            //send the data
            check((sendto(sock, data, dataLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) == dataLen), "sendto() sent a different number of bytes than expected");
            //get response from server
            respStringLen = recvfrom(sock, buffer, MAX, 0, (struct sockaddr *)&fromAddr, &fromSize);
            check((echoServAddr.sin_addr.s_addr == fromAddr.sin_addr.s_addr), "Error: received a packet from unknown source.\n");
            /* null-terminate the received data */
            buffer[respStringLen] = '\0';
            //display
            printf("%s\n", buffer);
            //clear out the buffer
            memset(buffer, 0, respStringLen);
        }
        else if (strncmp(buffer, "start game", 10))
        {
            printf("%s", buffer);
        }
        else
        {
            printf("invalid response recieved\n");
        }
    }

error:
    close(sock);
    exit(0);
}
