/*
 * File: receiver.h
 *
 * Date:        2017-08-27
 *
 */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <net/if.h>
#include <math.h>
#include "udpnode.h"

#define BUF_SIZE 100

#ifndef __RECEIVER
#define __RECEIVER

typedef struct nodeAddr {
    char *name;
    char* port;
}nodeAddr;

typedef struct ringNode{
    nodeAddr* this;
    nodeAddr* next;
    char* nodeId;
    char* highId;
    int isParticipant;
    int isMaster;
}ringNode;

/* Sets up a socket for receiving messages for the program
 * @param server, host struct with the nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *listenUdp(void *server);

/* Sets up a socket for receiving messages for the program
 * @param sendTo, host struct with the receiving nodes information
 * @returned    void pointer (needed to end the thread)
 */
void *sendUdp(void *sendTo);

void createElectionMessage(ringNode* node, char* buf);

void getIdFromMessage(const char* message, char* id);

#endif
