/******************************************************************************
 *
 * File Name: lista.h
 *	      (c) 2010 AED
 * Authors:    AED Team
 * Last modified: ACR 2010-03-17
 * Revision:  v2.1
 *
 * COMMENTS:
 *		Structure and prototypes for type t_list, a 1st order
 *              abstract data type that is a container.
 *		Each variable of type t_list implements a node of
 *              list of Items.
 *
 *****************************************************************************/

#ifndef _LISTA_H
#define _LISTA_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>


#include "defs.h"


/* type definition for structure to hold list item */
typedef struct _t_list t_list;

t_list  *initList(void);
t_list  *new(t_list* lp, Item this, int reuseNode(Item));
Item getItem (t_list *p);
t_list *next(t_list *p);
void freeList(t_list *lp, void freeItem(Item));
void closeFreeList(t_list *lp, void freeItem(Item), void closeItem(Item));

#endif