#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){

		int fd = clipboard_connect("./");
		
		
		if(fd == -1){
			exit(-1);
		}
		
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
		char dados[10];
		int count = 10;
		
		clipboard_copy(fd, 0, "uma", 3);
		// sleep(1);

		clipboard_copy(fd, 1, "dois", 4);
		// sleep(1);
		
		clipboard_copy(fd, 1, "ovo", 3);
		// sleep(1);
		
		clipboard_copy(fd, 3, "dois", 4);
		// sleep(1);
		
		clipboard_copy(fd, 4, "cinco", 5);
		// sleep(1);


		clipboard_paste(fd, 1, (void*)&dados, count);
		printf("Received from [1] - %s||\n", dados);



		//write(fd, dados, 10);
		//clipboard_paste(fd, 0, dados, &dados_int);
		//read(fd+1, &dados_int, sizeof(dados_int));
		//printf("Received %d\n", dados_int);
		
		close(fd);
		exit(0);
	}
