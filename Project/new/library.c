#include <sys/un.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "utils.h"
#include "connection.h"
#include "clipboard.h"


int clipboard_connect(char * clipboard_dir){
	int sfd;
	struct sockaddr_un my_addr;

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Local clipboard couldn't be accessed: [%s]\n", strerror(errno));
		return -1;
	}


	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, clipboard_dir, strlen(clipboard_dir));

	if(connect(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) != 0){
		printf("Couldn't open socket: %s\n", strerror(errno));
		return -1;
	}
	
	return sfd;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	if(region < 0 || region > 9 || count < 0 || clipboard_id < 0) return 0;

	long int finalsize = count+sizeof(char)+sizeof(char);
	
	// C|10|olawjmidoanwdanwjabwdjawdawd
	char *msg = (char*)mallocV(finalsize, ": copy msg alloc");

	msg[0]='C';
	msg[1]=region+'0';
	memcpy(msg+2, buf, count);
	
	int r = sendMsg(clipboard_id, msg, finalsize);
	return (r == -1)?(0):(count);
}


int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	if(region < 0 || region > 9 || count <= 0 || clipboard_id < 0) return 0;

	char toSend[2];
	void * p = NULL;
	int r = 0;
	
	// P|10|19
	toSend[0]='P';
	toSend[1]=region+'0';
	if(sendMsg(clipboard_id, toSend, 2) == -1)
		return 0;

	memset(buf, '\0', count);
	// 28|olawjmidoanwdanwjabwdjawdawd|
	if((r = recvMsg(clipboard_id, (void**)&p)) <= 0){
		return 0;
	}
	count = (r<count)?r:count; // TO CHANGE
	//printf(">>>%s|%d\n", (char*)p, (int)count );
	memcpy(buf, p, count);
	
	free(p);

	return r;
}


int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){
	if(region < 0 || region > 9 || count <= 0 || clipboard_id < 0) return 0;

	char toSend[2];
	void * p = NULL;
	int r = 0;
	
	// P|10|19
	toSend[0]='W';
	toSend[1]=region+'0';
	if(sendMsg(clipboard_id, toSend, 2) == -1)
		return 0;

	// 28|olawjmidoanwdanwjabwdjawdawd|
	if((r = recvMsg(clipboard_id, (void**)&p)) == -1){
		return 0;
	}
	//printf("\nreceived %d bytes\n",r);

	count = (r<count)?r:count;
	//printf("Received from [%d] - %s||\n",region, (char*)p);

	memcpy(buf, p, count);
	
	free(p);

	return r;
}

void clipboard_close(int clipboard_id){
	close(clipboard_id);
}