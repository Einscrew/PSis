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

#include "clipboard.h"
#include "connection.h"

#define FALSE 0
#define TRUE 1

char * clip[10];
int sizes[10];
 
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



void handleRequest(int size, char* request, int cfd, int sync, int bfd){
	int region = request[1]-'0';
	switch(request[0]){
		case 'C':
			//local save
			
			if(clip[region] != NULL){
				printf("freeing %d\n", region);
				sizes[region] = 0;
				free(clip[region]-2);
			}
			clip[region] = &request[2];
			sizes[region] = size-2;

			printf("[%d]-[%s]\n", region, clip[region] );

			//remote save
			if(sync){
				//reply(elmBuf, bfd);
			}
			break;

		case 'P':
			sendMsg(cfd, clip[region], sizes[region]);
			
			break;

		default:
			printf("Unknown instruction\n");
			break;
	}

}

int main(int argc, char *argv[]){
	struct sockaddr_un cli_addr;
	socklen_t cli_addrlen;

	//FLAGS
	int sync = FALSE;

		int i, n,sfd, cfd, bfd, size, index=0, left2cpy=sizeof(Element);
	char * bufFull =(char*) malloc(sizeof(Element));
	char buf[1000], opt;
	char * msg;

	Element elmBuf;

	for (i = 0; i < 10; ++i){
		clip[i]=NULL;
		sizes[i]=0;
	}
	
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

	
	sfd = createListenerUnix();

	//accept()
	while(1){
		cfd = accept(sfd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if(cfd == -1){
			printf("Couldn't accept client connection: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		index=0;
		while ((n = recvMsg(cfd, &msg)) > 0){
			printf("\n>>>>>>>>>>>>>>>>>>>>>\n");
			printf("\n|");
			for (int i = 0; i < n; ++i)
			{
				printf("%c|", msg[i]);
			}
			printf("\n");
			handleRequest(n, msg, cfd, sync, bfd);
		}
		//CHECK IF size buf > size(Element) --------------------------_>>>>>>>>>>>>>>>>>>>>>>>>>>>
		// while((size = recv(cfd, &(buf), 1000,0)) > 0){
		// 	//printf("Read:%d\t index:%d\n", size, index);

		// 	/*
		// 	 __4___3__4,___
		// 	|1234|567|8,123|
		// 	*/	
		// 	if(index + size >= sizeof(Element)){
		// 		//printf("size[%d] index[%d]\n", size, index);
		// 		printf("Saving...\n");
		// 		left2cpy = sizeof(Element) - index;
		// 		memcpy(bufFull+index, buf, left2cpy);
				
				
		// 		handleRequest(&elmBuf, bufFull, cfd, sync, bfd);
		// 		printf("type: %s ---\n         %c\n         %d\n", elmBuf.content , elmBuf.type, elmBuf.region);

		// 		if(index + size > sizeof(Element)){
		// 			index =  size - left2cpy;
		// 			memcpy(bufFull, buf+(left2cpy), index);						
		// 		}
		// 		else{
		// 			index = 0;
		// 		}

		// 	}else{
		// 		memcpy(bufFull+index, buf, size);
		// 		index+=size;
		// 	}

		// }

		printf("-----------------------------------\n");
		for ( i = 0; i < 10; ++i)
		{
			printf("[%d]-[%s]\n",i, clip[i] );
		}

		close(cfd);
	}

	free(bufFull);
	close(sfd);	
	exit(0);
	
}
