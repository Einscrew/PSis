#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "clipboard.h"

char clip[10][100];
 

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

	//listem()
	if (listen(sfd, 0) == -1){
		printf("Couldn't listen: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}
	return sfd;
}


void handleRequest(Element * elmBuf, char* bufFull, int cfd){

	memmove(elmBuf, bufFull, sizeof(Element));
	int n=0;

	if(elmBuf->type == 'C'){
		memcpy(clip[elmBuf->region], elmBuf->content, 100);
		printf("[%d]-[%s]\n",elmBuf->region, clip[elmBuf->region] );
	}else if(elmBuf->type == 'P'){
		printf("TODO send region:\n");
		n=write(cfd, clip[elmBuf->region],strlen(clip[elmBuf->region]));
		printf("Send: %d\n", n);
	}else{
		printf("Unknown instruction\n");
	}
}

 
int main(){
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	int i,sfd, cfd, size, index=0, left2cpy=sizeof(Element);
	char * bufFull =(char*) malloc(sizeof(Element));
	char buf[10];

	Element elmBuf;
	

	for (i = 0; i < 10; ++i)
	{
		clip[i][0]='\0';
	}


	
	sfd = createListener();

	//accept()
	while(1){
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(cfd == -1){
			printf("Couldn't accept client connection: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("sizeof(Element):%ld\n", sizeof(Element));
		index=0;
		while((size = recv(cfd, &(buf), 10,0)) > 0){
			//printf("Read:%d\t index:%d\n", size, index);
					
			/*
			 __4___3__4,___
			|1234|567|8,123|
			*/	
			if(index + size >= sizeof(Element)){
				//printf("size[%d] index[%d]\n", size, index);
				printf("Saving...\n");
				left2cpy = sizeof(Element) - index;
				memcpy(bufFull+index, buf, left2cpy);
				
				
				handleRequest(&elmBuf, bufFull, cfd);
				printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

				if(index + size > sizeof(Element)){
					memcpy(&bufFull, &buf+(left2cpy), size);
					
				}
				index = 0;

			}else{
				memcpy(bufFull+index, &buf, size);
				index+=size;
			}

		}

		printf("-----------------------------------\n");
		for ( i = 0; i < 10; ++i)
		{
			printf("[%d]-[%s]\n",i, clip[i] );
		}

		close(cfd);
	}

	free(bufFull);
	close(sfd);	
	exit(0);
	
}
