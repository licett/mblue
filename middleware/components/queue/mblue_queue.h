/*
 * =====================================================================================
 *
 *       Filename:  mblue_queue.h
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

#ifndef  MBLUE_QUEUE_INC
#define  MBLUE_QUEUE_INC
#include "mblue_stddefine.h"
#define	MAX_MESSAGE_QUEUE_ITEMS			128			/*  */

struct mblue_queue
{
	int    size;
	int    next;
	int    capacity;
	void** data;
	//struct common_bundle *cb;

	int (*init)(struct mblue_queue *mq, uint8_t *data, uint32_t size);
	int (*uninit)(struct mblue_queue *mq);
	int (*add)(struct mblue_queue *mq, void *value);
	int (*get_size)(struct mblue_queue *mq);
	void* (*peek)(struct mblue_queue *mq);
	void* (*remove)(struct mblue_queue *mq);
};
struct mblue_queue *mblue_queue_alloc(uint32_t size);
int mblue_queue_release(struct mblue_queue *mq);
mblue_errcode mblue_queue_construct(struct mblue_queue *, uint8_t *, uint32_t);
#endif   /* ----- #ifndef MBLUE_QUEUE_INC  ----- */
