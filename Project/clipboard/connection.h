#ifndef CONNECTION_H
#define CONNECTION_H

/* Sends a message to the given file descriptor */
int sendMsg(int to, void * buf, int size);

/* Receives a message from the given file descriptor */
int recvMsg(int from, void ** buf);

/* Creates an UNIX socket to accept new connections from applications */
int createListenerUnix();

/* Creates a stream socket to accept new connections from remote clipboards */
int setupParentListener();

/* Establishes a connection with the parent clipboard given by the user */
int connect2parent(char * argip, char * argport);

#endif