#include <stdio.h>
#include <stdlib.h>

int getSize(char * s){
	int i = 0;
	int size = 0;

	while(s[i] != '\0'){
		size++;
		i++;
	}
	return size;
}

void cat (int *size, char * r, char *s){
	int i = 0;
	while(s[i] != '\0'){
		r[(*size)++] = s[i++];
	}
}

int main(int argc, char const *argv[])
{
	int i = 0;
	int size = 0;
	char * result_str = NULL;
	
	for(i=0; i < argc; i++){
		size += getSize((char *)argv[i]);
	}

	result_str = (char *)malloc(sizeof(char)*++size);
	result_str[size] ='\0';
	
	size = 0;
	for(i=0; i < argc; i++){
		cat(&size, result_str, (char *)argv[i]);
	}
	result_str[size] ='\0';
	printf("%s\n", result_str);
	
	return 0;
}