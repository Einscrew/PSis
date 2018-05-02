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
		char dados[10];
		int count = 10;
		int fd = clipboard_connect("./");

		int ini = atoi(argv[1]);
		char c = '0'+ini;

		for (int i = ini; i < ini+1; ++i)
		{
			clipboard_copy(fd, i, &c, 1);
			sleep(4);
			c++;
		}

		/*
		if(fork()==0){
			fd = clipboard_connect("./");
		
			
			if(fd == -1){
				exit(-1);
			}

			clipboard_copy(fd, 0, "uma", 3);
			sleep(1);

			clipboard_copy(fd, 1, "dois", 4);
			sleep(1);
			
			clipboard_copy(fd, 1, "ovo", 3);
			sleep(1);
			
			clipboard_copy(fd, 3, "dois", 4);
			sleep(1);
			
			clipboard_copy(fd, 4, "cinco", 5);
			sleep(1);
			
			clipboard_paste(fd, 1, (void*)&dados, count);
			printf("Received from [1] - %s||\n", dados);
		}else{
			fd = clipboard_connect("./");
		
			
			if(fd == -1){
				exit(-1);
			}

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
		}*/
		
		close(fd);
		exit(0);





		//write(fd, dados, 10);
		//clipboard_paste(fd, 0, dados, &dados_int);
		//read(fd+1, &dados_int, sizeof(dados_int));
		//printf("Received %d\n", dados_int);
		
	}
