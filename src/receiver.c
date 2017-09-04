/*
 * File: receiver.c
 *
 * Date:        2017-08-27
 *
 *
 */

#include "receiver.h"
#define oops(s) {perror(s); exit(-1); }



/* Sets up a socket for receiving messages for the program
 * @param arg   host of the program.
 * @returned    void pointer (needed to end the thread)
 */
void *listenUdp(void *server) {

    struct host *thisServer = (struct host*)server;

    struct sockaddr_in addr, from;
    int sock;
    unsigned int fromlen;
    char buf[BUF_SIZE];
    char* endPtr;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        oops("socket");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)strtol(thisServer->port, &endPtr, 10));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0){
        oops("bind");
    }


    /* Server main loop; it here receivs "questions",
     * processes them and sends the answer back */
    while (1) {
        fromlen = sizeof(from);
        if(recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&from, &fromlen) == 0){
            printf("%s", buf);
            //sendto(sock, "ACK\n", 4, 0, (struct sockaddr *)&from, fromlen);
            break;
        }
    }
}

void *sendUdp(void* sendTo){

    struct host *receiver = (struct host*)sendTo;

    struct sockaddr_in addr;
    int sock, status;
    char buf[BUF_SIZE];
    struct addrinfo *res, hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        oops("socket");
    }

    /* Build the network address of this client */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0){
        oops("bind");
    }


    /* Build the network address of server */
    if(getaddrinfo(receiver->name, receiver->port, &hints, &res) < 0){
        oops("getaddrinfo");
    }


    /* Create a message to the server */
    sprintf(buf, "Some data\n");
    /* Send it to the server */
    sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)res->ai_addr, res->ai_addrlen);
    /* Receive the answer */
    memset(buf, 0, BUF_SIZE);
    //recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)res->ai_addr, &res->ai_addrlen);
    /* process the answer */
    //printf(buf);
    /* free getaddrinfo struct */
    free(res);
}
