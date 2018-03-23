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
  int fifo;

  /*if( (fifo = open(argv[1], 'w')) == -1){
      printf("Erro on open(): %s\nTrying to crate fifo...\n", strerror(errno));
  }
  else */
  if( mkfifo(argv[1], S_IRWXU) ){
      printf("Erro on mkfifo(): %s\n", strerror(errno));
  }
  if( (fifo = open("./fifo", S_IRWXU)) == -1){
      printf("Erro on open(): %s\n", strerror(errno));
  }

  write(fifo, "0", 1);

  close(fifo);


  return 0;
}
