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

#include <pthread.h>

#include "clipboard.h"

#define FALSE 0
#define TRUE 1

char clip[10][100];

typedef struct argT{
	int fd;
	long int * working;
}argT;

int sendMsg(Element * e, int fd){
	char * msg = (char*)malloc(sizeof(Element));
	Element el;
	el.type='C';
	el.region =1;
	memcpy(el.content, "olaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\0", 100);

	memcpy(msg, e, sizeof(Element));
	int n, count=0;

	while(count < sizeof(Element)){
		printf("Attempts for sendMsg\n");
		n = write(fd, msg, sizeof(Element));
		if(n == -1){
			printf("%s\n", strerror(errno));
			return -1;
		}
		count += n;

	}
	return 1;
}
 
int syncBack(char * opt){

	int bfd, n, size, i;
	struct sockaddr_in my_addr;
	char buf[10] = "", bufFull[100];
	Element e;

	if((bfd = socket(AF_INET, SOCK_STREAM, 0) ) == -1){
		printf("Couldn't create socket: %s\n", strerror(errno));
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
	
	//Syncronization
	char * msg = (char*)malloc(sizeof(Element));
	
	for (i = 0; i < 10; ++i)
	{
		e.type = 'P';
		e.region = i;

		//memcpy(e.content, "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooop\0", 100);

		memcpy(msg, &e, sizeof(Element));
		n = write(bfd, msg, sizeof(Element));
		//printf("Writing [%d]-%d\n",i, n);

		n = 0;
		while((size = read(bfd, buf, 10)) > 0){
			///printf("read:#%d-[%s]\n",size , buf);

			memcpy(bufFull+n, buf, size);
			n +=size;
			if(buf[size-1] == '\0'){
				memcpy(clip[i], bufFull, n);
				break;
			}
		}
		if(size == -1){
			printf("Could not complete sync\n");
			exit(EXIT_FAILURE);
		}

		//printf("[%d]-[%s]\n", i, bufFull);
		
	}
	for (i = 0; i < 10; ++i)
	{
		printf("[%d]-[%s]\n", i, clip[i]);
	}
	free(msg);
	return bfd;

}

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

	if (listen(sfd, 10) == -1){
		printf("Couldn't listen: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}
	return sfd;
}


void handleRequest(Element *elmBuf, char* bufFull, int cfd, int sync, int bfd){

	memmove(elmBuf, bufFull, sizeof(Element));
	int n=0;

	if(elmBuf->type == 'C'){
		//local save
		memcpy(clip[elmBuf->region], elmBuf->content, 100);
		printf("[%d]-[%s]\n", elmBuf->region, clip[elmBuf->region] );

		/*//remote save
		if(sync){
			sendMsg(elmBuf, bfd);
		}*/

	}else if(elmBuf->type == 'P'){
		printf("TODO send region:\n");
		n=write(cfd, clip[elmBuf->region] ,strlen(clip[elmBuf->region]));
		printf("Send: %d\n", n);
	}else{
		printf("Unknown instruction\n");
	}
}

void *handleConnection(void *arg){

	argT * a = (argT *)arg;
	int sock = a->fd;

	int size = 0, index = 0, left2cpy = sizeof(Element);
	char *bufFull = (char*)malloc(sizeof(Element));
	char buf[10] = "";

	int sync = FALSE;

	Element elmBuf;

	//CHECK IF size buf > size(Element) --------------------------_>>>>>>>>>>>>>>>>>>>>>>>>>>>
	while((size = recv(sock, &(buf), 10, 0)) > 0){
		//printf("Read:%d\t index:%d\n", size, index);
				
		if(index + size >= sizeof(Element)){
			//printf("size[%d] index[%d]\n", size, index);
			printf("Saving...\n");
			left2cpy = sizeof(Element) - index;
			memcpy(bufFull+index, buf, left2cpy);
			
			
			handleRequest(&elmBuf, bufFull, sock, sync, 0);//REPLACE BFD <...............
			printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

			if(index + size > sizeof(Element)){
				index =  size - left2cpy;
				memcpy(bufFull, buf+(left2cpy), index);						
			}
			else{
				index = 0;
			}

		}else{
			memcpy(bufFull+index, buf, size);
			index+=size;
		}

	}

	free(bufFull);
	close(sock);
	//MUTEX HEEEEEEEEEEEEEEERE
	*(a->working)= -1;

	return 0;
}


int main(int argc, char *argv[]){

	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;
	int n = 4;

	pthread_t * p = malloc(sizeof(pthread_t) * n);
	argT * arg = (argT*)malloc(sizeof(argT)*n);
	long int * working =  (long int*)malloc(sizeof(long int)*n);

	/*int i,sfd, cfd, bfd, size, index=0, left2cpy=sizeof(Element);
	char * bufFull =(char*) malloc(sizeof(Element));
	char buf[1000], opt;*/

	char opt;
	int i=0, bfd, sfd, cfd;

	//FLAGS
	int sync=FALSE;

	for (i =0 ; i < n; i++){
		working[i] = -1;
	}
	for (i = 0; i < 10; ++i)
		clip[i][0]='\0';
	
	while ((opt = getopt(argc, argv, "c:")) != -1) {
	    switch (opt) {
	    case 'c':
	        bfd = syncBack(optarg);
	        sync = TRUE;
	        break;
	    default: /* '?' */
	        fprintf(stderr, "Usage: %s [-c ip:port]\n",
	                argv[0]);
	        exit(EXIT_FAILURE);
	    }
	}
	
	sfd = createListener();

	//accept()
	while(1){

		for(i = 0; i<2; i++){
			if(working[i] == -1){
				cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
				printf("-----------%d accepted\n", i);
				if(cfd == -1){
					printf("Couldn't accept client connection: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				printf("sizeof(Element):%ld\n", sizeof(Element));
								
				arg[i].fd = cfd;
				working[i] = 1;
				arg[i].working = &working[i];
				/* Creates a new thread after accepting a new connection */
				if(pthread_create(&p[i], NULL, handleConnection, (void*)&arg[i]) < 0){
					printf("Couldn't create new thread: %s\n", strerror(errno));
					free(arg);
					close(cfd); 
					close(sfd);
					exit(EXIT_FAILURE);
				}
				printf("thread create @ %d\n",i );
			}
		}
		
	}
	close(sfd);
	//pthread_exit(NULL);
	exit(0);
	
}
