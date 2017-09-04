/*
 * File: node.c
 *
 * Date:        2017-08-27
 *
 *
 */

#include "udpnode.h"
#include "receiver.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {

    int nodeId;
    int isElected;
    host *this;
    host *next;

    if (argc != 5) {
        printWrongParams(argv[0]);
        return EXIT_FAILURE;
    }
    for(int i = 1; i < argc; i++){
        printf("\n%s",argv[i]);
    }
    if(strcmp(argv[4], "0") == 0){
        isElected = 0;
    }else{
        isElected = 1;
    }


    this = malloc(sizeof(host));
    this->name = getMemHostName(NULL);
    this->port = getPortMemString(argv[1]);

    next = malloc(sizeof(host));
    next->name = getMemHostName(argv[2]);
    next->port = getPortMemString(argv[3]);

    /* Create receiver communication thread */
    if(isElected){
        pthread_t sendThread;
        if (pthread_create(&sendThread, NULL, &sendUdp, (void*)this) < 0) {
            perror("Error creating listener-thread");
            return EXIT_FAILURE;
        }
        pthread_join(sendThread, NULL);
    }else{
        pthread_t listenerThread;
        if (pthread_create(&listenerThread, NULL, &listenUdp, (void*)this) < 0) {
            perror("Error creating listener-thread");
            return EXIT_FAILURE;
        }
        pthread_join(listenerThread, NULL);
    }



    fprintf(stderr, "Exit-message received, goodbye!\n");
    return EXIT_SUCCESS;

}


void printWrongParams(char *progName) {
    fprintf(stderr, "%s\n%s %s\n",
            "Invalid parameters",
            progName,
            "<LOCAL PORT> <NEXT HOST> <NEXT PORT> <ELECTED DEBUG FLAG>");
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

