#ifndef _LISTA_H
#define _LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "defs.h"

/* type definition for structure to hold list item */
typedef struct _t_list t_list;

/* Initializes a list */
t_list  *initList(void);

/* Creates a new node of the list */ 
t_list  *new(t_list* lp, Item this, int reuseNode(Item));

/* Gets the content of the element given in the list */ 
Item getItem (t_list *p);

/* Gets the next element of the list */
t_list *next(t_list *p);

/* Fress the entire list */
void freeList(t_list *lp, void freeItem(Item));

/* Frees the entire list and closes the file descriptors */
void closeFreeList(t_list *lp, void freeItem(Item), void closeItem(Item));

#endif