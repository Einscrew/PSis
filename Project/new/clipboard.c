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
#include "connection.h"

#define FALSE 0
#define TRUE 1


typedef struct argT{
	int fd;
	char * working;
}argT;

typedef struct clip{
	 char * data;
	 int size;
}Clip;

pthread_rwlock_t cliplock[10];
pthread_mutex_t waitlock[10];
pthread_cond_t w [10] = {PTHREAD_COND_INITIALIZER};
Clip clip[10];

void broadcastReq(int size, char * request, int fd){
	if(parent_fd == -1 || fd == parent_fd){
		while(aux)
	}
}
int waitRegion(int region, char ** content){
	int size;
	pthread_mutex_lock(&waitlock[region]);
	
	pthread_cond_wait(&w[region], &waitlock[region]);
	size = clip[region].size;
	*content = malloc(size);
	memcpy(*content, clip[region].data, size);
	pthread_mutex_unlock(&waitlock[region]);
	return size;
}


int handleRequest(int size, char* request, int fd){
	int region = request[1]-'0', b_size; //LONG INT??
	char * buf = NULL;

	switch(request[0]){
		case 'C':

			if(parent_fd == -1 || fd == parent_fd){
				//local save
				//lock region 2 copy as reader
				pthread_rwlock_wrlock(&cliplock[region]);
				pthread_mutex_lock(&waitlock[region]);

				printf("entering\n");
				if(clip[region].data != NULL){
					printf("freeing %d\n", region);
					clip[region].size = 0;
					free(clip[region].data-2);
				}
				//  0 1 2 3 4 5 6
				// -C|2|A|B|C|D|E|\0
				// /   /
				clip[region].data = &request[2];
				clip[region].size = size-2;

				printf("[%d]-[", region);
				fflush(stdout);
				write(1, clip[region].data, clip[region].size);
				printf("]-%d\n", clip[region].size );
				fflush(stdout);
				//unlock region
				sleep(5);
				printf("leaving\n");
				pthread_cond_broadcast(&w[region]);
				pthread_mutex_unlock(&waitlock[region]);
				pthread_rwlock_unlock(&cliplock[region]);


			}
			broadcastReq(size, request, fd);
			break;

		case 'P':
			pthread_rwlock_rdlock(&cliplock[region]);
			printf("on the write\n");
			b_size = clip[region].size;
			buf = malloc(b_size);
			memcpy(buf, clip[region].data, b_size);
			sleep(5);
			printf("leaving write\n");
			pthread_rwlock_unlock(&cliplock[region]);
			
			if(sendMsg(fd, buf, size) == -1){
				close(fd);
				return -1;
			}
			free(buf);
			break;

		case 'W':
			b_size = waitRegion(region, &buf);
			if(sendMsg(fd, buf, b_size) == -1){
				close(fd);
				return -1;
			}
			free(buf);
			break;
		default:
			printf("Unknown instruction\n");
			break;
	}
	return 0;
}

void attend_app(void * arg){
	argT *a =(argT*) arg;

	int cfd = a->fd, n, i;

	char * msg = NULL;

	while ((n = recvMsg(cfd, (void**)&msg)) > 0){
		printf("\n>>>>>>>>>>>>>>>>>>>>>\n");
		printf("\n|");
		for ( i = 0; i < n; i++)
		{
			printf("%c|", msg[i]);
		}
		printf("\n");

		if( n >= 3 && (msg[0] == 'C' || msg[0] == 'P' || msg[0] == 'W') && msg[1] <= '9' && msg[1] >= '0'){
			if( handleRequest(n, msg, cfd) == -1 ){
				//TODO
			}
		}else{
			free(msg);
			break;
		}		
	}
	printf("-----------------------------------\n");
	for ( i = 0; i < 10; ++i)
	{
		printf("[%d]-[", i);
		fflush(stdout);
		write(1, clip[i].data, clip[i].size);
		printf("]-%d\n", clip[i].size );
		fflush(stdout);
	}

	close(cfd);
	*(a->working) = -1;
}

void attend_clip(void * fd){
	int * clip_fd = (int*)fd;
	int i;
	//lock mutex fd
	for(i = 0; i<=9; i++){
		//acede e copia

		sendMsg();
	}
	while(){

	}
}
void listenChilds(){
	int fd = setupParentListener();
	struct sockaddr_in cli_addr;
	socklen_t cli_addrlen;

	if(fd != -1){
		while(1){
			int clip_fd = accept(fd, (struct sockaddr *) &cli_addr, &cli_addrlen);
			if(clip_fd == -1){
				printf("Couldn't accept client connection: %s\n", strerror(errno));
			}else{
				//add(List);
				pthread_create(NULL, NULL, (void*)attend_clip, &clip_fd);
			}

		}
	}
}
void initialSync(int *fd){
	int n;
	char * msg = NULL;
	while(1){
		n = recvMsg(*fd, (void**)&msg);	
		if( n >= 3 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			if( handleRequest(n, msg, *fd) == -1 ){
				
			}
		}else if(n > 0){
			free(msg);
			break;
		}else{
			close(*fd);
			*fd = -1;
		}
	}
}

int main(int argc, char *argv[]){
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	int i, n = 4 ,sfd, cfd, parent_fd;
	char opt;

	char * working = malloc(sizeof(char)*n);
	for (i = 0; i < n; ++i)
	{
		working[i] = -1;
	}
	argT * args = malloc(sizeof(argT) * n);
	pthread_t * threads = malloc(sizeof(pthread_t) * n);

	for (i = 0; i < 10; ++i){
		clip[i].data=NULL;
		clip[i].size=0;
		if(pthread_rwlock_init(&cliplock[i], NULL) == -1){
			printf("could create locks\n");
			exit(EXIT_FAILURE);
		}
	}
	
	while ((opt = getopt(argc, argv, "c:")) != -1) {
	    switch (opt) {
		    case 'c':
		        if((parent_fd = createListenerInet(optarg)) == -1){
		        	printf("On single mode\n");
		        }else{
		        	initialSync(&parent_fd);
		        }
		        break;
		    default: /* '?' */
		        fprintf(stderr, "Usage: %s [-c ip:port]\n",
		                argv[0]);
		        exit(EXIT_FAILURE);
	    }
	}

	

	pthread_create(NULL, NULL, (void*)listenChilds, NULL);

	sfd = createListenerUnix();

	// Create accept Clipboard threads


	//accept()
	while(1){
		for (i = 0; i < n; i++)
		{
			if(working[i] == -1){
				working[i] = 1;
				cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
				if(cfd == -1){
					printf("Couldn't accept client connection: %s\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
				
				args[i].fd = cfd;
				args[i].working = &working[i];
				pthread_create(&threads[i], NULL, (void*)attend_app, (void*)&args[i]);
			}
		}
		//printf("busyXXXXXXX\n");
	}

	close(sfd);
	if(parent_fd == -1) close(parent_fd);
	exit(0);
	
}
