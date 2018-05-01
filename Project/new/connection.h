
int sendMsg(int to, void * buf, int size);

int recvMsg(int from, void ** buf);

int reply(Element * e, int fd);

int createListenerUnix();