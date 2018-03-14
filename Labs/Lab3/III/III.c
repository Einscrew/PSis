#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {

  long int counter = 0;
  long int r = 0;

  s_rand(time(NULL));

  while (1) {
    r = random()%RAND_MAX /(RAND_MAX/10);
    printf("[%10ld]\n", counter++);
    sleep(1);
  }


  return 0;
}
