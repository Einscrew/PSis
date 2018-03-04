#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define CORRECTION 1

#ifdef CORRECTION
#include <string.h>
#endif

int main(){
	char v1[100];
	char *v2=NULL;
	int i;

	printf("Write a word: ");
	fgets(v1, 100, stdin);

	#ifdef CORRECTION
	v2=(char*)malloc(sizeof(char)*strlen(v1));
	#endif

	for (i=0; v1[i]!='\0'; i++){
		v2[i] = toupper(v1[i]);
	}

	printf("Converted string: %s", v2);


}
