#include <sys/types.h>
#include <sys/socket.h>
 #include <sys/un.h>


#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>


#include <unistd.h>

#include "clipboard.h"

 
int main(){
	int sfd, cfd, size, index=0;
	char buf[10];
	Element elmBuf;
	char pathSocket[108];
	
	sprintf(pathSocket, "./%s", CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr, cli_addr;
	socklen_t cli_addrlen;

	unlink(CLIPBOARD_SOCKET);

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// when????????????
	
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
			printf("\nRead:%d\t index:%d\n", size, index);
			
			/*
			 __4___3__4,___
			|1234|567|8,123|
			*/	
			if(index + size >= sizeof(Element)){
				memcpy(&elmBuf+index, buf,  sizeof(Element) - index);
				//save;
				printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

				if(index + size > sizeof(Element)){
					memcpy(&elmBuf, &buf+(sizeof(Element) - index), size);
					
				}
				index = 0;

			}else{
				memcpy(&elmBuf+index, &buf, size);
				index+=size;
			}

		}


		close(cfd);
	}
	close(sfd);
	/*
	int len_data;
	char clip[10][10];

	Element inbox;
	while(1){
		memset(&inbox, 0, sizeof(Element));
		printf(".\n");
		read(fifo_in, &inbox, sizeof(Element));
		printf("received %c\n", inbox.type);

		if(inbox.type == 'C'){
			memcpy(clip[(int)inbox.region], inbox.content, sizeof(inbox.content));
			printf("copying: %s ---\n         %c\n         %d\n", inbox.content , inbox.type, inbox.region);

		}else if(inbox.type == 'P'){
			write(fifo_out, &clip[(int)inbox.region], sizeof(clip[(int)inbox.region]));
			printf("pasted: %s\n", clip[(int)inbox.region]);

		}

	}
	*/	
	exit(0);
	
}
