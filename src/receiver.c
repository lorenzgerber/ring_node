

#include "receiver.h"
#define error(s) {perror(s); exit(-1); }



/* Sets up a socket for receiving messages for the program
 * @param server, host struct with the nodes information
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
        error("socket");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)strtol(thisServer->port, &endPtr, 10));
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0){
        error("bind");
    }


    fromlen = sizeof(from);

    recvfrom(sock, buf, BUF_SIZE, 0, (struct sockaddr *)&from, &fromlen);
    printf("%s", buf);


}

/* Sets up a socket for receiving messages for the program
 * @param sendTo, host struct with the receiving nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *sendUdp(void* sendTo){

    struct host *receiver = (struct host*)sendTo;
    struct sockaddr_in addr;
    int sock;
    char buf[BUF_SIZE];
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
    if(getaddrinfo(receiver->name, receiver->port, &hints, &res) < 0){
        error("getaddrinfo");
    }

    /* Create a message to the server */
    sprintf(buf, "Some data\n");
    /* Send it to the server */
    sendto(sock, buf, strlen(buf), 0, res->ai_addr, res->ai_addrlen);
    /* Receive the answer */
    memset(buf, 0, BUF_SIZE);

    /* free getaddrinfo struct */
    free(res);
}

