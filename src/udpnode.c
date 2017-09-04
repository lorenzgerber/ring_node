/*
 * File: udpnode.c
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
    char* debugMessege;

    debugMessege = "electedNode...sendning";

    if (argc != 5) {
        printWrongParams(argv[0]);
        return EXIT_FAILURE;
    }

    if(argv[3] == "1"){
        isElected = 0;
    }else{
        isElected = 1;
    }


    this = malloc(sizeof(host));
    this->name = getCurrentHostName();
    strcpy(this->port, argv[1]);

    next = malloc(sizeof(host));
    next->name = argv[2];
    strcpy(next->port, argv[3]);

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


/* Will allocate a string and set the current hostname.
 * @return  current hostname.
 */
char* getCurrentHostName() {
    char tmpHost[255];
    memset(tmpHost, 0, 255);
    if (gethostname(tmpHost, 255) != 0) {
        return 0;
    }
    char *host = calloc(strlen(tmpHost)+1, sizeof(char));
    strcpy(host, tmpHost);
    return host;
}

