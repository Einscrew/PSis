#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

/*
def isPrime(i):
...     for n in range(2,i):
...             if i%n == 0:
...                     return False
...
...     return True

*/
int isPrime(long int p){
  int i = 2;
  while (i < p) {
    if(p%i == 0){
        return 0;
    }
  }
  return 1;
}

int main(int argc, char const *argv[]) {
  int pid, i;
  int stat;
  int setSize = atoi(argv[2]) -1 ;
  long int r = -1;
  int msPipe[2];
  int smPipe[2];

  if(argv[1] == NULL || argv[2] == NULL){
    printf("Usage %s <#slaves> <lenght of set>\n", argv[0]);
    return 0;
  }

  if (pipe(msPipe) != 0 || pipe(smPipe) != 0 ) {
    printf("Pipe problems\n");
    return 0;
  }
  srand(time(NULL));

  /* Cria filhos */
  for( i=0; i < atoi(argv[1]); i++){
    if( (pid = fork()) == 0){
      long int v;
      int ret = 0;
      while ((ret = read(msPipe[0], &v, 1)) > 0) {
          printf("receiving %ld\n", v);
        if(isPrime(v)){
          printf("[SON %d] %ld\n", (int)i, v);
        }
      }
      if(ret == -1){
        printf("ERROR@%d\t-%s\n",i, strerror(errno));
      }
      return EXIT_SUCCESS;

    }else if( pid < 0){
      printf("Error crating slave %d\n", i);
    }
  }

  /* Lê todos os números enviados */
  while (setSize > 0) {
    memset(&r, 0, 1);
    r = (long int) rand() % 100000;

    printf("sending %ld\n", r);
    write(msPipe[1],&r, 1);
    setSize--;
  }
  if(close(msPipe[1]) == -1){
      printf("ERROR closing pipe\n");
      return -1;
  }

  while ( wait(&stat) != -1) { }

  if(close(msPipe[0]) == -1){
      printf("ERROR closing pipe\n");
      return -1;
  }
  return 0;
}
