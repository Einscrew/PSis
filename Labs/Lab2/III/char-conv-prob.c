#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


int main(){
	char v1[100];
	char *v2=NULL;
	int i;
	
	int j=0;


	printf("Write a word");
	fgets(v1, 100, stdin);

	v2 = (char*)malloc(sizeof(char)*strlen(v1));
	for (i=0; v1[i]!='\0'; i++, j++){
		if(v1[i]=='\n'){
			j--;
			continue;
		}
		else{
			v2[j] = toupper(v1[i]);

		}
	}
	v2[j]='\0';
	printf("Converted string: %s\n", v2);

	free(v2);

}
