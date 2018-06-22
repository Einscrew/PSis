#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include "clipboard.h"

 
int main(){
	char file_name[100];

	/*
	_________u___g___o
	octal____6___4___6
	binario_110_100_110
	what____rwx_rwx_rwx
	*/

	sprintf(file_name, "./%s", OUTBOUND_FIFO);
	unlink(file_name);
	if(mkfifo(file_name, 0666)==-1){
		printf("Error creating out fifo\n");
		exit(-1);
	}
	int fifo_out = open(file_name, O_RDWR);
	if(fifo_out == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}
	
	
	
	sprintf(file_name, "./%s", INBOUND_FIFO);
	unlink(file_name);
	if(mkfifo(file_name, 0666)==-1){
		printf("Error creating in fifo\n");
		exit(-1);
	}
	int fifo_in = open(file_name, O_RDWR);
	if(fifo_in == -1){
		printf("Error opening in fifo\n");
		exit(-1);
	}

	//criar FIFOS
	
	//abrir FIFOS
	int len_data;
	char clip[10][10];

	Element inbox;
	while(1){
		memset(&inbox, 0, sizeof(Element));
		printf(".\n");
		read(fifo_in, &inbox, sizeof(Element));
		printf("received %c\n", inbox.type);

		if(inbox.type == 'C'){
			memcpy(clip[(int)inbox.region], inbox.content, sizeof(inbox.content));
			printf("copying: %s ---\n         %c\n         %d\n", inbox.content , inbox.type, inbox.region);

		}else if(inbox.type == 'P'){
			write(fifo_out, &clip[(int)inbox.region], sizeof(clip[(int)inbox.region]));
			printf("pasted: %s\n", clip[(int)inbox.region]);

		}

	}
		
	exit(0);
	
}
