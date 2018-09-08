/*
 * =====================================================================================
 *
 *       Filename:  message_queue.c
 *
 *    Description:  a simple message queue
 *
 *        Version:  1.0
 *        Created:  2017/4/13 19:57:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "message_queue.h"
#include "mblue_assert.h"
#include "mblue_heap.h"
#include "custom_int.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  _queue_at_capacity
 *  Description:   
 *  @return:0 if the queue's size is smaller that its capacity. 
 *		1 otherwise.
 * =====================================================================================
 */
static inline int _queue_at_capacity(struct message_queue* queue)
{
	return queue->size >= queue->capacity;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  queue_init
 *  Description:  Allocates and returns a new queue. The capacity 
 *  @param size:specifies the maximum number of items that can be stored 
 *  in the queue at one time.
 *  @return NULL if initialization failed and valid message_queue obj otherwise.
 * =====================================================================================
 */
static int queue_init(struct message_queue *mq, uint32_t size)
{
	if (size > MAX_MESSAGE_QUEUE_ITEMS)
	{
		return -1;
	}

	void **data = (void **)mblue_malloc(size * sizeof(void *));
	//cb  = (struct common_bundle *)mblue_malloc(size * sizeof(struct common_bundle));
	if (!data)
	{
		// In case of mblue_free(NULL), no operation is performed.
		return -1;
	}

	mq->size = 0;
	mq->next = 0;
	mq->capacity = size;
	mq->data = data;
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  queue_uninit
 *  Description:  releases the queue resources.
 * =====================================================================================
 */
static int queue_uninit(struct message_queue* queue)
{
	mblue_free(queue->data);
	mblue_free(queue);
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  queue_add
 *  Description:  Enqueues an item in the queue
 *  @return 0: add succeeded
 *	-1: add failed
 * =====================================================================================
 */
static int queue_add(struct message_queue* queue, void* value)
{
	uint32_t key = os_enter_critical();
	if (_queue_at_capacity(queue))
	{
		_ASSERT(FALSE);
		os_exit_critical(key);
		return -1;
	}

	int pos = queue->next + queue->size;
	if (pos >= queue->capacity)
	{
		pos -= queue->capacity;
	}

	/*cb = queue->cb[pos]; 
	cb->value = value;
	cb->info = info;*/
	queue->data[pos] = value;
	queue->size++;
	os_exit_critical(key);
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  queue_remove
 *  Description:  Dequeues an item from the head of the queue
 *  @return NULL if the queue is empty
 * =====================================================================================
 */
static void* queue_remove(struct message_queue* queue)
{
	void* value = NULL;

	uint32_t key = os_enter_critical();
	if (queue->size > 0)
	{
		value = queue->data[queue->next];
		queue->next++;
		queue->size--;
		if (queue->next >= queue->capacity)
		{
			queue->next -= queue->capacity;
		}
	}
	os_exit_critical(key);
	return value;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  queue_peek
 *  Description:  Returns, but does not remove, the head of the queue. 
 *  @return NULL if the queue is empty
 * =====================================================================================
 */
/*static void* queue_peek(struct message_queue* queue)
{
	return queue->size ? queue->data[queue->next] : NULL;
}*/

struct message_queue *mblue_mq_alloc(uint32_t size)
{
	struct message_queue *mq = (struct message_queue *)mblue_malloc
					(sizeof(struct message_queue));
	if (!mq) {
		_ASSERT(0);
		goto fail_wrapper;
	}

	mq->init = queue_init;
	mq->add = queue_add;
	mq->remove = queue_remove;
	mq->uninit = queue_uninit;

	if (!mq->init(mq, size)) {
		return mq;
	}

	mblue_free(mq);
fail_wrapper:
	return NULL;
}

int mblue_mq_release(struct message_queue *mq)
{
	return mq->uninit(mq);
}
