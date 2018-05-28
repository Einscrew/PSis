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
	//printf("[%d]->%d going to read from: %d\n", getpid(), size, from );

	if(size > 0 && size < INT_MAX){
		*buf = mallocV(size, "recv tmp variables");
		miss = size;
		read = 0;
		
		while (read < size){
			if((n=recv(from, *buf+read, miss, 0)) <= 0){
				//printf("[%d]Error receiving\n", getpid());
				read = -1;
				free(*buf);
				*buf =NULL;
				break;
			}
			if(n == 0){
				//printf("[%d]READING 0????\n", getpid());
				read = -1;
				free(*buf);
				*buf =NULL;
				break;
			}
			miss -= n;
			read += n;
			
			//printf("[%d]missing %d read %d, n %d",miss, read, n);			
		}

	}else if(size == 0){
		*buf = NULL;
		read = 0;
	}else{
		*buf = NULL;
		read = -1;
	}
	
	free(s);
	return read;
}


int createListenerUnix(){
	int sfd;
	char pathSocket[108];

	sprintf(pathSocket, "./%d", getpid());
	if(mkdir(pathSocket, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		printf("[%d]Couldn't creat socket folder: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	sprintf(pathSocket, "./%d/%s", getpid(), CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr;

	unlink(pathSocket);

	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("[%d]Couldn't open socket: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, pathSocket, sizeof(my_addr.sun_path)-1);

	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) == -1){
		printf("[%d]Couldn't bind UNIX socket: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}

	if (listen(sfd, 10) == -1){
		printf("[%d]Couldn't listen: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}
	return sfd;
}


int setupParentListener(){
	int yes = 0, sfd = -1, portM, portm, port;
	struct sockaddr_in my_addr;
	char buf[6];

	if((sfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("[%d]Couldn't create socket: %s\n", getpid(), strerror(errno));
		return -1;
	}

	
	if(( setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ) == -1){
		printf("[%d]Couldn't set socket: %s\n", getpid(), strerror(errno));
		close(sfd);
		return -1;
	}

	FILE * ports = NULL;
	if( (ports=fopen("ports.txt", "r" )) == NULL ){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fgets(buf, 6, ports);//lê porto
	sscanf(buf, "%d", &portM);

	fgets(buf, 6, ports);//lê porto
	sscanf(buf, "%d", &portm);

	fclose(ports);

	if(portM < portm){
		port = portM;
		portM = portm;
		portm = port;
	}

	port = portm;

	
	//char name [256];
	//gethostname(name, 256);
	//struct hostent * h = gethostbyname(name);
	while(1){

		
		if( port >= 65536 || port > portM){
			printf("[%d]Couldn't bind AF_INET socket\n", getpid());
			exit(EXIT_FAILURE);
		}
		memset(&my_addr, 0, sizeof(struct sockaddr_in));
		my_addr.sin_family = AF_INET;
		
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		my_addr.sin_port = htons(port);


		if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == -1){
		
		}else{
			printf("[%d]Port: %d\n", getpid(), port);
			break;
		}
		port++;
	}
	
	
	if (listen(sfd, 1) == -1){
		printf("[%d]Couldn't listen: %s\n", getpid(), strerror(errno));
		close(sfd);
		return -1;
	}
	return sfd;
}

int connect2parent(char * argip, char * argport){
	int bfd, port = atoi(argport);
	struct sockaddr_in my_addr;


	if((bfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("[%d]Couldn't create socket to communicate with parent clipboard: %s\n", getpid(), strerror(errno));
		return -1;
	}
	
	memset(&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET;

	printf("[%d]Connecting to %s:%d...\t", getpid(), argip, port);
	
	my_addr.sin_port = htons(port);
	
	if(inet_aton(argip, &my_addr.sin_addr) == 0){
		printf("[%d][Invalid ip]\n", getpid());
		return -1;
	}
	
	if(connect(bfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in))== -1){
		printf("[%d][Couldn't connect to parent clipboard: %s]\n", getpid(), strerror(errno));
		return -1;
	}

	printf("[%d][Success]\n", getpid());
	return bfd;
}
