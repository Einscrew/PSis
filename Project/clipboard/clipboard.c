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

#include "../lib/clipboard.h"
#include "../lib/connection.h"
#include "../lib/utils.h"
#include "../lib/list.h"

#define VERBOSE 0

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

int pid = -1;

//---[ RWLock to ensure syncronization inside clipboard ]---
pthread_rwlock_t cliplock[10];
pthread_mutex_t waitlock[10];
pthread_cond_t w [10] = {PTHREAD_COND_INITIALIZER};
Clip clip[10];

//---[ RWLock to ensure serial communication with parent clipboard ]---
pthread_rwlock_t parentLock;
int parent_fd = -1;

int afd = -1;

pthread_rwlock_t cLstLock;
t_list* childsList;

t_list* appsList;

void freeItem(Item i){
	free(i);
}
void closeChild(Item i){
	Elm *e = (Elm*)i;
	close(e->fd);
}

/*****************************************************************************************
*                     This function closes cliboard file descriptors                     *
* Returns: void                                                                          *          		                                     
*****************************************************************************************/
void closeChildren(){
	if(pthread_rwlock_wrlock(&cLstLock)!= 0){
		fprintf(stderr, "[%d]Error wrlock clipboard list\n", pid);
	}
	closeFreeList(childsList, freeItem, closeChild);
	if(pthread_rwlock_unlock(&cLstLock)!= 0){
		fprintf(stderr, "[%d]Error unlock clipboard list\n", pid);
	}
}

void closeApp(Item i){
	int e = *(int*)i;
	close(e);
}

void closeApps(){	
	closeFreeList(appsList, freeItem, closeApp);
}

/*****************************************************************************************
*                        This function frees the local clipboard	                     *
* Returns: void                                                                          *          		                                     
*****************************************************************************************/
void freeClipboard(){
	int region = 0;
	for ( region = 0; region < 10; ++region)
	{
		pthread_rwlock_wrlock(&cliplock[region]);
		if(clip[region].data != NULL){
			printf("[%d]freeing %d\n", region, pid);
			clip[region].size = 0;
			free(clip[region].data-2);
			clip[region].data = NULL;
		}
		pthread_rwlock_unlock(&cliplock[region]);
	}
}


/*****************************************************************************************
*                This function frees the memory, closes file descriptors                 *
* 					   unlinks sockets and removes directories                           *
*																						 *
* Parameters: n - interruption code                                                      *
*																						 *
* Returns: void                                                                          *          		                                     
*****************************************************************************************/
void sigint_handler(int n){
	char pathSocket[108];
	close(parent_fd);
	printf("[%d]Clipboard terminating\n", pid);
	close(afd);
	closeChildren();
	closeApps();
	freeClipboard();
	
	sprintf(pathSocket, "./%d/%s", getpid(), CLIPBOARD_SOCKET);
	if(unlink(pathSocket) == -1){
		fprintf(stderr, "[%d]Error unlink AF_UNIX socket: %s\n", pid, strerror(errno));
	}
	
	sprintf(pathSocket, "./%d", getpid());
	if(rmdir(pathSocket) == -1){
		fprintf(stderr, "[%d]Error removing process directory: %s\n", pid, strerror(errno));
	}
	exit(EXIT_SUCCESS);
}

/*****************************************************************************************
*                                 Prints local clipboard                                 *
*																						 *
* Returns: void                                                                          *          		                                     
*****************************************************************************************/
void printClipboard(){
	int i = 0;
	printf("[%d]-----------------------------------\n", pid);
	for ( i = 0; i < 10; ++i)
	{
		pthread_rwlock_rdlock(&cliplock[i]);
		printf("[%d]>[%d]-[%.*s]-[%d]\n", pid, i, clip[i].size, clip[i].data, clip[i].size);
		pthread_rwlock_unlock(&cliplock[i]);
	}
}

