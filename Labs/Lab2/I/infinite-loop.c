#include <stdio.h>
#include <stdlib.h>

#define CORRECT 1



int main(){
	int v[100];
	int i;

	for (i=0; i<100; i++/*i+1*/){
		v[i] = random();
	}

	printf("vector initialized\n");

}
