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
      int r = rand() % 10;
      //long int r = random()%RAND_MAX /(RAND_MAX/10);
      struct timespec tb;
      struct timespec te;
      clock_gettime(CLOCK_REALTIME,&tb);
      sleep(r);
      clock_gettime(CLOCK_REALTIME,&te);

      printf("child slept %fms ???? real: %d\t[%d - %d]\n", (te.tv_nsec - tb.tv_nsec)*0.0000001 + te.tv_sec - tb.tv_sec, r, i, getpid());
      fflush(stdin);
      exit(0);
    }
  }

  int status;
  while (wait(&status) > 0){}
  printf("TERMINATING\n" );
  return 0;
}
