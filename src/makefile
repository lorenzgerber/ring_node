# Makefile
CC=gcc
CFLAGS= -Wall -std=c99 -pedantic -g -pthread

all: udpnode

udpnode: udpnode.c
	$(CC) $(CFLAGS) -o udpnode udpnode.c receiver.c
	