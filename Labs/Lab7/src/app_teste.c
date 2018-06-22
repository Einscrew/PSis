#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <string.h>


int main(){

		/*int fd = clipboard_connect("./");
		
		
		if(fd == -1){
			exit(-1);
		}
		
		char dados[10] = "";*/
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
			
		}*//*
		

		clipboard_copy(fd, 0, "uma", 4);
sleep(1);
		clipboard_copy(fd, 1, "dois", 5);
sleep(1);
		clipboard_copy(fd, 1, "over", 5);
sleep(1);
		clipboard_copy(fd, 3, "dois", 5);
sleep(1);
		clipboard_copy(fd, 4, "cinco", 6);
sleep(1);


		clipboard_paste(fd, 1, dados, sizeof(dados));

*/

		//write(fd, dados, 10);
		//clipboard_paste(fd, 0, dados, &dados_int);
		//read(fd+1, &dados_int, sizeof(dados_int));
		//printf("Received %d\n", dados_int);
		
		//close(fd);

		char c = 0;
		int index;
		Element e;
		memset(e.content, 0, sizeof(Element));
		e.type = 'C';
		e.region = 1;
		memcpy(e.content,"ola", 3);

		printf("Sizeof Element:%lu\n", sizeof(Element));

		index = 100;
		printf("!%c!\n", &e+index );
		memcpy(&e+index, &c, 1);
		printf("!%c!\n", &e+index);
		printf("%s\n",e.content );
		exit(0);
	}