/*****************************************************************************************
*                    Changes a certain region with the new request	                     *
*                           with respect to thread serialization                         *
*																						 *
* Parameters: size - request size                                                        *
*             request - request content                                                  *
*																						 *
* Returns: void                                                                          *
*****************************************************************************************/
void changeRegion(int size, char * request){
	int region = request[1]-'0';
	
	//lock da regiao
	if(pthread_rwlock_wrlock(&cliplock[region]) != 0){
		fprintf(stderr, "[%d]Error wrlock @ region: %d\n", pid, region);
	}

	printf("[%d]entering\n", pid);
	if(clip[region].data != NULL){
		printf("[%d]freeing %d\n", pid, region);
		clip[region].size = 0;
		free(clip[region].data-2);
	}
	//  0 1 2 3 4 5 6
	// -C|2|A|B|C|D|E|\0
	// /   /
	clip[region].size = size-2;
	clip[region].data = (size-2 == 0)?NULL:&request[2];

	printf("[%d][%d]-[%.*s]-[%d]\n", pid , region, clip[region].size, clip[region].data, clip[region].size);
	//unlock region
	//sleep(5);
	printf("[%d]leaving\n", pid);
	fflush(stdout);

	// Ensure clipboard list
	pthread_cond_broadcast(&w[region]);

	if(pthread_rwlock_unlock(&cliplock[region]) != 0){
		fprintf(stderr, "[%d]Error rw_unlock @ region: %d\n", pid, region);
	}

	if(size-2 == 0) free(request); //<----------------------------------
}

/*****************************************************************************************
*           Forward the request either to the parent clipboard or to the childs          *
*              serializing each write access to the destiny file descriptor              *
*																						 *
* Parameters: size - request size                                                        *
*             request - request content                                                  *
*             fd - file descriptor from which the request was read                       *
*																						 *
* Returns: void                                                                          *
*****************************************************************************************/
void broadcastReq(int size, char * request, int fd){
	int pfd = -1;
	
	if(pthread_rwlock_rdlock(&parentLock) != 0){
		fprintf(stderr, "[%d]Error rdlock parent_fd\n", pid);
	}
	pfd = parent_fd;	
	if(pthread_rwlock_unlock(&parentLock)!= 0){
		fprintf(stderr, "[%d]Error rw_unlock parent_fd\n", pid);
	}



	if(pfd == -1 || fd == pfd){ // dispatch to all children

		Elm * curr = NULL, *prev = NULL;
		t_list* aux = NULL;

		if(pthread_rwlock_rdlock(&cLstLock)!= 0){
			fprintf(stderr, "[%d]Error rdlock clipboard List:\n", pid );
		}
		aux = childsList;
		if(aux == NULL){	
			printf("[%d] NO CHILDS TO ANSWER\n", pid);
			changeRegion(size, request);
		}
		else{
			prev = (Elm *)getItem(aux);
			if(pthread_mutex_lock(&(prev->fdMutex))!= 0){
				fprintf(stderr, "[%d]Error mutex lock clipboard list:\n", pid );
			}		
			curr = prev;
		}
		
		while(aux != NULL){
			
			printf("[%d]broadcast to %d result: ", pid, curr->fd );fflush(stdout);

			if(curr->fd > 0){

				if(sendMsg(curr->fd, request, size) == -1){
					printf("[%d] X\n", pid);fflush(stdout);
					close(curr->fd);
					curr->fd = -2;
				}else{
					printf("[%d] OK\n", pid);fflush(stdout);
				}
			}
			aux = next(aux);
			if(aux != NULL){
				curr = (Elm *)getItem(aux);
				if(pthread_mutex_lock(&(curr->fdMutex))!= 0){
					fprintf(stderr, "[%d]Error locking clipboard List mutex\n", pid);
				}		
			}else{
				changeRegion(size, request);
			}

			if(pthread_mutex_unlock(&(prev->fdMutex))!= 0){
				fprintf(stderr, "[%d]Error unlock clipboard list mutex\n", pid);
			}		
			prev = curr;
		}

		if(pthread_rwlock_unlock(&cLstLock)!= 0){
			fprintf(stderr, "[%d]Error unlock clipboard list\n", pid);
		}		

	}else{ // dispatch to the parent
		
		if(pthread_rwlock_wrlock(&parentLock)!= 0){
			fprintf(stderr, "[%d]Error wrlock clipboard List\n", pid);
		}		
		int s = sendMsg(parent_fd, request, size);
		free(request);
		
		if(pthread_rwlock_unlock(&parentLock)!= 0){
			fprintf(stderr, "[%d]Error rdlock clipboard List\n", pid);
		}		
		
		if( s == -1){ //Father is dead
			printf("[%d]Parent DEAD noticed by child??\n", pid);
			if(pfd > 0)close(pfd);
			if(pthread_rwlock_wrlock(&parentLock)!= 0){
				fprintf(stderr, "[%d]Error wrlock parent_fd\n", pid);
			}
			parent_fd = -1;
			if(pthread_rwlock_unlock(&parentLock)!= 0){
				fprintf(stderr, "[%d]Error unlock parent_fd\n", pid);
			}
		}else if(s == 0){
			printf("[%d] sent  0 to parent\n", pid);
		}else{
			printf("[%d] sent %d to parent\n", pid, s);
		}
	}	
}

