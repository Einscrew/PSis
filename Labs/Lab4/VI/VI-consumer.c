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


/*
octal    6   4   6
binario 110 100 110
what    rwx rwx rwx
*/

int main(int argc, char const *argv[]) {

  if(argv[1] == NULL){
    printf("Usage %s <fifo> \n", argv[0]);
    return 0;
  }
  int fifo  = open(argv[1], 0444);

  if( fifo == -1){
      printf("ERRO: exiting\n");
      return 1;
  }

  char line[100];
  int n = 0;
  while(1){
      n = read(fifo, line, sizeof(line));
      if(n <= 0){
          printf("Exiting\n" );
          break;
      }
      line[n-1] = '\0';
      printf("%s\n", line);
  }

  close(fifo);

  return 0;
}
