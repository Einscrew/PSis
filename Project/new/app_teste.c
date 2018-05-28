#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <string.h>


#include "utils.h"

extern char *optarg;

char * dados = NULL;
int fd = -1;

void sigint_handler(int n){
    
    if(dados != NULL) free(dados);
	
	clipboard_close(fd);
    exit(EXIT_SUCCESS);
}

int main(int argc, char*argv[]){

	char c = 0, r= 'r';
	int size = -1;
	int read = 0;
	size_t n = 0;
	
	struct sigaction act_INT;
	act_INT.sa_handler = sigint_handler;
	sigemptyset(&act_INT.sa_mask);
	act_INT.sa_flags=0;
	sigaction(SIGINT, &act_INT, NULL);

	if(argc == 2){

		fd = clipboard_connect(argv[1]);
		if(fd == -1){
			fprintf(stderr, "Error: Bad UNIX socket\n");
			exit(EXIT_FAILURE);
		}

	}else{

		fprintf(stderr, "Usage: %s path/to/AF_UNIsocket\n", argv[0]);
		exit(EXIT_FAILURE);		
	}

	while(1){
		c = 0;
		r = 'r';
		size = -1;
		printf("Insert option [ c | p | w ]: ");
		while(!(c == 'c' || c == 'p' || c == 'w')){
			c = getchar();
		}
		printf("Insert region [ 0 - 9 ]: ");
		while(r < '0' || r > '9'){
			r = getchar();
		}

		while(getchar() != '\n'){}

		if(c == 'c'){
			printf("Terminate the content to copy with 'ESC':");
			read = getdelim(&dados, &n, 27 ,stdin);
			printf("\nread: %d -[", read-1);
			fflush(stdout);
			write(1, dados, read-1);
			printf("]\n");
			fflush(stdout);

			printf("sent: %d\n", clipboard_copy(fd, r-'0', dados, read-1));
			free(dados);
			dados = NULL;
			read = 0;

		}else{
			printf("How much to receive?: ");
			while(size < 0 && (scanf("%d", &size) != 1)){

			}
			printf("Will receive %d bytes\n",size );
			if( (dados = mallocV(size, ": size too big\n")) == NULL){

			}else{
				if(c == 'p'){
					read = clipboard_paste(fd, r-'0', dados, size);
				}
				else{
					read = clipboard_wait(fd, r-'0', dados, size);
				}

				if(read >= 0){
					printf("> ");
					fflush(stdout);
					write(1, dados, read );
					printf(" <\n");
				}else{
					printf("Couldn't perform the instruction\n");
				}
				free(dados);
				dados = NULL;
				read = 0;
			}
		}
	}

	exit(0);
}

/*2awndaliwdaw^
size = -1;
			if(c == 'c'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						printf("Tell the size of the content to copy\n");
						while(size < 0 && scanf("%d\n", &size) == 1){
							
						}

						p = mallocV(size, ": Couldn't allocate enought\n");
						i = 0;
						printf("Which char to send: \n");
						fflush(stdin);
						s = '\n';
						while (s == '\n')
							s=getchar();
						while(i < size){
							memcpy(p+i, &s, 1);
							i++;
						}

						clipboard_copy(fd, c-'0', p, size);
						printf("sendt\n");
						done = 0;
						free(p);
					}
				}
			}else if(c == 'p'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						i = clipboard_paste(fd, c-'0', dados, 10);
						
						printf(">%s\n", dados);
						done = 0;
					}
				}
			}else if(c == 'w'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						i = clipboard_wait(fd, c-'0', dados, 10);
						
						printf(">%s\n", dados);
						done = 0;
					}
				}
			}else if(c == 'e'){
				c=getchar();
				
					clipboard_copy(fd, -2, &c, 1);
					printf("sendt\n");
					done = 0;
				
			}
			if(c != '\n')printf("Insert option [c|p|w] region[0-9]\n");
			*/