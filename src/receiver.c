
#include <bits/time.h>
#include "receiver.h"
#define error(s) {perror(s); exit(-1); }

pthread_mutex_t lock;

/* Sets up a socket for receiving messages for the program
 * @param server, host struct with the nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *listenUdp(void *server) {
    pthread_mutex_lock(&lock);
    struct ringNode *thisServer = (struct ringNode*)server;

    struct sockaddr_in addr, from;
    int sock;
    unsigned int fromlen;
    char* buf;
    char* msId;
    char* endPtr;

    buf = calloc(BUF_SIZE, sizeof(char));
    msId = calloc(BUF_SIZE, sizeof(char));

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

        memset(msId, 0, BUF_SIZE);

        printf("\nNode %s is waiting for message", thisServer->nodeId);
        recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&from, &fromlen);

        getIdFromMessage(buf, msId);
        printf("\nNode %s recieved datagram from %s", thisServer->nodeId, msId);
        sendto(sock, "ACK\n", 4, 0, (struct sockaddr *)&from, fromlen);


        //if new messege has higher id: forward the messege
        if(strcmp(thisServer->highId, msId) < 0){
            printf("\nNode %s is forwarding a message with %s as id", thisServer->nodeId, msId);
            memset(thisServer->highId, 0, BUF_SIZE);
            strcpy(thisServer->highId, msId);
            sendUdp(thisServer);
            //forward message

            //if new message has lower id: discard the messege, become nonparticipant
        }else if(strcmp(thisServer->highId, msId) > 0){
            printf("\nNode %s is shutting down", thisServer->nodeId);
            thisServer->isParticipant = 0;
            break;
            //if it is the original messege: become master
        }else if(strcmp(thisServer->highId, msId) == 0){
            printf("\nNode %s is becoming master",thisServer->nodeId);
            thisServer->isMaster = 1;
            thisServer->isParticipant = 0;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
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
    char* recvbuf;
    char* sendbuf;
    struct addrinfo *res, hints;

    recvbuf = calloc(BUF_SIZE, sizeof(char));
    sendbuf = calloc(BUF_SIZE, sizeof(char));

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
                printf("\nNode: %s send success", thisServer->nodeId);
                break;
            }
        }
    }

    free(sendbuf);
    free(recvbuf);
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


