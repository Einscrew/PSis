#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <string.h>


#include "../lib/utils.h"
#include "../lib/clipboard.h"

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
			printf("\nread: %d -[%.*s]\n", read-1, read-1, dados);	

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
					printf(">%.*s<\n", read, dados);
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
