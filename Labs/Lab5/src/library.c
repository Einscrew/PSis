#include "clipboard.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int clipboard_connect(char * clipboard_dir){
	char fifo_name[100];
	
	sprintf(fifo_name, "%s%s", clipboard_dir, INBOUND_FIFO);
	int fifo_send = open(fifo_name, O_WRONLY);
	sprintf(fifo_name, "%s%s", clipboard_dir, OUTBOUND_FIFO);
	int fifo_recv = open(fifo_name, O_RDONLY);
	return fifo_send;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	
	Element e;
	e.type = 'C';
	e.region = region;
	memcpy(e.content, buf, count);

	char * msg = malloc(sizeof(e));
	memcpy(msg, &e, sizeof(e));

	write(clipboard_id, msg, sizeof(msg));

	return 0;
}


int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	
	Element e;
	e.type = 'P';
	e.region = region;
	e.content[0] = '\0';

	char * msg = malloc(sizeof(Element));
	memcpy(msg, &e, sizeof(e));

	write(clipboard_id, msg, sizeof(msg));

	read(clipboard_id+1, buf, count);

	return 1;
}
