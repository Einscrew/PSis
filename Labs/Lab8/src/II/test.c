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
        
        char dados[10] = "";
        int dados_int;
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
        

        clipboard_copy(fd, 5, "uma", 4);
        sleep(1);

        clipboard_copy(fd, 6, "dois", 5);
        sleep(1);
        
        clipboard_copy(fd, 7, "ovo", 5);
        sleep(1);
        
        clipboard_copy(fd, 8, "dois", 5);
        sleep(1);
        
        clipboard_copy(fd, 9, "cinco", 6);
        sleep(1);


        clipboard_paste(fd, 9, dados, sizeof(dados));
        printf("Received from [1] - %s||\n", dados);



        //write(fd, dados, 10);
        //clipboard_paste(fd, 0, dados, &dados_int);
        //read(fd+1, &dados_int, sizeof(dados_int));
        //printf("Received %d\n", dados_int);
        
        close(fd);
        exit(0);
    }
