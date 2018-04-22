#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "clipboard.h"

int clipboard_connect(char * clipboard_dir){
	char fifo_name[100];
	
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);

	struct sockaddr {
	    sa_family_t sa_family;
	    char        sa_data[14];
	}

	bind(fd, );
	
	sprintf(fifo_name, "%s%s", clipboard_dir, INBOUND_FIFO);
	int fifo_send = open(fifo_name, O_WRONLY);
	sprintf(fifo_name, "%s%s", clipboard_dir, OUTBOUND_FIFO);
	int fifo_recv = open(fifo_name, O_RDONLY);
	return fifo_send;

}


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


/*
 sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr,
            sizeof(struct sockaddr_un)) == -1)
        handle_error("bind");

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");

    // Now we can accept incoming connections one\
       at a time using accept(2) 

    peer_addr_size = sizeof(struct sockaddr_un);
    cfd = accept(sfd, (struct sockaddr *) &peer_addr,
                 &peer_addr_size);
    if (cfd == -1)
        handle_error("accept");

    // Code to deal with incoming connection(s)... 

    // When no longer required, the socket pathname, MY_SOCK_PATH\
       should be deleted using unlink(2) or remove(3) 
}
*/