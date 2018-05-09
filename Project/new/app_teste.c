#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char*argv[]){

		
		
		/*while(1){
			switch fgetc(){
				case 'P':
					
					break;

				case 'C':
					fgets(dados, 10, stdin);
					clipboard_copy(fd, 0, dados, sizeof(dados));
					break;

				default:
					break;
			}
			
		}*/
		char c, done;
		char dados[10];
		int fd = clipboard_connect("./");

		
		while((c=getchar()) != 'q')		
		{
			if(c == 'c'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						clipboard_copy(fd, c-'0', &c, 1);
						printf("sendt\n");
						done = 0;
					}
				}
			}else if(c == 'p'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						clipboard_paste(fd, c-'0', dados, 10);
						
						done = 0;
					}
				}
			}else if(c == 'w'){
				done = 1;
				while(done)		
				{
					c=getchar();
					if(c <= '9' && c >= '0'){
						clipboard_wait(fd, c-'0', dados, 10);
						
						done = 0;
					}
				}
			}
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
