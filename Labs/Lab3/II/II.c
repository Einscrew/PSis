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
      exit(r);
    }

  }


  int status;
  pid_t pid;
  int s = 0;
  while (n > 0) {
    pid = wait(&status);
    if(pid == -1 && !WIFEXITED(status)){
      printf("Error on child exit\n");
      return 1;
    }else{
      printf("[%ld] slept %ds.\n", (long)pid, WEXITSTATUS(status));
      s+=(status/256); //<--------------------------==?
      --n;
    }

  }
  printf("Master will sleep %ds\n", s);
  //sleep(39);


  return 0;
}
