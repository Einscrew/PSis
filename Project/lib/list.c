#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "utils.h"
#include "list.h"


/* Linked list structure */
struct _t_list
{
    Item this;
    struct _t_list *prox;
};

/*******************************************
 *   Initializes a list   				   *
 *										   *
 * Returns: t_list *  				       *
 ******************************************/

t_list  *initList(void){

    return NULL;
}

/******************************************************************************
 *   This fucntion puts a node in the list 								      *
 *   															      		  *
 * Parameters: lp - head's list    											  *
 *             this - content of the new element     						  *
 *	           reuseNode() - function that checks if a node can be reused     *
 *   															     		  *
 * Returns: the head of the list   								      		  *
 *****************************************************************************/

t_list  *new(t_list* lp, Item this, int reuseNode(Item)){

	t_list *aux;

    /* Puts the node as the head of the list */
    if(lp==NULL){
	    t_list * new = (t_list*) mallocV(sizeof(t_list), ": new list node");
        new->this = this;
        lp=new;
        new->prox = NULL;

    }else{

        if(reuseNode == NULL){
            t_list * new = (t_list*) mallocV(sizeof(t_list), ": new list node");

            new->this = this;
            new->prox=lp;
            lp = new;
        }else{

            aux=lp;
            while(1){

            	/* Checks if a node can be reused */
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

/*********************************************************
 *    Gets a content of a specified element of the list  *
 *   											         *
 * Parameters: p -Element of the list  					 *
 *		    										     *
 * Returns: Content of the element  					 *
 ********************************************************/

Item getItem (t_list *p){

    return p -> this;
}

/***************************************************
 *    Gets the next element of the list            *
 *  											   *
 * Parameters: p - element of the list             *
 *												   *
 * Returns: pointer to next element in the list    *
 **************************************************/

t_list *next(t_list *p){

    return p -> prox;
}

/**********************************************************************
 *    Frees the entire list 										  *
 *			  												      	  *
 * Arguments: lp - pointer to list   								  *
 *            freeItem() - function to free the content of the Item   *
 *********************************************************************/

void freeList(t_list *lp, void freeItem(Item)){
    t_list *aux, *newhead;  /* auxiliar pointers to travel through the list */

    for(aux = lp; aux != NULL; aux = newhead)
    {
        newhead = aux->prox;
        if(freeItem != NULL)freeItem(aux->this);
        free(aux);
    }

    return;
}

/**********************************************************************
 *    Frees the entire list and closes file descriptors				  *
 *			  												      	  *
 * Arguments: lp - pointer to list   								  *
 *            freeItem() - function to free the content of the Item   *
 *            closeItem() - function that closes the file descriptors *
 *********************************************************************/

void closeFreeList(t_list *lp, void freeItem(Item), void closeItem(Item)){
	
	/* Auxiliar pointer to travel through the list */
    t_list *aux, *newhead; 

    for(aux = lp; aux != NULL; aux = newhead)
    {
        newhead = aux->prox;
        closeItem(aux->this);
        if(freeItem != NULL)freeItem(aux->this);
        free(aux);
    }

    return;
}
