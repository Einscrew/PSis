#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>


#include "clipboard.h"

#define BACK_UP_SOCKET "BACK_UP_SOCKET" 

char clip[10][100];

int setup(){
	int yes, sfd;
	char pathSocket[108];
	sprintf(pathSocket, "./%s", BACK_UP_SOCKET);
	struct sockaddr_in my_addr;

	unlink(BACK_UP_SOCKET);

	if((sfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't create socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	
	if(( setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) ) == -1){
		printf("Couldn't set socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10101);
	inet_aton("127.0.0.1", &my_addr.sin_addr);

	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1){
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

void handleRequest(Element * elmBuf, char* bufFull, int cfd){

	memmove(elmBuf, bufFull, sizeof(Element));
	int n=0;

	if(elmBuf->type == 'C'){
		memcpy(clip[elmBuf->region], elmBuf->content, 100);
		printf("[%d]-[%s]\n",elmBuf->region, clip[elmBuf->region] );
	}else if(elmBuf->type == 'P'){
		printf("TODO send region %d , len: %lu\n", elmBuf->region, strlen(clip[elmBuf->region])+1);
		//n=write(cfd, "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooop\0", 100);
		n=write(cfd, clip[elmBuf->region],strlen(clip[elmBuf->region])+1);
		printf("Send: %d\n", n);
	}else{
		printf("Unknown instruction\n");
	}
}

int main(){
	int sfd, cfd, size, index=0, i,left2cpy;
	char buf[10] , * bufFull = (char*)malloc(sizeof(Element));

	Element elmBuf;
	struct sockaddr_in cli_addr;
	socklen_t cli_addrlen;
	
	for (i = 0; i < 10; ++i)
	{
		clip[i][0]='\0';
	}

	sfd=setup();

	//accept()
	while(1){
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(cfd == -1){
			printf("Couldn't accept client connection: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		index=0;
		while((size = recv(cfd, &(buf), 10,0)) > 0){
			if(index + size >= sizeof(Element)){
				printf("size[%d] index[%d]\n", size, index);
				printf("Saving... index+size=%dVS%lu\n", index+size, sizeof(Element));
				left2cpy = sizeof(Element) - index;

				memcpy(bufFull+index, buf, left2cpy);
				
				//  V ---multithreading ISSUEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
				handleRequest(&elmBuf, bufFull, cfd);
				printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

				
				if(index + size > sizeof(Element)){
					index =  size - left2cpy;
					memcpy(bufFull, buf+(left2cpy), index);	
					printf(">>>>> contentsaved - %d, saved from: buf[%d-%d]\n", index,left2cpy, left2cpy+index);
				}else{
					index =  0;
				}

			}else{
				printf("size[%d] index[%d]\n", size, index);
				memcpy(bufFull+index, buf, size);
				index+=size;
			}

		}
		close(cfd);
	}
	close(sfd);
	exit(0);
	
}
