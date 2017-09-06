/*
 * File: node.c
 *
 * Date:        2017-08-27
 *
 *
 */

#include "udpnode.h"


int main(int argc, char **argv) {

    //char* nodeId;
    char* idHigh;
    int election;
    nodeAddr *thisAddr;
    nodeAddr *nextAddr;
    ringNode* node;

    if (argc != 4) {
        printWrongParams(argv[0]);
        return EXIT_FAILURE;
    }
    node = malloc(sizeof(ringNode));

    thisAddr = malloc(sizeof(nodeAddr));
    thisAddr->name = getMemHostName(NULL);
    thisAddr->port = getPortMemString(argv[1]);

    nextAddr = malloc(sizeof(nodeAddr));
    nextAddr->name = getMemHostName(argv[2]);
    nextAddr->port = getPortMemString(argv[3]);

    node->nodeId = createNodeId(thisAddr);

    node->this = thisAddr;
    node->next = nextAddr;
    node->isParticipant = 1;
    node->isMaster = 0;
    //printf("\n%s", nodeId);

    //phase 1
    election = 1;


    /* Create receiver communication thread */

    pthread_t listenerThread;
    pthread_t sendThread;
    if (pthread_create(&sendThread, NULL, &sendUdp, (void*)node) < 0) {
        perror("Error creating listener-thread");
        return EXIT_FAILURE;
    }


    if (pthread_create(&listenerThread, NULL, &listenUdp, (void*)node) < 0) {
        perror("Error creating listener-thread");
        return EXIT_FAILURE;
    }

    pthread_join(sendThread, NULL);
    pthread_join(listenerThread, NULL);

    if(node->isMaster){
        printf("this node is master");
    }
    free(thisAddr->port);
    free(thisAddr->name);
    free(nextAddr->name);
    free(nextAddr->port);
    free(node);


    //fprintf(stderr, "Exit-message received, goodbye!\n");
    return EXIT_SUCCESS;

}


void printWrongParams(char *progName) {
    fprintf(stderr, "%s\n%s %s\n",
            "Invalid parameters",
            progName,
            "<LOCAL PORT> <NEXT HOST> <NEXT PORT>");
}


/* Will take the given string and return a allocated integer of the
 * string representation.
 *  @param givenStr The string that will remade to a int.
 *  @return         Integer pointer of the the given string
 */
int* getIntFromStr(char *givenStr) {
    int p;
    int *fixedInt;
    char *strRest;
    if ((p = (int)strtol(givenStr, &strRest, 10)) == 0) {
        return NULL;
    }
    fixedInt = malloc(sizeof(int)*strlen(givenStr));
    *fixedInt = p;
    return fixedInt;
}


/* Will allocate a string and set the hostname from the input string.
 * NULL value inputstring returns the localhost
 * @return  current hostname.
 */
char* getMemHostName(char* hostString) {
    char tmpHost[255];
    memset(tmpHost, 0, 255);
    if(hostString == NULL){
        if (gethostname(tmpHost, 255) != 0) {
            return 0;
        }
        char *localhost = calloc(strlen(tmpHost)+1, sizeof(char));
        strcpy(localhost, tmpHost);
        return localhost;
    }else{
        char *host = calloc(strlen(hostString)+1, sizeof(char));
        strcpy(host, hostString);
        return host;
    }

}

char* getPortMemString(char* port){

    char* portString = calloc(strlen(port)+1, sizeof(char));
    strcpy(portString, port);
    return portString;
}

char* createNodeId(nodeAddr* this){
    char* tmpId = calloc(strlen(this->name)+strlen(this->port), sizeof(char));
    strcat(tmpId, this->name);
    strcat(tmpId, ",");
    strcat(tmpId, this->port);

    return tmpId;
}


