#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>

#include "clipboard.h"
extern char *optarg;


int SIZE = 5000000;

int main(int argc, char*argv[]){

		char c, opt;
		char dados[SIZE];
		int fd = -1, r, index = 0;
		FILE * f = NULL;

		while ((opt = getopt(argc, argv, "c:f:r:i:")) != -1) {
		    switch (opt) {
			    case 'c':
					fd = clipboard_connect(optarg);
			        break;
			    case 'f':
			    	if((f = fopen(optarg, "rb")) == NULL){
			    		fprintf(stderr, "Not a valid file [%s]\n", optarg);	
			    		exit(EXIT_FAILURE);	
			    	}
			    case 'r':
			    	r = atoi(optarg);
			    	if(r > 9 || r < 0){
			    		fprintf(stderr, "Not a valid region [%d]\n", r);	
			    		exit(EXIT_FAILURE);	
			    	}
			    case 'i':
			    	c = optarg[0];
			    	break;
			    default: /* '?' */
			        fprintf(stderr, "Usage: %s -c path/to/AF_UNIXsocket -f filename -r 0 -i c|p|w\n", argv[0]);
			        exit(EXIT_FAILURE);
		    }
		}

		if(fd == -1){
			fprintf(stderr, "Usage: %s -c path/to/AF_UNIXsocket -f filename -r 0 -i c|p|w\n", argv[0]);
			exit(EXIT_FAILURE);
		}

		switch(c){
			case 'c':
			//https://bytes.com/topic/c/answers/917991-reading-jpg-file-binary
				fseek(f,0,SEEK_END); //go to end
				index = ftell(f); //get position at end (length)
				
				rewind(f);//,0,SEEK_SET); //go to beg.
				
				fread(dados,index,1,f); //read into buffer
				fclose(f);

				//printf("cpy\n");
				
				//printf("%d-%c\n", index, (dados[index]==EOF)?'y':'n');
				//write(1, dados, index);
				
				clipboard_copy(fd, r, &dados, index);
				break;
			case 'p':
				index = clipboard_paste(fd, r, &dados, SIZE);
				write(1, dados, index);
				break;
			case 'w':
				clipboard_wait(fd, r, &dados, strlen(dados)+1);
				break;
			default:
				fprintf(stderr, "Not a valid instruction [%c]\n", c);	
				break;
		}
		
		clipboard_close(fd);
		exit(0);
		
	}
