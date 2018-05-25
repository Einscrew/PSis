#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>

#include "utils.h"

extern char *optarg;

int main(int argc, char*argv[]){

		char c, opt, done, s;
		char dados[10];
		int size = -1;
		int fd = -1, i = 0;
		void * p;

		/*
		char * p = malloc(808464432);
		//printf("%c\n", (p==NULL)?'y':'n');
		if(p!=NULL){
			for (int i = 0; i < 808464432; ++i)
			{
				p[i]='0';
			}
			p[i-1] = '\0';
			printf("%s\n",p );
		}
		exit(1);
		*/

		while ((opt = getopt(argc, argv, "c:")) != -1) {
		    switch (opt) {
			    case 'c':
					fd = clipboard_connect(optarg);
			        break;
			    default: /* '?' */
			        fprintf(stderr, "Usage: %s [-c path/to/AF_UNIsocket]\n",
			                argv[0]);
			        exit(EXIT_FAILURE);
		    }
		}

		if(fd == -1){
			fprintf(stderr, "Usage: %s [-c path/to/AF_UNIsocket]\n",argv[0]);
			exit(EXIT_FAILURE);
		}
		
		printf("Insert option [c|p|w] region[0-9]\n");
		while((c=getchar()) != 'q')		
		{
			if(c == 'c'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						printf("Tell the size of the content to copy\n");
						while(size < 0){
							scanf("%d\n", &size);
						}

						p = mallocV(size, ": Couldn't allocate enought\n");
						i = 0;
						printf("Which char to send: \n"); fflush(stdout);
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
		}
/*
			clipboard_copy(fd, 6, "uma", 3);
			sleep(1);

			clipboard_copy(fd, 7, "dois", 4);
			sleep(1);
			
			clipboard_copy(fd, 8, "ovo", 3);
			sleep(1);
			
			clipboard_copy(fd, 9, "dois", 4);
			sleep(1);
			
			clipboard_copy(fd, 5, "cinco", 5);
			sleep(1);
			
			clipboard_paste(fd, 1, (void*)&dados, count);
			printf("Received from [1] - %s||\n", dados);
		*/
		
		close(fd);
		exit(0);





		//write(fd, dados, 10);
		//clipboard_paste(fd, 0, dados, &dados_int);
		//read(fd+1, &dados_int, sizeof(dados_int));
		//printf("Received %d\n", dados_int);
		
	}
