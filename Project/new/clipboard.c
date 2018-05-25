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
#include <signal.h>

#include <pthread.h>

#include "connection.h"
#include "utils.h"
#include "list.h"


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

typedef struct elm{
	pthread_mutex_t fdMutex;
	int fd;
}Elm;

pthread_rwlock_t cliplock[10];
pthread_mutex_t waitlock[10];
pthread_cond_t w [10] = {PTHREAD_COND_INITIALIZER};
Clip clip[10];

pthread_rwlock_t parentLock;
int parent_fd = -1;

pthread_rwlock_t cLstLock;
t_list* childsList;


void sigint_handler(int n){
    close(parent_fd);
    printf("Clipboard terminating\n");
    exit(EXIT_SUCCESS);
}

void printClipboard(){
	printf("-----------------------------------\n");
	for ( int i = 0; i < 10; ++i)
	{
		pthread_rwlock_rdlock(&cliplock[i]);
		printf("[%d]-[", i);
		fflush(stdout);
		write(1, clip[i].data, clip[i].size);
		printf("]-%d\n", clip[i].size );
		fflush(stdout);
		pthread_rwlock_unlock(&cliplock[i]);
	}
}

void broadcastReq(int size, char * request, int fd){
	int pfd = -1;
	
	pthread_rwlock_rdlock(&parentLock);
	pfd = parent_fd;
	pthread_rwlock_unlock(&parentLock);

	if(pfd == -1 || fd == pfd){ // dispatch to all childs

		Elm * curr = NULL, *prev = NULL;
		
		t_list* aux = childsList;
		if(aux == NULL)
			printf("NO CHILDS TO ANSWER\n");
		else{
			prev = (Elm *)getItem(aux);
			pthread_mutex_lock(&(prev->fdMutex));
		}

		curr = prev;
		
		while(aux != NULL){
			
			printf("broadcast to %d result: ", curr->fd );fflush(stdout);

			if(curr->fd > 0){

				if(sendMsg(curr->fd, request, size) == -1){
					printf("X\n");fflush(stdout);
					close(curr->fd);
					curr->fd = -2;
				}else{
					printf("OK\n");fflush(stdout);
				}

			}

			aux = next(aux);
			if(aux != NULL){
				curr = (Elm *)getItem(aux);
				pthread_mutex_lock(&(curr->fdMutex));
			}
			pthread_mutex_unlock(&(prev->fdMutex));
			prev = curr;
		}

		pthread_rwlock_unlock(&cLstLock);

	}else{ // dispatch to the parent

		printf("Send to parent\n");
		
		if(sendMsg(pfd, request, size) == -1){ //Father is dead
			printf("Parent DEAD noticed by child??\n");
			if(pfd > 0)close(pfd);
			pthread_rwlock_wrlock(&parentLock);
			printf("Parent = -1 ..................\n");
			parent_fd = -1;
			pthread_rwlock_unlock(&parentLock);
		}else
			printf("send to par done\n");
	}
}

int waitRegion(int region, char ** content){
	int size;
	pthread_mutex_lock(&waitlock[region]);
	pthread_cond_wait(&w[region], &waitlock[region]);
	pthread_mutex_unlock(&waitlock[region]);

	pthread_rwlock_rdlock(&cliplock[region]);
	size = clip[region].size;
	*content = mallocV(size, ": wait regionutils.o");
	memcpy(*content, clip[region].data, size);
	pthread_rwlock_unlock(&cliplock[region]);
	
	return size;
}

