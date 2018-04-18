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

 
void syncBack(){
	int bfd;
	struct sockaddr_in my_addr;
	

	if((bfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10101);
	inet_aton("127.0.0.1", &my_addr.sin_addr);
	

	if(connect(bfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in))== -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	

	//SYNC






	close(bfd);

}

int main(){
	int sfd, cfd, size, index=0 , region = -1;
	char buf[10];
	Element elmBuf;
	char data[10][100];

	char pathSocket[108];
	
	syncBack();	
	sprintf(pathSocket, "./%s", CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr, cli_addr;
	socklen_t cli_addrlen;

	unlink(CLIPBOARD_SOCKET);

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}	
	
	//bind()

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
			
			
			/*
			 __4___3__4,___
			|1234|567|8,123|
			*/	
			if(index + size >= sizeof(Element)){
				printf("\nFinal:Read:%d\t index:%d\n", size, index);
				memcpy(&elmBuf+index, buf,  sizeof(Element) - index);
				if(index + size > sizeof(Element)){
					memcpy(&elmBuf, &buf+(sizeof(Element) - index), size);
				}
				//save;
				region = elmBuf.region;
				if(elmBuf.type == 'C'){
					if(region <= 9 && region >= 0){
						memcpy(data[region], &elmBuf.content, 100);
						printf(">[%d] - %s\n", region, data[region] );
					}
				}
				printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

				index = 0;

			}else{
				printf("\nRead:%d\t index:%d\n", size, index);
				memcpy(&elmBuf+index, &buf, size);
				index+=size;
			}

		}

		for(int i = 0; i < 10; i++){
			printf("[%d] - %s\n", i, data[i] );
		}
		close(cfd);
	}
	close(sfd);
	exit(0);
	
}
