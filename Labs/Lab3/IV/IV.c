#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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

extern char **environ;

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
