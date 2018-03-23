#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>


int main(int argc, char const *argv[]) {

  if(argv[1] == NULL){
    printf("Usage %s <fifo> \n", argv[0]);
    return 0;
  }
  int fifo  = open(argv[1], O_WRONLY);

  if( fifo == -1){
      //printf("Erro on open(): %s\nTrying to crate fifo...\n", strerror(errno));
      if(mkfifo(argv[1], 0666) == -1){//S_IRWXU))
          printf("Erro on mkfifo(): %s\n", strerror(errno));
      }
      fifo = open(argv[1], O_WRONLY);

      if(fifo == -1){
          printf("ERRO: exiting\n");
          return 1;
      }
  }

  char line[100];
  while(1){
      fgets(line, sizeof(line), stdin);
      write(fifo, line, strlen(line));
  }

  close(fifo);

  return 0;
}
