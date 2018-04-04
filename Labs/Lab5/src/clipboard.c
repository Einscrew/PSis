#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clipboard.h"

 
int main(){
	char file_name[100];
	
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
	char data[10];
	int len_data;
	while(1){
		printf(".\n");
		read(fifo_in, data, 10);
		printf("received %s\n", data);
		len_data = strlen(data);
		printf("sending value %d - legth %d\n", len_data, sizeof(len_data));
		write(fifo_out, &len_data, sizeof(len_data));
	}
		
	exit(0);
	
}
