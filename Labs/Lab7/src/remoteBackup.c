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
 
int main(){
	int sfd, cfd, size, index=0;
	char buf[10];
	Element elmBuf;
	char pathSocket[108];
	
	sprintf(pathSocket, "./%s", BACK_UP_SOCKET);
	struct sockaddr_in my_addr, cli_addr;
	socklen_t cli_addrlen;

	unlink(BACK_UP_SOCKET);

	if((sfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	
	//bind()
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10101);
	inet_aton("127.0.0.1", &my_addr.sin_addr);


	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1){
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
	//accept()
	while(1){
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(cfd == -1){
			printf("Couldn't accept client connection: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		index=0;
		while((size = recv(cfd, &(buf), 10,0)) > 0){
			write(1,&(buf),size);
		}
		close(cfd);
	}
	close(sfd);
	exit(0);
	
}
