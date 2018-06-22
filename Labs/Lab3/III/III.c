#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>


long int counter = 0;


void handler(int signal) {
  counter = 0;
  alarm(random()%RAND_MAX /(RAND_MAX/10));
}

int main(int argc, char const *argv[]) {

  long int r = 0;

  struct sigaction *s = malloc(sizeof(struct sigaction));
  s->sa_handler = handler;

  sigaction(SIGALRM, s, NULL);

  srand(time(NULL));

  r = random()%RAND_MAX /(RAND_MAX/10);
  alarm(r);

  while (1) {
    printf("[%10ld]\n", counter++);
    sleep(1);
  }


  return 0;
}
