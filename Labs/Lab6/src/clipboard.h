#define CLIPBOARD_SOCKET "./abcdefghiclipSocket"
#include <sys/types.h>



typedef struct element{
	 char type;
	 char region;
	 char content[10];
}Element;


int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);

