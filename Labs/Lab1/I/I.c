#include <stdio.h>
#include <stdlib.h>


int main(int argc, char const *argv[])
{
	int i,j = 0;
	int size = 0;
	char * result_str = NULL;
	
	for(i=0; i < argc; i++){
		result_str = (char*)realloc(result_str, size*sizeof(char)+sizeof(argv[i]));

		j=0;
		while(argv[i][j] != '\0'){
			result_str[size] = argv[i][j];
			
			size++;
			j++;
		}	
	}
	result_str[size] ='\0';
	printf("<<%s>>\n", result_str);
	
	return 0;
}