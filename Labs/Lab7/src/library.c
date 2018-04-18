#include "clipboard.h"
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

int clipboard_connect(char * clipboard_dir){
	int sfd;
	struct sockaddr_un my_addr;
	char pathSocket[108];
	sprintf(pathSocket, "./%s", CLIPBOARD_SOCKET);

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, pathSocket, sizeof(my_addr.sun_path)-1);

	if(connect(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un))== -1){
		printf("Couldn't open socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	return sfd;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	Element e;
	e.type = 'C';
	e.region = region;
	memset(e.content, 0, 100);
	memcpy(e.content, buf, count);
	
	char *msg = (char*)malloc(sizeof(Element));

	memcpy(msg, &e, sizeof(Element));
	printf("write:%ld\n", sizeof(Element));
	write(clipboard_id, msg, sizeof(Element));

	return 0;
}


int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	Element e;
	e.type = 'P';
	e.region = region;
	memcpy(e.content, buf, count);
	e.content[0] = '\0';
	
	char * msg = (char*)malloc(sizeof(Element));
	memcpy(msg, &e, sizeof(Element));

	write(clipboard_id, msg, sizeof(Element));

	//read(clipboard_id, buf, count);

	return 1;
}