/*****************************************************************************************
*        Makes the attend_app thread wait for a change on a given region in order        *
*                             to prossed the paste procedure                             *
*																						 *
* Parameters: region - region on which the wait will be performed                        *
*             content - inout pointer on which the content will be saved                 *
*																						 *
* Returns: void                                                                          *
*****************************************************************************************/
int waitRegion(int region, char ** content){
	int size;
	if(pthread_mutex_lock(&waitlock[region])!= 0){
		fprintf(stderr, "[%d]Error lock wait mutex\n", pid);
	}
	if(pthread_cond_wait(&w[region], &waitlock[region])!= 0){
		fprintf(stderr, "[%d]Error cond_wait\n", pid);
	}
	if(pthread_mutex_unlock(&waitlock[region])!= 0){
		fprintf(stderr, "[%d]Error unlock wait mutex\n", pid);
	}


	if(pthread_rwlock_rdlock(&cliplock[region])!= 0){
		fprintf(stderr, "[%d]Error rdlock region\n", pid);
	}
	size = clip[region].size;
	*content = mallocV(size, ": wait regionutils.o");
	memcpy(*content, clip[region].data, size);
	if(pthread_rwlock_unlock(&cliplock[region])!= 0){
		fprintf(stderr, "[%d]Error unlock region\n", pid);
	}
	
	return size;
}

/*****************************************************************************************
*            Handles a given request either received by other clipboard or app           *
*																						 *
* Parameters: size - request size in bytes                                               *
*             request - request content                                                  *
*             fd - file descriptor on which the receive was made                         *
*																						 *
* Returns: return if the request was well processed (0) or not (-1)                      *
*****************************************************************************************/
int handleRequest(int size, char* request, int fd){
	int region = request[1]-'0', b_size; //LONG INT??
	char * buf = NULL;

	switch(request[0]){
		case 'C':

			broadcastReq(size, request, fd); // dois casos
			break;

		case 'P':
			free(request);
			if(pthread_rwlock_rdlock(&cliplock[region])!= 0){
				fprintf(stderr, "[%d]Error rdlock region\n", pid);
			}
			printf("[%d]on the write\n", pid);
			b_size = clip[region].size;
			buf = mallocV(b_size, ": paste tmp buf");
			memcpy(buf, clip[region].data, b_size);
			//sleep(5);
			printf("[%d]leaving write\n", pid);
			if(pthread_rwlock_unlock(&cliplock[region])!= 0){
				fprintf(stderr, "[%d]Error unlock region\n", pid);
			}
			
			if(sendMsg(fd, buf, b_size) == -1){
				close(fd);
				free(buf);
				return -1;
			}
			free(buf);
			break;

		case 'W':
			free(request);
			b_size = waitRegion(region, &buf);
			if(sendMsg(fd, buf, b_size) == -1){
				free(buf);
				close(fd);
				return -1;
			}
			free(buf);
			break;
		default:
			free(request);
			printf("[%d]Unknown instruction\n", pid);
			break;
	}
	return 0;
}

