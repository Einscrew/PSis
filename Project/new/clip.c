#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/un.h>


#include "clip.h"

#include "clipboard.h"


int createListener(){
	int sfd;
	char pathSocket[108];
	sprintf(pathSocket, "./%s", CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr;

	unlink(CLIPBOARD_SOCKET);

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, pathSocket, sizeof(my_addr.sun_path)-1);

	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) == -1){
		printf("Couldn't bind socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}

	if (listen(sfd, 0) == -1){
		printf("Couldn't listen: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}
	return sfd;
}
//TODO: if n == 0 what??
int sendMsg(int to, void * buf, int size){
	int written = 0,  n = 0, miss = sizeof(int);
	
	while ( (n = write(to, &size+written, miss)) > 0){
		miss -= n;
		written += n;
	}
	
	if(n < 0 ) return -1;	

	written = 0;
	while (written < size){
		if((n = write(to, buf+written, size-written)) <= 0 ){
			return -1;
		}
		written += n;
	}
	return written;
}

//TODO: if n == 0 what??
int recvMsg(int from, void ** buf){
	int read = 0,  n = 1, miss = sizeof(int), size = 0;
	char * s = malloc(sizeof(int));
	//get size of read
	if(buf == NULL){
		return -1;
	}
	
	while( miss > 0 ){
		n = recv(from, &s[read], miss,0);
		if(n < 0 ) return -1;
		miss -= n;
		read += n;
	}
	memcpy(&size, s, sizeof(int));
	//size should be changed @ this point
	printf("->%d going to read\n", size );
	*buf = malloc(size);
	read = 0;
	miss = size;
	
	while (miss > 0){
		printf("missing %d read %d",miss, read);
		if((n=recv(from, *buf+read, miss,0)) < 0){
			printf("Error receiving\n");
			exit(EXIT_FAILURE); //TODO return error instead
		}
		printf(", n %d\n", n);
		miss -= n;
		read += n;
	}
	
	return read;
}

int main(int argc, char const *argv[])
{
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	int n, cfd, sfd = createListener();

	//accept()
	cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
	if(cfd == -1){
		printf("Couldn't accept client connection: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while(1){
		char *b = NULL;
		n = recvMsg(cfd, (void**)&b);
		printf("FINAL: %d\n",n );
		/*printf("\n");
		for (int i = 0; i < n; ++i)
		{
			printf("%d ", b[i]);
		}*/
		free(b);
		
	}
	return 0;
}