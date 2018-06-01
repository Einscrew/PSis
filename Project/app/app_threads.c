#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>

#include <stdbool.h>

#include <string.h>

#include "clipboard.h"

int msg = 0;

bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

void overload(void*arg){
	int * fd = (int*)arg;
	printf("ola%d %d\n", *fd, msg);
	int i = msg;
	char dados[100];
	sprintf(dados, "%ld", random());
	while(i > 0 && clipboard_copy(*fd, random()%10, dados, strlen(dados)) != -1){
		sprintf(dados, "%lu", random());
		i--;
	}
	close(*fd);
	free(fd);
}

int main(int argc, char*argv[]){
	int * fd;
	char path [1000];

	struct dirent *de;  // Pointer for directory entry
	pthread_t th;
	if(argc != 3){
		fprintf(stderr, "Usage: %s directory num\n", argv[0] );
		return 0;
	}
 	
 	msg = atoi(argv[2]);
 	if(msg == 0){
 		fprintf(stderr, "Insert a valid number\n");
 		return 0;
 	}
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(argv[1]);
 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }
 
    while ((de = readdir(dr)) != NULL){
    	sprintf(path, "%s/%s", argv[1], de->d_name );
    	if(is_dir(path)){
        	if(strcmp(de->d_name, ".") && strcmp(de->d_name, "..")){
        		sprintf(path, "%s/clipSocket", path);
        		fd = malloc(sizeof(int));
        		if( (*fd = clipboard_connect(path)) != -1){
        			//printf("%s\n", path);
        			if(pthread_create(&th, NULL, (void*)overload, (void*)fd) == -1){
						fprintf(stderr, "Couldn't create thread: %s\n", strerror(errno));
					}else{
						pthread_detach(th);
					}
        		}else{
        			free(fd);
        		}
        		

        	}
    	}    	
    }
 
    closedir(dr);

    pause();
	return 0;
		
}
