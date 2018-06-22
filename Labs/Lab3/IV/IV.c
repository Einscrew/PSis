#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
/*
int main(int argc, char const *argv[]) {
  char line [100]={'\0'};

  while(fgets(line, 100, stdin) != NULL){
    //printf("%s", line);
    system(line);
  }


  return 0;
}*/


/*
int main(int argc, char const *argv[]) {
  char line [100]={'\0'};

  while(fgets(line, 100, stdin) != NULL){
    //printf("%s", line);

    if(fork()==0){

      execl("/bin/bash", "bash", "-c", line , (char*)0);
      exit(0);
    }
    int status;
    wait(&status);
  }


  return 0;
}
*/

/*
int main(int argc, char const *argv[]) {
  char line [100]={'\0'};
  int sum = 0;
  int status;
  while(fgets(line, 100, stdin) != NULL){
    //printf("%s", line);
    if(fork()==0){
      struct timespec tp;
      execl("/bin/bash", "bash", "-c", line , (char*)0);
      if(clock_gettime(CLOCK_REALTIME,&tp)){
        printf("error getting time\n" );
      }
      exit(tp.tv_sec);
    }
    wait(&status);
    sum += WEXITSTATUS(status);
    printf("[%ld s] %s\n", (long int)WEXITSTATUS(status) , line);
  }
  printf("------------------\n[%d s] all comands\n", sum);

  return 0;
}*/


int main(int argc, char const *argv[]) {
  char line [100]={'\0'};
  int sum = 0;
  int status;
  while(fgets(line, 100, stdin) != NULL){
    //printf("%s", line);
    if(fork()==0){
      struct timespec tp;
      execl("./execute", "./execute", line , (char*)0);
      if(clock_gettime(CLOCK_REALTIME,&tp)){
        printf("error getting time\n" );
      }
      exit(tp.tv_sec);
    }
    wait(&status);
    sum += WEXITSTATUS(status);
    printf("[%ld s] %s\n", (long int)WEXITSTATUS(status) , line);
  }
  printf("------------------\n[%d s] all comands\n", sum);

  return 0;
}
