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

int handleRequest(int size, char* request, int cfd, int sync, int bfd){
	int region = request[1]-'0', b_size; //LONG INT??
	char * buf = NULL;

	switch(request[0]){
		case 'C':
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

			printf("[%d]-[%s]\n", region, clip[region].data );
			//unlock region
			sleep(10);
			printf("leaving\n");
			pthread_cond_broadcast(&w[region]);

			pthread_mutex_unlock(&waitlock[region]);
			pthread_rwlock_unlock(&cliplock[region]);

			//remote save
			if(sync){
				//reply(elmBuf, bfd);
			}
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
			
			if(sendMsg(cfd, buf, size) == -1){
				return -1;
			}
			free(buf);
			break;

		case 'W':
			printf("WWWWWWWW received\n");
			buf = malloc(2);
			buf[0]='!';
			buf[1]='\0';
			sendMsg(cfd, buf, 2);
			//waitRegion();
			break;

		default:
			printf("Unknown instruction\n");
			break;
	}
	return 0;

}

void thread_attend(void * arg){
	argT *a =(argT*) arg;

	int cfd = a->fd, n, i;

	char * msg = NULL;

	// Pass as arg;
	int sync = FALSE;
	int bfd = 0;

	while ((n = recvMsg(cfd, (void**)&msg)) > 0){
		printf("\n>>>>>>>>>>>>>>>>>>>>>\n");
		printf("\n|");
		for ( i = 0; i < n; ++i)
		{
			printf("%c|", msg[i]);
		}
		printf("\n");
		if(handleRequest(n, msg, cfd, sync, bfd) == -1){
		}
		
	}
	printf("-----------------------------------\n");
	for ( i = 0; i < 10; ++i)
	{
		printf("[%d]-[%s]\n",i, clip[i].data );
	}

	close(cfd);
	*(a->working) = -1;
}

int main(int argc, char *argv[]){
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	//FLAGS
	int sync = FALSE;

	int i, n = 4 ,sfd, cfd, bfd;
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
	       // bfd = syncBack(optarg);
	        sync = TRUE;
	        break;
	    default: /* '?' */
	        fprintf(stderr, "Usage: %s [-c ip:port]\n",
	                argv[0]);
	        exit(EXIT_FAILURE);
	    }
	}

	
	sfd = createListenerUnix();

	//accept()
	while(1){
		for (int i = 0; i < n; ++i)
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
				pthread_create(&threads[i], NULL, (void*)thread_attend, (void*)&args[i]);
			}
		}
		//printf("busyXXXXXXX\n");
	}

	close(sfd);	
	exit(0);
	
}
