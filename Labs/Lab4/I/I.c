#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>



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
    if( p%i == 0 ){
        return 0;
    }
  }
  return 1;

}

int main(int argc, char const *argv[]) {
  int pid, i;
  int stat;
  if(argv[1] == NULL || argv[2] == NULL){
    printf("Usage %s <#slaves> <lenght of set>\n", argv[0]);
    return 0;
  }

  int msPipe[2];
  int smPipe[2];
  if (pipe2(msPipe, O_NONBLOCK) != 0 || pipe(smPipe) != 0 ) {
    printf("Pipe problems\n");
    return 0;
  }
  srand(time(NULL));

  for( i=0; i < atoi(argv[1]); i++){
    if( (pid = fork()) == 0){
      long int v;
      while (read(msPipe[0], &v, 1) > 0) {
          printf("receiving %ld\n", v);
        if(isPrime(v)){
          printf("[SON %d] %d\n", (int)i, (int)v);
        }
      }
      return EXIT_SUCCESS;

    }else if( pid < 0){
      printf("Error crating slave %d\n", i);
    }
  }

  int setSize = atoi(argv[2]) -1 ;
  long int r = -1;

  while (setSize > 0) {
    r = (long int) rand()%99999;
    printf("sending %ld\n", r);
    write(msPipe[1],&r, 1);
    setSize--;
  }

  while ( wait(&stat) != -1) {
  }
  close(msPipe[1]);
  close(msPipe[0]);
  return 0;
}
