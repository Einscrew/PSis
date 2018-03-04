#include <stdlib.h>
#include <stdio.h>

int a;                      //DATA

int main(){
    int b;                  //STACK
    char v[10]="abcdefghi"; //STACK     

    char *p = malloc(1);    //HEAP
        
    printf("&a    - %p\n", &a);
    printf("&b    - %p\n", &b);

    printf("v     - %p\n", v);
    printf("&v    - %p\n", &v);
    printf("*(v+5)- %c\n", *(v+5));

    printf("&main - %p\n", main);
    printf("&exit - %p\n", exit);
    
    printf("&p    - %p\n", &p);
    printf("p     - %p\n", p);
    
    p = v;
    printf("p     - %c\n", *(p+4));

    exit(0);

}

/*run:                                           ffff ffff ffff ffff
&a    - 0x60104c            <--- HEAP           |                   |
&b    - 0x7ffdf3f2a4a4                          |                   |b
v     - 0x7ffdf3f2a4b0    <---Iguais            |                   |
&v    - 0x7ffdf3f2a4b0    <---Iguais            |                   |
&main - 0x4005b6            <--- HEAP           |                   |
&exit - 0x4004a0            <--- HEAP           |                   |
&p    - 0x7ffdf3f2a4a8                          |                   |a
p     - 0x1ca0010                               |                   |
                                                 0000 0000 0000 0000   */

/* gdb:
&a    - 0x60104c            <--- HEAP
&b    - 0x7fffffffdc34
v     - 0x7fffffffdc40    <---Iguais
&v    - 0x7fffffffdc40    <---Iguais
&main - 0x4005b6            <--- HEAP
&exit - 0x4004a0            <--- HEAP
&p    - 0x7fffffffdc38
p     - 0x602010

(gdb) print $pc
$1 = (void (*)()) 0x400685 <main+207>
(gdb) print $sp
$2 = (void *) 0x7fffffffdc30

*/