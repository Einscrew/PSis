#include <sys/un.h>
#include <sys/socket.h>

#include <errno.h>
#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "utils.h"
#include "connection.h"
#include "clipboard.h"

/***********************************************************************************************
*    This function establishes a connection with the local clipboard		               	   *
* 															 				                   *
* Parameters: clipboard_dir - directory where the local clipboard was launched	               *
*                                        			  						                   *
* Returns: -1 if the local clipboard cannot be accessed      				                   *
*          the file descriptor that allows the communication with the local clipboard          *
***********************************************************************************************/

int clipboard_connect(char * clipboard_dir){

	int sfd = 0;
	struct sockaddr_un my_addr;

	/* Creates a socket */
	if((sfd = socket(AF_UNIX, SOCK_STREAM, 0) ) == -1){
		printf("Local clipboard couldn't be accessed: [%s]\n", strerror(errno));
		return -1;
	}

	memset(&my_addr, 0, sizeof(struct sockaddr_un));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, clipboard_dir, strlen(clipboard_dir));

	/* Establishes a connection with the local clipboard */
	if(connect(sfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_un)) != 0){
		printf("Couldn't connect to the Local clipboard: %s\n", strerror(errno));
		return -1;
	}
	
	/* Returns the file descriptor that allows the communication with the local clipboard */
	return sfd;
}

/*************************************************************************************************
*    This function copies the data that is pointed by the given buffer to a region on the        *
*	local clipboard	               	  														     *
* 															 				                     *
* Parameters: clipboard_id - value returned by the clipboard_connect() function	                 *
*			  region - region that the user wants to copy the data to              	  		     *
*			  buf - pointer to the buffer that has the data the will be copied to the specified  *
*			 region    																		     *
*			  count - length of the data pointed by buf 									     *
*																							     * 
* Returns: 0 in case of error     				                                                 *
*          the number of bytes copied              											     *
*************************************************************************************************/

int clipboard_copy(int clipboard_id, int region, void *buf, size_t count){

	/* Checks if the given region is valid */ 
	if(region < 0 || region > 9 || count < 0 || clipboard_id < 0) return 0;

	long int finalsize = count+sizeof(char)+sizeof(char);
	
	char *msg = (char*)mallocV(finalsize, ": copy msg alloc");

	msg[0] = 'C';
	msg[1] = region+'0';
	memcpy(msg+2, buf, count);
	
	/* Sends the content of the requested region */
	int r = sendMsg(clipboard_id, msg, finalsize);

	return (r == -1)?(0):(count);
}

/*************************************************************************************************
*    This function copies the data of a certain region to the buffer given up to the length of   *
*	count	               	  														             *
* 															 				                     *
* Parameters: clipboard_id - value returned by the clipboard_connect() function	                 *
*			  region - region that the user wants to paste the data from            	         *
*			  buf - pointer to the buffer that has the data the will be paste                    * 				
*			  count - length of the data pointed by buf 									     *
*																							     *                           			  						                     
* Returns: 0 in case of error     				                                                 *
*          the number of bytes copied              											     *
*************************************************************************************************/

int clipboard_paste(int clipboard_id, int region, void *buf, size_t count){

	/* Checks if the given region is valid */ 
	if(region < 0 || region > 9 || count <= 0 || clipboard_id < 0) return 0;

	char toSend[2];
	void * p = NULL;
	int r = 0;
	
	toSend[0] = 'P';
	toSend[1] = region+'0';

	/* Send the paste request to the local clipboard */
	if(sendMsg(clipboard_id, toSend, 2) == -1){
		return 0;
	}

	memset(buf, '\0', count);

	/* Receive the content of the requested region */
	if((r = recvMsg(clipboard_id, (void**)&p)) <= 0){
		return 0;
	}

	/* Keeps the size of the message that was received  */
	count = (r<count)?r:count;
	
	memcpy(buf, p, count);
	
	free(p);

	return count;
}

/*************************************************************************************************
*    This function waits for a change in a certain region and copies it to the buffer given up   *
*   to a length of count  																         *
*															 				                     *
* Parameters: clipboard_id - value returned by the clipboard_connect() function	                 *
*			  region - region that the user wants to wait for            	                     *
*			  buf - pointer to the data where the data will be copied to                         *				
*			  count - length of the data pointed by buf 									     *
*																							     *                           			  						                     
* Returns: 0 in case of error     				                                                 *
*          the number of bytes copied              											     *
*************************************************************************************************/

int clipboard_wait(int clipboard_id, int region, void *buf, size_t count){

	/* Checks if the given region is valid */ 
	if(region < 0 || region > 9 || count <= 0 || clipboard_id < 0) return 0;

	char toSend[2];
	void * p = NULL;
	int r = 0;
	
	toSend[0] = 'W';
	toSend[1] = region+'0';

	/* Send the wait resquest to the local clipboard */
	if(sendMsg(clipboard_id, toSend, 2) == -1){
		return 0;
	}

	/* Receive the requested region after a copy is done to that region */
	if((r = recvMsg(clipboard_id, (void**)&p)) == -1){
		return 0;
	}

	/* Keeps the size of the message that was received */
	count = (r<count)?r:count;

	memcpy(buf, p, count);
	
	free(p);

	return count;
}

/*************************************************************************************************
*    This function closes the connection between the application and the local clipboard         * 
*															 				                     *
* Parameters: clipboard_id - value returned by the clipboard_connect() function	                 *
*************************************************************************************************/

void clipboard_close(int clipboard_id){
	close(clipboard_id);
}