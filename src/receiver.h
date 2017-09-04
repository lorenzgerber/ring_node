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

#define BUF_SIZE 500
#define NI_MAXHOST 1025
#define NI_MAXSERV 32


#ifndef __RECEIVER
#define __RECEIVER



/* Sets up a socket for receiving messages for the program
 * @param server   host struct of this node
 * @returned    void pointer (needed to end the thread)
 */
void *listenUdp(void *server);

/* Sets up a socket for receiving messages for the program
 * @param server   host struct of receiving node
 * @returned    void pointer (needed to end the thread)
 */
void *sendUdp(void *sendTo);


/* Will receive and print to the host, uses protocol UDP.
 * @param   ht      The host of the program
 * @param   sfd     The socket it will receive data to.
 */
void receiver_listenUDP(host *ht, int sfd);

/* Will receive and print messages to the host, uses protocol TCP.
 * @param   ht      The host of the program
 * @param   sfd     The socket it will receive data to.
 */
void receiver_listenTCP(host *ht, int sfd);

#endif
