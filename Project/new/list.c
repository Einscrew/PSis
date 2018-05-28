/******************************************************************************
 *
 * File Name: lista.c
 *	      (c) 2009 AED
 * Authors:    AED Team
 * Last modified: ACR 2009-03-23
 * Revision:  v2.0
 *
 * COMMENTS
 *		implements functions for type t_list
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "utils.h"
#include "list.h"


/* Linked list  */
struct _t_list
{
    Item this;
    struct _t_list *prox;
};

/******************************************************************************
 * iniLista ()
 *
 * Arguments: none
 * Returns: t_list *
 * Side-Effects: list is initialized
 *
 * Description: initializes list
 *
 *****************************************************************************/

t_list  *initList(void)
{

    return NULL;
}

/******************************************************************************
 * criaNovoNoListaNoFim ()
 *
 * Arguments: lp - lista a que adiciona novo elemento
 *            this - conteudo do novo elemento
 * Returns: t_list  *
 *
 * Description: Cria novo nó no fim da lista
 *
 *****************************************************************************/
t_list  *new(t_list* lp, Item this, int reuseNode(Item))
{


	t_list *aux;

    /*Coloca-o como head da lista*/
    if(lp==NULL)
    {
	    t_list * new = (t_list*) mallocV(sizeof(t_list), ": new list node");
        new->this = this;
        lp=new;
        new->prox = NULL;

    }
    else
    {
        if(reuseNode == NULL){
            t_list * new = (t_list*) mallocV(sizeof(t_list), ": new list node");

            new->this = this;
            new->prox=lp;
            lp = new;
        }else{

            aux=lp;
            while(1){
                if(reuseNode(aux->this)){
                	free(aux->this);
                	aux->this = this;
                	return lp;
                }
                if(aux->prox == NULL)
                	break;
                aux=aux->prox;
            }
            t_list * new = (t_list*) mallocV(sizeof(t_list), ": new list node");

            new->this = this;
            aux->prox=new;
            new->prox = NULL;
        }


    }
    return lp;
}




/******************************************************************************
 * getItemLista ()
 *
 * Arguments: this - pointer to element
 * Returns: Item
 * Side-Effects: none
 *
 * Description: returns an Item from the list
 *
 *****************************************************************************/

Item getItem (t_list *p)
{

    return p -> this;
}


/******************************************************************************
 * getProxElementoLista ()
 *
 * Arguments: this - pointer to element
 * Returns: pointer to next element in list
 * Side-Effects: none
 *
 * Description: returns a pointer to an element of the list
 *
 *****************************************************************************/

t_list *next(t_list *p)
{

    return p -> prox;
}



/******************************************************************************
 * libertaLista ()
 *
 * Arguments: lp - pointer to list
 * Returns:  (void)
 * Side-Effects: frees space occupied by list items
 *
 * Description: free list
 *
 *****************************************************************************/
void freeList(t_list *lp, void freeItem(Item))
{
    t_list *aux, *newhead;  /* auxiliar pointers to travel through the list */

    for(aux = lp; aux != NULL; aux = newhead)
    {
        newhead = aux->prox;
        if(freeItem != NULL)freeItem(aux->this);
        free(aux);
    }

    return;
}

void closeFreeList(t_list *lp, void freeItem(Item), void closeItem(Item))
{
    t_list *aux, *newhead;  /* auxiliar pointers to travel through the list */

    for(aux = lp; aux != NULL; aux = newhead)
    {
        newhead = aux->prox;
        closeItem(aux->this);
        if(freeItem != NULL)freeItem(aux->this);
        free(aux);
    }

    return;
}
