#include <stdlib.h>
#include <stdio.h>

void * mallocV(int size, char * msg){
	void * r = malloc(size);
	if(r == NULL){
		printf("Error allocing memory%s\n", (msg==NULL)?"":msg);
		exit(EXIT_FAILURE);
	}
	return r;
}