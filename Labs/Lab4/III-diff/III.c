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
    for n in range(2,i):
        if i%n == 0:
            return False

    return True

*/
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

int main(int argc, char const *argv[]) {
  int pid, i, numChilds;
  int stat;
  int setSize;
  long int r = -1;
  int ret = 0;
  int ** msPipe;
  int smPipe[2];


  if(argv[1] == NULL || argv[2] == NULL){
    printf("Usage %s <#slaves> <lenght of set>\n", argv[0]);
    return 0;
  }

  setSize =  atoi(argv[2]) - 1;
  numChilds = atoi(argv[1]);
  msPipe = (int**) malloc(sizeof (int*) * numChilds);

  for(i = 0; i < numChilds; i++){
    msPipe[i] = (int*)malloc(sizeof(int) * 2);
    if (pipe(msPipe[i]) != 0 ) {
      printf("Pipe problems i:%d\n", i);
      return 0;
    }
  }

  if(pipe(smPipe) != 0 ) {
    printf("Pipe problems\n");
    return 0;
  }
  srand(time(NULL));

  /* SPECIAL CHILD*/
  if( (pid = fork()) == 0){

      for(i = 0; i < numChilds; i++){
        if(close(msPipe[i][1]) == -1 || close(msPipe[i][0]) == -1){
            printf("ERROR closing pipe[][x]@SpecialChild\t-%s\n", strerror(errno));
            return -1;
        }
      }
      if(close(smPipe[1]) == -1 ){
          printf("ERROR closing pipe[1]@SpecialChild\t-%s\n", strerror(errno));
          return -1;
      }

      while ( (ret = read(smPipe[0], &r, sizeof(r)  )) > 0 ) {
        printf("[SPECIAL] %ld\n", r );
      }
      if (ret == 0) {
        printf("nada para ler\n" );
      }

      return EXIT_SUCCESS;
  }

  /* Cria filhos */
  for( i=0; i < numChilds; i++){
    if( (pid = fork()) == 0){

      long int v;


      if( close(smPipe[0]) == -1 ){
         printf("ERROR@[0] - %d\t-%s\n", i, strerror(errno));
         return -1;
       }

       //close pipes from other siblins
      for (int j = 0; j < numChilds; j++){
        if(j != i){
           if( close(msPipe[j][0]) == -1 || close(msPipe[j][1]) == -1 ){
              printf("ERROR@[%d][x] - %d \t-%s\n",j,i, strerror(errno));
              return -1;
            }
        }else{
            if( close(msPipe[j][1]) == -1 ){
                printf("ERROR@[%d]\t-%s\n",i, strerror(errno));
                return -1;
            }
        }
      }

      while ( (ret = read(msPipe[i][0], &v, sizeof(v)  )) > 0 ) {
        //printf("receiving %ld\n", v);
        if(isPrime(v)){
          write(smPipe[1], &v, sizeof(v));
        }

        //printf("[SON %d] isPrime(%ld)? %d\n", (int)i, v, p );

      }

      if(ret == -1){
        printf("ERROR@%d\t-%s\n",i, strerror(errno));
        return EXIT_FAILURE;
      }
      else if( ret == 0){
        return EXIT_SUCCESS;
      }


    }else if( pid < 0){
      printf("Error crating slave %d\n", i);
    }
  }
  //////////////////PARENT CODE

  if( close(smPipe[1]) == -1 || close(smPipe[0]) == -1){
     printf("ERROR@[0] - %d\t-%s\n", i, strerror(errno));
     return -1;
  }

  for(i = 0; i < numChilds; i++){
    if(close(msPipe[i][0]) == -1){
        printf("ERROR closing pipe[0]@Parent\t-%s\n", strerror(errno));
        return -1;
    }
  }
  /* Lê todos os números enviados */
  i = 0;
  while (setSize >= 0) {
    memset(&r, 0, 1);
    r = (long int) rand() % 99999;
    if (i == numChilds){
      i = 0;
    }
    //printf("sending %ld\n", r);
    write(msPipe[i][1],&r, sizeof(r));
    i++;
    setSize--;
  }
  for(i = 0; i < numChilds; i++){
    if(close(msPipe[i][1]) == -1){
        printf("ERROR closing pipe[1]@Parent\t-%s\n", strerror(errno));
        return -1;
    }
  }


  while ( wait(&stat) != -1 ) {
    if(stat == EXIT_FAILURE){
      printf("some child has problems\n");
    }
    else if( stat == EXIT_SUCCESS){
        printf("Parent will leave\n" );
    }
  }

  return 0;
}
