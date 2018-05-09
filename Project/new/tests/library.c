#include <stdlib.h>
#include <stdio.h>

#include "clip.h" //<-------------------glo declared here
#include "library.h"

int clipboard_connect(char * clipboard_dir){
	printf("connect\n");
	return 1;
}

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){
	printf("copy\n");
	return 1;
}

int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){
	printf("paste\n");
	return 1;
}

int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){
	printf("wait %d\n", glo);
	waitclip();
	return 1;
}
