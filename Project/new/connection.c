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

#include <limits.h>
#include <sys/stat.h>

#include "clipboard.h"
#include "connection.h"
#include "utils.h"

int PORT = 10101;
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
		if((n = write(to, buf+written, size-written)) < 0 ){
			return -1;
		}

		written += n;
	}
	return written;
}

//TODO: if n == 0 what??
int recvMsg(int from, void ** buf){
	int read = 0,  n = 1, miss = sizeof(int), size = 0;
	char * s = mallocV(sizeof(int), "recv tmp variables");
	//get size of read
	if(buf == NULL){
		return -1;
	}
	
	while( miss > 0 ){
		n = recv(from, &s[read], miss,0);
		if(n < 0 ){ free(s);return -1;}
		if(n == 0){ free(s);return 0;}
		miss -= n;
		read += n;
	}

	memcpy(&size, s, sizeof(int));
	//size should be changed @ this point
	//printf("->%d going to read from: %d\n", size, from );
	if(size > 0 && size < INT_MAX){
		*buf = mallocV(size, "recv tmp variables");
		miss = size;
		read = 0;
		
		while (read < size){
			if((n=recv(from, *buf+read, miss, 0)) < 0){
				printf("Error receiving\n");
				read = -1;
				free(*buf);
				break;
			}
			if(n == 0){
				printf("READING 0????\n");
				read = -1;
				free(*buf);
				break;
			}
			miss -= n;
			read += n;
			
			//printf("missing %d read %d, n %d",miss, read, n);			
		}

	}else{
		printf("UUUPS negative size\n");
		*buf = NULL;
		read = -1;
	}
	//printf("///////////////////////\n");
	free(s);
	return read;
}


int createListenerUnix(){
	int sfd;
	char pathSocket[108];

	sprintf(pathSocket, "./%d", getpid());
	if(mkdir(pathSocket, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		printf("Couldn't creat socket folder: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	sprintf(pathSocket, "./%d/%s", getpid(), CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr;

	unlink(pathSocket);

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


int setupParentListener(){
	int yes, sfd, port;
	struct sockaddr_in my_addr;
	char buf[6], *ip =NULL;

	if((sfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't create socket: %s\n", strerror(errno));
		return -1;
	}

	
	if(( setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) ) == -1){
		printf("Couldn't set socket: %s\n", strerror(errno));
		close(sfd);
		return -1;
	}

	FILE * ports = NULL;
	if( (ports=fopen("ports.txt", "r" )) == NULL ){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fgets(buf, 6, ports);//lê porto
	sscanf(buf, "%d", &port);


	
	//char name [256];
	//gethostname(name, 256);
	//struct hostent * h = gethostbyname(name);
	while(1){

		
		if( port >= 65536){
			printf("Couldn't bind socket\n");
			exit(EXIT_FAILURE);
		}
		memset(&my_addr, 0, sizeof(struct sockaddr_in));
		my_addr.sin_family = AF_INET;
		
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		my_addr.sin_port = htons(port);


		if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1){
		
		}else{
			if((ip=inet_ntoa(my_addr.sin_addr)) != NULL){
				printf("IP:%s\n",ip );
			}
			printf("Port: %d\n", port);
			break;
		}
		port++;
	}
	
	
	if (listen(sfd, 1) == -1){
		printf("Couldn't listen: %s\n", strerror(errno));
		close(sfd);
		return -1;
	}
	return sfd;
}

int connect2parent(char * argip, char * argport){
	int bfd, port = atoi(argport);
	struct sockaddr_in my_addr;


	if((bfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't create socket to communicate with parent clipboard: %s\n", strerror(errno));
		return -1;
	}
	
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;

	printf("Connecting to %s:%d\n", argip, port);
	
	my_addr.sin_port = htons(port);
	
	if(inet_aton(argip, &my_addr.sin_addr) == 0){
		printf("Invalid ip\n");
		return -1;
	}
	
	if(connect(bfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in))== -1){
		printf("Couldn't connect to parent clipboard: %s\n", strerror(errno));
		return -1;
	}
	
	return bfd;
}
