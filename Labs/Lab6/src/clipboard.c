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
	int sfd, cfd, size;
	char buf[10];
			
	struct sockaddr_un my_addr, cli_addr;
	socklen_t cli_addrlen;

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	// when????????????
	unlink(CLIPBOARD_SOCKET);
	//bind()
	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, CLIPBOARD_SOCKET, sizeof(my_addr.sun_path)-1);

	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) == -1){
		printf("Couldn't bind socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//listem()
	if (listen(sfd, 0) == -1){
		printf("Couldn't listen: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	//accept()
	cli_addrlen = sizeof(struct sockaddr_un);
	cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
	if(cfd == -1){
		printf("Couldn't accept client connection: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while(1){
		size = recv(cfd, &buf, 10,0);
		write(1, &buf, size);

	}


	close(cfd);
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
