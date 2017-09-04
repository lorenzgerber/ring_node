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

#define BUF_SIZE 500


#ifndef __RECEIVER
#define __RECEIVER

typedef struct host {
    char *name;
    char* port;
    struct messege* mess;
}host;


struct message {
    char* type;
    int IdHigh;
    char* messege;
};

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


#endif
