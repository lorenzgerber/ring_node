#ifndef UDPNODE_H_
#define UDPNODE_H_
#include "receiver.h"

char* createNodeId(nodeAddr* this);

void printWrongParams(char *progName);

int* getIntFromStr(char *givenStr);

char* getMemHostName(char* hostString);

char* getPortMemString(char* port);

#endif

