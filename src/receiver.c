#include <bits/time.h>
#include "receiver.h"
#define error(s) {perror(s); exit(-1); }



/* Sets up a socket for receiving messages for the program
 * @param server, host struct with the nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *listenUdp(void *server) {

    struct ringNode *thisServer = (struct ringNode*)server;

    struct sockaddr_in addr, from;
    int sock;
    unsigned int fromlen;
    char buf[BUF_SIZE];
    char msId[BUF_SIZE];
    char* endPtr;

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        error("socket");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)strtol(thisServer->this->port, &endPtr, 10));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0){
        error("bind");
    }

    while(thisServer->isParticipant){
        fromlen = sizeof(from);
        memset(buf, 0, BUF_SIZE);
        memset(msId, 0, BUF_SIZE);

        recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&from, &fromlen);
        printf("%s", buf);
        sendto(sock, "ACK\n", 4, 0, (struct sockaddr *)&from, fromlen);
        getIdFromMessage(buf, msId);

        //if new messege has higher id: forward the messege
        if(strcmp(thisServer->highId, msId) < 0){
            memset(thisServer->highId, 0, BUF_SIZE);
            strcpy(thisServer->highId, msId);
            sendUdp(thisServer);
            //if new messege has lower id: discard the messege, become nonparticipant
        }else if(strcmp(thisServer->highId, msId) > 0){
            thisServer->isParticipant = 0;
            //if it is the original messege: become master
        }else{
            thisServer->isMaster = 1;
            thisServer->isParticipant = 0;
        }
    }

    //compare = compareElectionMessage(thisServer->nodeId, buf);

}

/* Sets up a socket for receiving messages for the program
 * @param sendTo, host struct with the receiving nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *sendUdp(void* server){

    struct ringNode *thisServer = (struct ringNode*)server;

    struct sockaddr_in addr;
    int sock;
    char recvbuf[BUF_SIZE];
    char sendbuf[BUF_SIZE];
    struct addrinfo *res, hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        error("socket");
    }

    /* Build the network address of this client */
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0){
        error("bind");
    }


    /* Build the network address of server */
    if(getaddrinfo(thisServer->next->name, thisServer->next->port, &hints, &res) < 0){
        error("getaddrinfo");
    }
    thisServer->highId = calloc(strlen(thisServer->nodeId), sizeof(char));
    strcpy(thisServer->highId, thisServer->nodeId);
    /* Create a message to the server */
    if(thisServer->isParticipant){
        createElectionMessage(thisServer, sendbuf);

        struct timeval read_timeout;
        read_timeout.tv_sec = 0;
        read_timeout.tv_usec = 10;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);

        while(1){

            sendto(sock, sendbuf, strlen(sendbuf), 0, res->ai_addr, res->ai_addrlen);

            /* Receive the answer */
            memset(recvbuf, 0, BUF_SIZE);
            recvfrom(sock, recvbuf, BUF_SIZE, 0, res->ai_addr, &res->ai_addrlen);
            if(strlen(recvbuf) > 0){
                break;
            }
        }
    }


    /* Send it to the server */



    /* free getaddrinfo struct */
    free(res);
}

void createElectionMessage(ringNode* node, char* buf){

    memset(buf, 0, BUF_SIZE);
    strcpy(buf, "ELECTION\n");
    strcat(buf, node->nodeId);
    strcat(buf, "\n");

}

void getIdFromMessage(const char* message, char* id){
    int i = 0;
    int j = 0;
    while(message[i] != '\n'){
        id[i] = message[i];
        i++;
    }
    i++;
    while(message[i] != '\n'){
        id[j] = message[i];
        i++;
        j++;
    }
}


