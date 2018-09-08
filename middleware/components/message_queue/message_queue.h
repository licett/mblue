/*
 * =====================================================================================
 *
 *       Filename:  message_queue.h
 *
 *    Description:  a simple message queue
 *
 *        Version:  1.0
 *        Created:  2017/4/13 20:21:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  message_queue_INC
#define  message_queue_INC
#include "mblue_stddefine.h"
#define	MAX_MESSAGE_QUEUE_ITEMS			128			/*  */
/*
struct common_bundle {
	void *value;
	void *info;
};
*/
struct message_queue
{
	int    size;
	int    next;
	int    capacity;
	void** data;
	//struct common_bundle *cb;

	int (*init)(struct message_queue *mq, uint32_t size);
	int (*uninit)(struct message_queue *mq);
	int (*add)(struct message_queue *mq, void *value);
	void* (*remove)(struct message_queue *mq);
};
struct message_queue *mblue_mq_alloc(uint32_t size);
int mblue_mq_release(struct message_queue *mq);
#endif   /* ----- #ifndef message_queue_INC  ----- */
