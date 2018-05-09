#ifndef SV
#define SV
#include <pthread.h>

typedef struct clip{
	 char * data;
	 int size;
}Clip;

pthread_rwlock_t cliplock[10];
pthread_mutex_t waitlock[10];
pthread_cond_t w [10] = {PTHREAD_COND_INITIALIZER};
Clip clip[10];

#endif