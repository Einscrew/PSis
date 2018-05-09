#ifndef CONNECTION_H
#define CONNECTION_H

int sendMsg(int to, void * buf, int size);

int recvMsg(int from, void ** buf);

int createListenerUnix();

#endif