#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
  int n = 10;
  pid_t pids[10] = {-1};


  printf("I am THE father (aka %d)\n", getpid());

  for(int i=0; i<n; i++){
    if((pids[i]=fork()) < 0){
      printf("Error on iter. %d\n", i);
    }else if(pids[i] == 0){
      srand(time(NULL)*getpid());
      long int r = random()%RAND_MAX /(RAND_MAX/10);
      sleep(r);
      printf("child will sleep %ld s\t[%d]\n", r, i);
      exit(0);
    }

  }


  int status;
  pid_t pid;
  while (n > 0) {
    pid = wait(&status);
  //printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
    --n;  // TODO(pts): Remove pid from the pids array.
  }


  return 0;
}
