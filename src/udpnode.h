/*
 * node.h
 *
 *  Created on: Sep 3, 2017
 *
 */


#ifndef UDPNODE_H_
#define UDPNODE_H_


typedef struct host {
    char *name;
    char* port;
}host;


typedef struct message {
    int IdHigh;
    char* messege;
}messege;

void printWrongParams(char *progName);

int* getIntFromStr(char *givenStr);

char* getCurrentHostName();

#endif  /*UDPNODE_H_*/