int handleRequest(int size, char* request, int fd){
	int region = request[1]-'0', b_size; //LONG INT??
	char * buf = NULL;

	pthread_rwlock_rdlock(&parentLock);
	int pfd = parent_fd;
	pthread_rwlock_unlock(&parentLock);

	switch(request[0]){
		case 'C':

			buf = mallocV(size, ": copy tmp buf");
			memcpy(buf, request, size);
			
			if(pfd == -1 || fd == pfd){
				//local save
				//lock region 2 copy as reader
				pthread_rwlock_wrlock(&cliplock[region]);

				printf("entering\n");
				fflush(stdout);
				if(clip[region].data != NULL){
					printf("freeing %d\n", region);
					clip[region].size = 0;
					free(clip[region].data-2);
				}
				//  0 1 2 3 4 5 6
				// -C|2|A|B|C|D|E|\0
				// /   /
				clip[region].size = size-2;
				clip[region].data = (size-2 == 0)?NULL:&request[2];

				printf("[%d]-[", region);
				fflush(stdout);
				write(1, clip[region].data, clip[region].size);
				printf("]-%d\n", clip[region].size );
				fflush(stdout);
				//unlock region
				//sleep(5);
				printf("leaving\n");
				fflush(stdout);
				pthread_cond_broadcast(&w[region]);

				pthread_rwlock_rdlock(&cLstLock);

				pthread_rwlock_unlock(&cliplock[region]);

				if(size-2 == 0) free(request);
			}

			broadcastReq(size, buf, fd); // dois casos
			free(buf);
			break;

		case 'P':
			pthread_rwlock_rdlock(&cliplock[region]);
			printf("on the write\n");
			b_size = clip[region].size;
			buf = mallocV(b_size, ": paste tmp buf");
			memcpy(buf, clip[region].data, b_size);
			//sleep(5);
			printf("leaving write\n");
			pthread_rwlock_unlock(&cliplock[region]);
			
			if(sendMsg(fd, buf, b_size) == -1){
				close(fd);
				free(buf);
				return -1;
			}
			free(buf);
			break;

		case 'W':
			b_size = waitRegion(region, &buf);
			if(sendMsg(fd, buf, b_size) == -1){
				free(buf);
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

void attend_parent(void * arg){
	int n = 0, i;
	char * msg = NULL;
	pthread_rwlock_rdlock(&parentLock);
	int pdf = parent_fd;
	pthread_rwlock_unlock(&parentLock);

	while ((n = recvMsg(pdf, (void**)&msg)) > 0){
		printf("\n[from parent]>>>>>>>>>>>>>>>>>>>>>\n");
		printf("|");
		for ( i = 0; i < n; i++)
		{
			printf("%c|", msg[i]);
		}
		printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

		if( n >= 2 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			if( handleRequest(n, msg, pdf) == -1 ){
				free(msg);
				//close(parent_fd); ................................................
				return;
			}	
		}else{
			free(msg);
			break;
		}

		pthread_rwlock_rdlock(&parentLock);
		pdf = parent_fd;
		pthread_rwlock_unlock(&parentLock);
	}
	printf("[attend_parent] stop handle parent\n");
	pthread_rwlock_wrlock(&parentLock);
	close(parent_fd);
	parent_fd = -1;
	pthread_rwlock_unlock(&parentLock);

}

void attend_app(void * arg){
	int cfd = *(int*)arg, n, i;

	char * msg = NULL;

	while ((n = recvMsg(cfd, (void**)&msg)) > 0){
		printf("\n>>>>>>>>>>>>>>>>>>>>>\n");
		printf("\n|");
		for ( i = 0; i < n; i++)
		{
			printf("%c|", msg[i]);
		}
		printf("\n");

		sleep(5);
		if( n >= 2 && (msg[0] == 'C' || msg[0] == 'P' || msg[0] == 'W') && msg[1] <= '9' && msg[1] >= '0'){
			if( handleRequest(n, msg, cfd) == -1 ){
				//TODO
			}
		}else{
			free(msg);
			break;
		}		
	}
	
	printClipboard();
	close(cfd);
}

void attend_clip(void * arg){
	Elm * elm = (Elm *)arg;
	int i, b_size, r, n;
	char * buf = NULL;

	//lock mutex fd
	for(i = 0; i<=9; i++){
		pthread_rwlock_rdlock(&cliplock[i]);			
		b_size = clip[i].size+2;
		if(b_size > 2){
			buf = mallocV(b_size, ": attend sync" );
			memcpy(buf, clip[i].data-2, b_size);
		}
		pthread_rwlock_unlock(&cliplock[i]);

		if(b_size > 2){	
			pthread_mutex_lock(&(elm->fdMutex));
			r = sendMsg(elm->fd, buf, b_size);
			pthread_mutex_unlock(&(elm->fdMutex));
				
			if(r == -1){
				printf("Error attending clip\n");
				close(elm->fd);
				pthread_rwlock_wrlock(&cLstLock);
				elm->fd = -2;
				pthread_rwlock_unlock(&cLstLock);
				free(buf);
				return; //Thread will disapear
			}
			free(buf);
		}
	}

	buf = mallocV(1, ": finalizing sync");
	buf[0]='S'; // END SYNC

	pthread_mutex_lock(&(elm->fdMutex));
	r = sendMsg(elm->fd, buf, 1);
	pthread_mutex_unlock(&(elm->fdMutex));

	printf("END SYNC\n");
	
	free(buf);

	char * msg = NULL;

	while ((n = recvMsg(elm->fd, (void**)&msg)) > 0){

		if( n >= 2 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			printf("[attend_clip][fd:%d] C|",elm->fd );
			if( handleRequest(n, msg, elm->fd) == -1 ){
				//TODO
			}
		}else{
			printf("[thread attend_clip fd:%d EXIT\n", elm->fd );
			fflush(stdout);
			if(msg != NULL) free(msg);
			break;
		}	
	}
}

int reuseNode(Item i){
	Elm *e = (Elm*)i;
	printf("reuse fd:%d\n", e->fd);
	if(e->fd == -2){
		printf("reuse\n");
		return 1;
	}
	else
		return 0;
}

void listenChildren(){
	int fd = setupParentListener();
	if(fd == -1){
		printf("Will not be able to accept other clipboards\n");
		return;
	}
	int clip_fd;
	struct sockaddr_in cli_addr;
	socklen_t cli_addrlen;
	memset(&cli_addr, 0, sizeof(struct sockaddr_in));
	memset(&cli_addrlen, 0, sizeof(socklen_t));

	pthread_t th;
	Elm * newElm=NULL;
	if(pthread_rwlock_init(&cLstLock, NULL) == -1){
		printf("couldn't create List Lock\n");
		exit(EXIT_FAILURE);
	}

	childsList = initList();

	if(fd != -1){
		while(1){
			clip_fd = accept(fd, (struct sockaddr *) &cli_addr, &cli_addrlen);
			if(clip_fd == -1){
				printf("Couldn't accept clipboard connection: %s\n", strerror(errno));
			}else{
				printf("clipboard just connected [fd:%d]\n", clip_fd);
				fflush(stdout);

				newElm = mallocV(sizeof(Elm), ": new list element");
				newElm->fd = clip_fd;
				pthread_mutex_init(&(newElm->fdMutex), NULL);

				pthread_rwlock_wrlock(&cLstLock);
				childsList=new(childsList, newElm, reuseNode);
				pthread_rwlock_unlock(&cLstLock);

				pthread_create(&th, NULL, (void*)attend_clip, newElm);
			}

		}
	}
}

void initialSync(int *fd){
	int n, region;
	char * msg = NULL;
	while(1){
		n = recvMsg(*fd, (void**)&msg);
		printf("[S]>[");
		fflush(stdout);
		write(1, msg, n);
		printf("]\n");
		fflush(stdout);
		if( n >= 2 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			region = msg[1] - '0';
			if(clip[region].data != NULL){
				printf("[S]> freeing %d\n", region);
				clip[region].size = 0;
				free(clip[region].data-2);
			}
			//  0 1 2 3 4 5 6
			// -C|2|A|B|C|D|E|\0
			// /   /
			clip[region].size = n-2;
			clip[region].data = (n-2 == 0)?NULL:&msg[2];

		}else if(n == 1 && msg[0] == 'S'){
			free(msg);
			break;
		}else{
			close(*fd);
			*fd = -1;
			free(msg);
			return;
		}
	}
}

int main(int argc, char *argv[]){
	
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	memset(&cli_addr, 0, sizeof(struct sockaddr_un));
	memset(&cli_addrlen, 0, sizeof(socklen_t));

	int i, sfd;

	pthread_t threads;
	pthread_t th;

	struct sigaction act_INT;
	act_INT.sa_handler = sigint_handler;
	sigemptyset(&act_INT.sa_mask);
	act_INT.sa_flags=0;
	sigaction(SIGINT, &act_INT, NULL);

	void (*old_handler)(int);
	if( (old_handler = signal(SIGPIPE, SIG_IGN)) == SIG_ERR) {
		printf("Couldn't ignore SIGPIPE\n");
		exit(EXIT_FAILURE);
	}


	for (i = 0; i < 10; ++i){
		clip[i].data=NULL;
		clip[i].size=0;
		if(pthread_rwlock_init(&cliplock[i], NULL) == -1){
			printf("couldn't create locks\n");
			exit(EXIT_FAILURE);
		}
	}

	
	if(argc == 4){
	    if(strcmp(argv[1], "-c") == 0){
		        if((parent_fd = connect2parent(argv[2], argv[3])) != -1){
		        	if(pthread_rwlock_init(&parentLock, NULL) == -1){
						printf("couldn't create parent fd lock\n");
						exit(EXIT_FAILURE);
					}
		        	initialSync(&parent_fd);
					if(parent_fd != -1)pthread_create(&th, NULL, (void*)attend_parent, NULL);
		        }
	    }
	}

	if(parent_fd == -1) printf("On single mode\n");

	pthread_create(&th, NULL, (void*)listenChildren, NULL);

	sfd = createListenerUnix();

	while(1){

		int cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(cfd == -1){
			printf("Couldn't accept client connection: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		printf("App connected\n");

		// Create accept Clipboard threads
		if(pthread_create(&threads, NULL, (void*)attend_app, (void*)&cfd) == -1){
			printf("Couldn't create app thread: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		
	}

	close(sfd);
	if(parent_fd == -1) close(parent_fd);
	exit(0);
	
}
