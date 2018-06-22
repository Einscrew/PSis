#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  system(argv[1]);
  alarm(0);
  exit(1);
}
