#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <gnu/lib-names.h>

int main(){
	int a;

	void * handle = NULL;
	void (*f1)();
	void (*f2)();
	char * error;

	printf("What version of the functions you whant to use?\n");
	printf("\t1 - Normal    (test1)\n");
	printf("\t2 - Optimized (test2)\n");
	scanf("%d", &a);
	if (a == 1){
		/* load library test1 */
		handle = dlopen("/home/einstein/IST/MSc/PSis/Labs/Lab1/IV/test1.so", RTLD_LAZY);

		printf("running the normal versions from \n");
	}else{
		if(a== 2){
			/* load library test2 */
			handle = dlopen("/home/einstein/IST/MSc/PSis/Labs/Lab1/IV/test2.so", RTLD_LAZY);

			printf("running the normal versions\n");
		}else{
			printf("Not running anything\n");
			exit(-1);
		}
	}
		
	if (!handle) {
	    fprintf(stderr, "%s\n", dlerror());
		exit(EXIT_FAILURE);
	}

	dlerror();

	f1 = (void (*)(void)) dlsym(handle, "func_1");
	f2 = (void (*)(void)) dlsym(handle, "func_2");
	
	f1();
	f2();	
	/* call func_1 from whichever library was loaded */
	/* call func_2 from whichever library was loaded */
	exit(0);
	
	
}
