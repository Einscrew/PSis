#define CLIPBOARD_SOCKET "clipSocket"
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>


int clipboard_connect(char * clipboard_dir){
    int sfd;
    struct sockaddr_un my_addr;
    char pathSocket[108];
    sprintf(pathSocket, "./%s", CLIPBOARD_SOCKET);

    if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
        printf("Couldn't open socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }


    memset(&my_addr, 0, sizeof(struct sockaddr_un));
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, pathSocket, sizeof(my_addr.sun_path)-1);

    if(connect(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un))== -1){
        printf("Couldn't open socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    return sfd;
}

//TODO: if n == 0 what??
int sendMsg(int to, void * buf, int size){
    int written = 0,  n = 0, miss = sizeof(int);
    
    while ( (n = write(to, &size+written, miss)) > 0){
        miss -= n;
        written += n;
    }
    
    if(n < 0 ) return -1;   

    written = 0;
    while (written < size){
        if((n = write(to, buf+written, size-written)) <= 0 ){
            return -1;
        }
        written += n;
    }
    return written;
}

int main(){

        int fd = clipboard_connect("./"), i = 0, size;
        char c = 0;
        char *buf = NULL;
        
        if(fd == -1){
            exit(-1);
        }
        /*
        printf("insert size:\n");
        while((c=getchar()) != 'q'){
            if(c != '\n'){
                size = atoi(&c);
                printf("size:%d\n",size );
                buf = malloc(size);
                for (i = 0; i < size; ++i)
                {
                    buf[i] = i+'0';
                }
                printf("------->sent: %d\n", sendMsg(fd, buf, size));
                free(buf);
                printf("insert size:\n");        
                
            }
        }*/
        while((c=getchar()) != 'q'){
            if(c != '\n'){
                size = 100000;
                printf("size:%d\n",size );
                buf = malloc(size);
                for (i = 0; i < size; ++i)
                {
                    buf[i] = i;
                }
                printf("------->sent: %d\n", sendMsg(fd, buf, size));
                free(buf);
                printf("insert size:\n");        
                
            }
        }

        
        close(fd);
        exit(0);
    }
