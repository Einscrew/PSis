#include <stdlib.h>
#include <stdio.h>

int a;

int main(){
    int b;
    char v[10];

    char *p = malloc(1);
    
    
    
    printf("&a    - %p\n", &a);
    printf("&b    - %p\n", &b);

    printf("v     - %p\n", v);
    printf("&v    - %p\n", &v);

    printf("&main - %p\n", main);
    printf("&exit - %p\n", exit);
    
    printf("&p    - %p\n", &p);
    
    printf("p     - %p\n", p);

    exit(0);

}

/*run:
&a    - 0x60104c            <--- HEAP
&b    - 0x7ffdf3f2a4a4
v     - 0x7ffdf3f2a4b0
&v    - 0x7ffdf3f2a4b0
&main - 0x4005b6            <--- HEAP
&exit - 0x4004a0            <--- HEAP
&p    - 0x7ffdf3f2a4a8
p     - 0x1ca0010
*/

/* gdb:
&a    - 0x60104c            <--- HEAP
&b    - 0x7fffffffdc34
v     - 0x7fffffffdc40
&v    - 0x7fffffffdc40
&main - 0x4005b6            <--- HEAP
&exit - 0x4004a0            <--- HEAP
&p    - 0x7fffffffdc38
p     - 0x602010

(gdb) print $pc
$1 = (void (*)()) 0x400685 <main+207>
(gdb) print $sp
$2 = (void *) 0x7fffffffdc30

*/