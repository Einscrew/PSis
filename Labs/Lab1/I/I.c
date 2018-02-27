#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	int i,j = 0;
	int size = 0;
	char * args = NULL;
	
	for(i=0; i < argc; i++){
		args = (char*)realloc(args, size*sizeof(char)+sizeof(argv[i]));

		j=0;
		while(argv[i][j] != '\0'){
			args[size] = argv[i][j];
			
			size++;
			j++;
		}	
	}
	args[size] ='\0';
	printf("<<%s>>\n", args);
	
	return 0;
}