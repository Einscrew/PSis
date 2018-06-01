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

/*********************************************************************************************
*    This function sends a message to the given file descriptor, but first         		     *
*   checks the size of what it will send 									       		     *
* 															 				       		     *
* Parameters: to - file descriptor to which the message will be sent                 	     *
*             buf - buffer that will hold the message       				          		 *
* 			  size - size of the buffer 									         	     *
*                                        			  						           	     *
* Returns: -1 in case of error on the write() function      						         *
*		   0 if the size of the given buffer isn't enough to the content that will be send   *
*          the size of what will be sent otherwise               		        	  		 *
*********************************************************************************************/

int sendMsg(int to, void *buf, int size){

	int written = 0, n = 0, miss = sizeof(int);
	
	/* Counts the size of the message that will be send */
	while((n = write(to, &size+written, miss)) > 0){
		miss -= n;
		written += n;
	}

	/* Returns -1 in case of error */
	if(n < 0 ) return -1;	

	written = 0;

	/* If the size of the buffer is enough writes to it */
	while (written < size){
		if((n = write(to, buf+written, size-written)) < 0 ){
			return -1;
		}

		written += n;
	}

	return written;
}

/*****************************************************************************************
*    This function receives a message from the given file descriptor, but first checks   *
*   the size of what it will receive in order to allocate the memory needed				 *
* 															 				             *
* Parameters: from - file descriptor from which the message will be received             *
*             buf - buffer that will hold the message       				             * 	
*                                        			  						             *
* Returns: -1 in case of error on the recv() function or if the buffer is null	  		 *
*          the size of what was read          		                                     *
*****************************************************************************************/

int recvMsg(int from, void **buf){

	int read = 0, n = 1, miss = sizeof(int), size = 0;
	char *s = mallocV(sizeof(int), "recv tmp variables");

	/* If the buffer is null, returns -1 */
	if(buf == NULL){
		return -1;
	}
	
	while(miss > 0){

		/* Returns -1 in case of error  on recv() function */ 
		if((n = recv(from, &s[read], miss,0)) <= 0){
			free(s);
			return -1;
		}
		
		miss -= n;
	    read += n;
	}

	memcpy(&size, s, sizeof(int));

	if(size > 0 && size < INT_MAX){
		/* Allocates memory to the buffer that will hold the message */
		*buf = mallocV(size, "recv tmp variables");
		miss = size;
		read = 0;
		
		/* Reads the specified size */
		while (read < size){

			/* Test if the recv() function returns error */
			if((n=recv(from, *buf+read, miss, 0)) <= 0){
				
				read = -1;
				free(*buf);
				*buf = NULL;
				break;
			}

			/* Updates the size of what will receive */
			miss -= n;
			read += n;
		}

	}else if(size == 0){
		*buf = NULL;
		read = 0;
	}else{
		*buf = NULL;
		read = -1;
	}
	
	/* Frees memory */
	free(s);

	/* Returns the size of what was received */
	return read;
}


/*****************************************************************************************
*    This function creates a socket UNIX in a path that has the name of the process id	 *
*   and prepares it to listen new connections from applications   					     *
* 															 				             *					      
* Returns: the file descriptor of the socket UNIX that will accept new connections from  *
*         applications	  								               				     *          		                                     
*****************************************************************************************/

int createListenerUnix(){

	int sfd = 0;
	char pathSocket[108];

	sprintf(pathSocket, "./%d", getpid());

	/* Creates a new directory for the UNIX socket */
	if(mkdir(pathSocket, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1){
		printf("[%d]Couldn't creat socket folder: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	sprintf(pathSocket, "./%d/%s", getpid(), CLIPBOARD_SOCKET);
	struct sockaddr_un my_addr;

	unlink(pathSocket);

	/* Creates a new socket UNIX */
	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("[%d]Couldn't open socket: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}

	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, pathSocket, sizeof(my_addr.sun_path)-1);

	/* Binds the socket */
	if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) == -1){
		printf("[%d]Couldn't bind UNIX socket: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}

	/* Listen new connections from applications */
	if(listen(sfd, 10) == -1){
		printf("[%d]Couldn't listen: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
		close(sfd);
	}

	/* The file descriptor of the UNIX socket */
	return sfd;
}

/**************************************************************************************************
*    This function creates a stream socket that will attend new connetions from remote clipboards *
* 															 				                      *					      
* Returns: the file descriptor of the stream socket	that will attend new connections from remote  *
*		  clipboards 								              							      *          		                                     
**************************************************************************************************/

int setupParentListener(){

	int yes = 0, sfd = -1, portM = 0, portm = 0, port = 0;
	struct sockaddr_in my_addr;
	char buf[6];

	/* Creates a new stream socket */
	if((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("[%d]Couldn't create socket: %s\n", getpid(), strerror(errno));
		return -1;
	}
	
	if((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1){
		printf("[%d]Couldn't set socket: %s\n", getpid(), strerror(errno));
		close(sfd);
		return -1;
	}

	/* Opens a file that has the inital and final ports available */
	FILE *ports = NULL;
	if((ports=fopen("ports.txt", "r" )) == NULL){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	/* Reads the maximum port number availble */
	char * r = fgets(buf, 6, ports);
	if(r == NULL || (sscanf(buf, "%d", &portM)) <= 0){
		/* Assigns a default maximum port number */ 
		portM = 8100;
	}

	/* Reads the minimum port number available */
	r = fgets(buf, 6, ports);
	if(r == NULL || (sscanf(buf, "%d", &portm)) <= 0){
		/* Assigns a default minimum port number */
		portm = 8000;
	}

	/* Close file */
	if((fclose(ports)) != 0){
		printf("Error closing file!\n");
	}

	if(portM < portm){
		port = portM;
		portM = portm;
		portm = port;
	}

	/* Starts by trying the minimum number port avalibale */
	port = portm;

	while(1){
		
		/* Protects against the maximum number avalable to the ports */
		if(port >= 65536 || port > portM){
			printf("[%d]Couldn't bind AF_INET socket\n", getpid());
			exit(EXIT_FAILURE);
		}

		memset(&my_addr, 0, sizeof(struct sockaddr_in));
		my_addr.sin_family = AF_INET;
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		my_addr.sin_port = htons(port);

		/* Checks if can bind the socket to that current port */
		if(bind(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in)) == 0){
			printf("[%d]Port: %d\n", getpid(), port);
			break;
		}

		/* Increase the port number in case of the current port isn't available */
		port++;
	}
	
	/* Listens new connections from remote clipboards */
	if (listen(sfd, 1) == -1){
		printf("[%d]Couldn't listen: %s\n", getpid(), strerror(errno));
		close(sfd);
		return -1;
	}

	/* Returns the file descriptor that will accept new connections from remote clipboards */
	return sfd;
}

/**************************************************************************
*    This function connects to the existent clipboard given by the user   *
* 															 		      *					      
* Returns: the file descriptor of the stream socket	that allows the       *
*         communication to the existent clipboard					      *          		                                     
**************************************************************************/

int connect2parent(char * argip, char * argport){

	int bfd = 0, port = atoi(argport);
	struct sockaddr_in my_addr;

	/* Creates a stream socket to communicate to the clipboard given by the user */
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
	
	/* Connects to the existent clipboard */
	if(connect(bfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_in))== -1){
		printf("[%d][Couldn't connect to parent clipboard: %s]\n", getpid(), strerror(errno));
		return -1;
	}

	printf("[%d][Success]\n", getpid());

	/* Returns the file descriptor that allows the clipboard to communicate to its parent clipboard */
	return bfd;
}
