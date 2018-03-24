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

int isPrime(long int p){
  int i = 2;
  while (i < p) {
    if(p%i == 0){
        return 0;
    }
    i++;
  }
  return 1;
}

int pipe_close(int * p, int n, const char name[]){
    int ret;
    char c = (n == 0)? '0' : (n == 1)? '1' : 'X';

    switch (n) {
        case 0:
            ret = close(p[n]);
            break;
        case 1:
            ret = close(p[n]);
            break;
        default:
            ret = close(p[0]) || close(p[1]);
            break;
    }
    if( ret == -1 ){
        printf("ERROR closing %s[%c]\t-%s\n", name, c, strerror(errno));
    }
    return ret;
}

int main(int argc, char const *argv[]) {
  int pid, i;
  int stat;
  int setSize = atoi(argv[2]) -1 ;
  long int r = -1;
  int msPipe[2];

  if(argv[1] == NULL || argv[2] == NULL){
    printf("Usage %s <#slaves> <lenght of set>\n", argv[0]);
    return 0;
  }

  srand(time(NULL));

  if (pipe(msPipe) != 0 ) {
    printf("Pipe problems\n");
    return 0;
  }

  /* Cria filhos */
  for( i=0; i < atoi(argv[1]); i++){
    if( (pid = fork()) == 0){
      long int v;
      int ret = 0;

      ret = ret || pipe_close(msPipe, 1, "@msPipe");

      while ( read(msPipe[0], &v, sizeof(v)  ) > 0 ) {
          //printf("receiving %ld\n", v);
        if(isPrime(v) == 1){
          printf("[SON %d] %ld\n", (int)i, v);
        }
      }
      return ret || pipe_close(msPipe, 0, "@msPipe");

    }else if( pid < 0){
      printf("Error crating slave %d\n", i);
      pipe_close(msPipe, 2, "@msPipe");
    }
  }

  pipe_close(msPipe, 0, "msPipe");

  /* Lê todos os números enviados */
  while (setSize > 0) {
    memset(&r, 0, 1);
    r = (long int) rand() % 100000;

    //printf("sending %ld\n", r);
    write(msPipe[1],&r, sizeof(r));
    setSize--;
  }

  pipe_close(msPipe, 1, "msPipe");

  while ( wait(&stat) != -1 ) { }

  return 0;
}