/*****************************************************************************************
*               Creates a thread to listen and handle the parent clipboard               *
*                                                                                        *
* Parameters: arg - thread arguments, nothing in this case                               *
*                                                                                        *
* Returns: void                                                                          *
*****************************************************************************************/
void attend_parent(void * arg){
	int n = 0, i;
	char * msg = NULL;
	if(pthread_rwlock_rdlock(&parentLock)!= 0){
		fprintf(stderr, "[%d]Error rdlock parent_fd\n", pid);
	}
	int pdf = parent_fd;
	if(pthread_rwlock_unlock(&parentLock)!= 0){
		fprintf(stderr, "[%d]Error unlock parent_fd\n", pid);
	}

	while ((n = recvMsg(pdf, (void**)&msg)) > 0){
		printf("[%d]\n[from parent]>>>>>>>>>>>>>>>>>>>>>\n", pid);
		printf("[%d]|", pid);
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

		if(pthread_rwlock_rdlock(&parentLock)!= 0){
			fprintf(stderr, "[%d]Error rdlock region\n", pid);
		}
		pdf = parent_fd;
		if(pthread_rwlock_unlock(&parentLock)!= 0){
			fprintf(stderr, "[%d]Error unlock region\n", pid);
		}
	}
	printf("[%d][attend_parent] stop handle parent\n", pid);
	if(pthread_rwlock_wrlock(&parentLock)!= 0){
		fprintf(stderr, "[%d]Error wrlock parent_fd\n", pid);
	}
	close(parent_fd);
	parent_fd = -1;
	if(pthread_rwlock_unlock(&parentLock)!= 0){
		fprintf(stderr, "[%d]Error unlock parent_fd\n", pid);
	}
}

/*****************************************************************************************
*                       Creates a thread to listen and handle a app                      *
*                                                                                        *
* Parameters: arg - thread arguments, a file descriptor in this case                     *
*                                                                                        *
* Returns: void                                                                          *
*                                                                                        *
*****************************************************************************************/
void attend_app(void * arg){
	int *cfd = (int*)arg, n;
	#ifdef VERBOSE
	int i = 0;
	#endif

	char * msg = NULL;

	while ((n = recvMsg(*cfd, (void**)&msg)) > 0){

		#ifdef VERBOSE
			printf("[%d]\n>>>>>>>>>>>>>>>>>>>>>\n", pid);
			printf("[%d]\n|", pid);
			for ( i = 0; i < n; i++)
			{
				printf("%c|", msg[i]);
			}
			printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		#endif

		//sleep(5);
		if( n >= 2 && (msg[0] == 'C' || msg[0] == 'P' || msg[0] == 'W') && msg[1] <= '9' && msg[1] >= '0'){
			if( handleRequest(n, msg, *cfd) == -1 ){
				free(msg);
				break;
			}
		}else{
			free(msg);
			break;
		}		
	}
	
	printClipboard();
	close(*cfd);
	return;
}

