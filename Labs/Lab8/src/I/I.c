#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

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

int process(void * arg){
	int *i = (int *) arg;
	long int v;
	int ret;
	int cnt=0;

	//printf("Received %d & %d\n", i[0], i[1]);
	while ( (ret = read(i[0], &v, sizeof(long int)  )) > 0 ) {
	  //printf("receiving %ld\n", v);
	  cnt=isPrime(v)?cnt+1:cnt;
	  //printf("isPrime(%ld)? [%c]\n", v, isPrime(v)?'Y':'N');

	}
	close(i[0]);
	return cnt;
}

int main(int argc, char const *argv[])
{
	pthread_t* p;
	int i, r, num_threads, num;
	void * res;
	int ** msPipe;
	long int value = 0;
	int cnt= 0;

	if (argc != 3){
		printf("Usage: %s <num-threads> <num>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	num_threads = atoi(argv[1]);
	num = atoi(argv[2]);
	p = malloc(sizeof(pthread_t)*num_threads);
	msPipe = malloc(sizeof(int*)*num_threads);

	for (i = 0; i < num_threads; ++i)
	{
		msPipe[i] = malloc(sizeof(int)*2);
		if (pipe(msPipe[i]) != 0 ) {
			printf("Pipe creating problem [%d]\n", i);
			return 0;
	    }

		pthread_create(&p[i], NULL, (void *)process, msPipe[i]);
	}

	i = 0;
	while(value < num ){
		//printf("Writing %ld, to %d\n", value, (int)(value%num_threads) );
		write(msPipe[(int)(value%num_threads)][1], &value, sizeof(long int));
		value++;
	}
		
	for (i = 0 ; i<num_threads; i++){
		if (close(msPipe[i][1]) != 0 ) {
			printf("Pipe creating problem [%d]\n", i);
			return 0;
	    }
	}

	for (i = 0; i < num_threads; ++i)
	{
		r=pthread_join(p[i], &res);
		if(r==1){
			printf("[%d] exited  with failure\n",i );
			exit(EXIT_FAILURE);
		}
		cnt+=res;
		printf("Thread %d returned %d\n", i, res );
	}
	printf("TOTAL:%d\n",cnt );

	return 0;
}