/*****************************************************************************************
*                    Creates a thread to listen and handle a clipboard*                  *
*                                                                                        *
* Parameters: arg - thread arguments, a pointer to Elm, that has a                       *
*                                      file descriptor and a mutex                       *
*                                                                                        *
* Returns: void                                                                          *
*                                                                                        *
*****************************************************************************************/
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
			pthread_mutex_lock(&(elm->fdMutex));
		}
		pthread_rwlock_unlock(&cliplock[i]);

		if(b_size > 2){	
			r = sendMsg(elm->fd, buf, b_size);
			pthread_mutex_unlock(&(elm->fdMutex));
				
			if(r == -1){
				fprintf(stderr, "[%d]Error attending clip\n", pid);
				close(elm->fd);
				pthread_mutex_lock(&(elm->fdMutex));
				elm->fd = -2;
				pthread_mutex_unlock(&(elm->fdMutex));
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

	printf("[%d]END SYNC\n", pid);
	
	free(buf);

	char * msg = NULL;

	while ((n = recvMsg(elm->fd, (void**)&msg)) > 0){

		if( n >= 2 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			printf("[%d][attend clip][fd:%d] C received\n", pid,elm->fd );
			if( handleRequest(n, msg, elm->fd) == -1 ){
				//TODO
			}
		}else{
			printf("[%d][attend_clip][fd:%d] Caused EXIT - not recognized msg\n", pid, elm->fd );
			fflush(stdout);
			if(msg != NULL) free(msg);
			break;
		}	
	}
	if(close(elm->fd) != 0){
		fprintf(stderr, " [%d]Error closing attend clip fd\n", pid);
	}
}

/*****************************************************************************************
*                 Function that determines if a list node must be reused                 *
*                                                                                        *
* Parameters: Item i - the given node of the list Elm or int in this case                *
*                                                                                        *
* Returns: (1) if the node can be reused                                                 *
*          (0) if not                                                                    *
*                                                                                        *
*****************************************************************************************/
int reuseNode(Item i){
	Elm *e = (Elm*)i;
	if(e->fd == -2){
		return 1;
	}
	else
		return 0;
}

/*****************************************************************************************
*        To be executed by the thread that will accept clipboards to be connected        *
*                                                                                        *
* Returns: void                                                                          *
*****************************************************************************************/
void listenChildren(){
	int fd = setupParentListener();
	if(fd == -1){
		fprintf(stderr, "[%d][child listener] Will not be able to accept other clipboards\n", pid);
		return;
	}
	int clip_fd;
	struct sockaddr_in cli_addr;
	socklen_t cli_addrlen;
	memset(&cli_addr, 0, sizeof(struct sockaddr_in));
	memset(&cli_addrlen, 0, sizeof(socklen_t));

	pthread_t th;
	Elm * newElm=NULL;
	if(pthread_rwlock_init(&cLstLock, NULL) != 0){
		fprintf(stderr, "[%d][child listener] Couldn't create list lock\n", pid);
		exit(EXIT_FAILURE);
	}

	childsList = initList();

	if(fd != -1){
		while(1){
			clip_fd = accept(fd, (struct sockaddr *) &cli_addr, &cli_addrlen);
			if(clip_fd == -1){
				fprintf(stderr, "[%d][child listener] Couldn't accept clipboard connection: %s\n", pid, strerror(errno));
			}else{
				printf("[%d]clipboard just connected [fd:%d]\n", pid, clip_fd);
				fflush(stdout);

				newElm = mallocV(sizeof(Elm), ": new list element");
				newElm->fd = clip_fd;
				pthread_mutex_init(&(newElm->fdMutex), NULL);

				if(pthread_rwlock_wrlock(&cLstLock)!= 0){
					fprintf(stderr, "[%d]Error wrlock clipboard list\n", pid);
				}
				childsList=new(childsList, newElm, reuseNode);
				if(pthread_rwlock_unlock(&cLstLock)!= 0){
					fprintf(stderr, "[%d]Error wrlock clipboard list\n", pid);
				}

				if(pthread_create(&th, NULL, (void*)attend_clip, newElm) == -1){
					fprintf(stderr, "[%d][child listener] Couldn't create attend clipboard thread: %s\n", pid, strerror(errno));
					if(pthread_rwlock_wrlock(&cLstLock)!= 0){
						fprintf(stderr, "[%d]Error wrlock clipboard list\n", pid);
					}
					close(newElm->fd);
					newElm->fd = -2;
					if(pthread_rwlock_unlock(&cLstLock)!= 0){
						fprintf(stderr, "[%d]Error unlock clipboard list\n", pid);
					}
				}

				if(pthread_detach(th)!= 0){
					fprintf(stderr, "[%d]Error couldn't detach attend_clip thread\n", pid);
				}
			}

		}
	}
}

/*****************************************************************************************
*                 Receives initial syncronization from the parent clipboard              *
*                                                                                        *
* Parameters: fd - parent clipboard file descriptor                                      *
*                                                                                        *
* Returns: void                                                                          *
*****************************************************************************************/
void initialSync(int *fd){
	int n, region;
	char * msg = NULL;
	while(1){
		n = recvMsg(*fd, (void**)&msg);

		printf("[%d][S]>[%.*s]-[%d]\n", pid, n, msg, n);
		if( n >= 2 && (msg[0] == 'C') && msg[1] <= '9' && msg[1] >= '0'){
			region = msg[1] - '0';
			if(clip[region].data != NULL){
				printf("[%d][S]> freeing %d\n", pid, region);
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

/*****************************************************************************************
* Tries to connect to the parent clipboard, if succeded syncronization will be performed *
*                                                                                        *
* Returns: void                                                                          *
*****************************************************************************************/
int main(int argc, char *argv[]){

	pid = getpid();
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	memset(&cli_addr, 0, sizeof(struct sockaddr_un));
	memset(&cli_addrlen, 0, sizeof(socklen_t));

	int i, afd, * cfd = NULL;

	pthread_t threads;

	struct sigaction act_INT;
	act_INT.sa_handler = sigint_handler;
	if(sigemptyset(&act_INT.sa_mask)!= 0){
		fprintf(stderr, "[%d]Error setting up sigaction\n", pid);
	}
	act_INT.sa_flags=0;
	if(sigaction(SIGINT, &act_INT, NULL)!= 0){
		fprintf(stderr, "[%d]Error setting up sigaction\n", pid);
	}

	act_INT.sa_handler = sigint_handler;
	if(sigemptyset(&act_INT.sa_mask)!= 0){
		fprintf(stderr, "[%d]Error setting up sigaction\n", pid);
	}
	act_INT.sa_flags=0;
	if(sigaction(SIGTERM, &act_INT, NULL)!= 0){
		fprintf(stderr, "[%d]Error setting up sigaction\n", pid);
	}

	void (*old_handler)(int);
	if( (old_handler = signal(SIGPIPE, SIG_IGN)) == SIG_ERR) {
		printf("[%d][main] Couldn't ignore SIGPIPE\n", pid);
		exit(EXIT_FAILURE);
	}


	for (i = 0; i < 10; ++i){
		clip[i].data=NULL;
		clip[i].size=0;
		if(pthread_rwlock_init(&cliplock[i], NULL) == -1){
			fprintf(stderr, "[main] Couldn't create region wrlocks\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if(argc == 4){
		if(strcmp(argv[1], "-c") == 0){
			if((parent_fd = connect2parent(argv[2], argv[3])) != -1){
				if(pthread_rwlock_init(&parentLock, NULL) == -1){
					fprintf(stderr, "[main] Couldn't create parent fd lock\n");
					exit(EXIT_FAILURE);
				}
				initialSync(&parent_fd);
				if(parent_fd != -1){
					if(pthread_create(&threads, NULL, (void*)attend_parent, NULL) != 0){
						fprintf(stderr, "[main] Error creating thread to attend parent clipboard\n");
						exit(EXIT_FAILURE);
					}
					if(pthread_detach(threads)!= 0){
						fprintf(stderr, "[%d]Error detaching listen parent thread\n", pid);
					}
				}
			}
		}
	}

	if(parent_fd == -1) printf("[%d]On single mode\n", pid);

	if(pthread_create(&threads, NULL, (void*)listenChildren, NULL) != 0){
		fprintf(stderr, "[main] Error creating thread to listen clipboards\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_detach(threads) != 0){
		fprintf(stderr, "[%d]Error detaching listen clipboardss	 thread\n", pid);
	}

	afd = createListenerUnix();

	appsList = initList();

	while(1){
		cfd= mallocV(sizeof(int), ": cfd");
		*cfd = accept(afd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(*cfd == -1){
			fprintf(stderr, "[main] Couldn't accept client connection: %s\n", strerror(errno));
			free(cfd);
			continue;
		}

		appsList = new(appsList, cfd, NULL); // add in the head
		// Create accept Clipboard threads
		if(pthread_create(&threads, NULL, (void*)attend_app, (void*)cfd) != 0){
			fprintf(stderr, "[main] Couldn't create app thread: %s\n", strerror(errno));
			close(*cfd);
			free(cfd);
		}
		if(pthread_detach(threads) != 0){
			fprintf(stderr, "[%d]Error detaching attend_app thread\n", pid);
		}
		
	}	
}
