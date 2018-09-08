/*
 * =====================================================================================
 *
 *       Filename:  event_queue.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/5/16 15:35:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "mblue_stddefine.h"
#include "mblue_assert.h"
#include "mblue_list.h"
#include "event_queue.h"
#include "mblue_heap.h"
#include "mblue_timer.h"
#include "mblue_logger.h"
#include "mblue_macro.h"
#include "event_queue.h"

static void __free_event_element(struct event_queue_element *ele)
{
	struct mblue_timer *timer;	
	if (ele->tick) {
		timer = ele->tick;
		timer->stop(timer);
		timer->uninit(timer);
		mblue_timer_release(timer);
	}
	if (ele->smart_data && PB_PAYLOAD(ele->smart_data)) {
		mblue_free(ele->smart_data);
	}
	list_del(&ele->element_list);
	mblue_free(ele);
}


mblue_errcode event_queue_init(struct event_queue_head *queue_head, void *context)
{
	queue_head->event_bm = 0;
	INIT_LIST_HEAD(&queue_head->queue);
	queue_head->context = context;
	return MBLUE_OK;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  event_queue_add
 *  Description:  get the pending event bit map from a event queue
 * =====================================================================================
 */
uint32_t event_queue_get_pending_event(struct event_queue_head *queue_head)
{
	return queue_head->event_bm;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  event_queue_get_element
 *  Description:  get a element from event queue
 * =====================================================================================
 */
struct event_queue_element *event_queue_get_element(
	struct event_queue_head *head, uint32_t type)
{
	struct list_head *node, *n;
	struct event_queue_element *ele;

	list_for_each_safe(node, n, &(head->queue)) {
		ele = list_entry(node, struct event_queue_element, element_list);
		_ASSERT(ele);
		if (ele->type != type) {
			continue;
		}
		return ele;
	}
	return NULL;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  event_queue_add
 *  Description:  add a delayed event to queue. The handler of CALL_START will be 
 *  called if it exits.
 * =====================================================================================
 */
mblue_errcode event_queue_add(struct event_queue_head *queue_head, 
	struct event_queue_element *element)
{
	struct mblue_timer *timer;
	struct event_queue_element *ele;
	mblue_errcode ret;

	ret = MBLUE_ERR_UNKNOWN;
	if (queue_head->event_bm & element->type) {
		LOGGER(LOG_DEBUG," event type %x pending twice\n", element->type);
		_ASSERT(0);
		goto fail;
	}

	ele = (struct event_queue_element *)mblue_malloc(
		sizeof(struct event_queue_element));
	if (!ele) {
		ret = MBLUE_ERR_NOMEM;
		goto fail;
	}

	ret = MBLUE_OK;
	memcpy(ele, element, sizeof(struct event_queue_element));
#ifdef DEBUG
	ele->magic = EVENT_QUEUE_ELEMENT_MAGIC;
#endif
	ele->head = queue_head;
	queue_head->event_bm |= element->type;
	list_add_tail(&ele->element_list, &queue_head->queue);

	if (ele->period && ele->period != EVENT_TIMEOUT_FOREVER) {
		_ASSERT(queue_head->__timer_function);
		//timer = mblue_timer_get(queue_head->context, ele);
		timer = mblue_timer_get();
		_ASSERT(timer);
		timer->init(timer, 
			queue_head->__timer_function,
			ele->period, ele->mode, (void *)ele);
		timer->start(timer);
		ele->tick = timer;
	}
	if (ele->call_chain[CALL_START]) {
		ret = (ele->call_chain[CALL_START])(ele, queue_head);
	}
fail:
	return ret;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  event_queue_delete
 *  Description:  delete a exist event from queue 
 * =====================================================================================
 */
mblue_errcode event_queue_delete(struct event_queue_head *queue_head, uint32_t type)
{
	struct list_head *node, *n;
	struct event_queue_element *ele;
	mblue_errcode ret;

	ret = MBLUE_ERR_UNKNOWN;
	list_for_each_safe(node, n, &(queue_head->queue)) {
		ele = list_entry(node, struct event_queue_element, element_list);
		_ASSERT(ele);
		if (ele->type != type) {
			continue;
		}

		// find the event type
		ret = MBLUE_OK;
		queue_head->event_bm &= ~type;
		__free_event_element(ele);
	}

	return ret;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  event_queue_wake
 *  Description:  wakeup corresponding event element in task context.
 *  Note that handler of CALL_WAKEUP could trigger CALL_FINISH and CALL_CANCEL, or
 *  return normally.
 *
 *  For example, if uart return 4 data fragments within a at session, function 
 *  CALL_WAKEUP will be called 4 times. 
 *  Except the last one, which will trigger CALL_FINISH in call chain, the
 *  other three ones will return normally(return value 0).
 * =====================================================================================
 */
mblue_errcode event_queue_wakeup(struct event_queue_head *queue_head, 
	uint32_t type, void *param)
{
	struct list_head *node, *n;
	struct event_queue_element *ele;
	mblue_errcode ret;
	int waked;

	waked	= -1;
	ret	= MBLUE_ELEMENT_EXIT;
	if (!(queue_head->event_bm & type)) {
		LOGGER(LOG_DEBUG," no event %x pending\n", type);
		goto exit_point;
	}

	waked = 0;
	list_for_each_safe(node, n, &(queue_head->queue)) {
		ele = list_entry(node, struct event_queue_element, element_list);
		_ASSERT(ele);
		if (ele->type != type) {
			continue;
		}

		// find the event type
		if (ele->call_chain[CALL_WAKEUP]) {
			ret = (ele->call_chain[CALL_WAKEUP])(ele, param);
			if (ret == MBLUE_OK) {
				goto exit_point;  
			}
			if (ret > 0 && ele->call_chain[ret]) {
				ret = (ele->call_chain[ret])(ele, param);
			}
		} else if (ele->call_chain[CALL_FINISH]) {
			ret = (ele->call_chain[CALL_FINISH])(ele, param);
		} else if (ele->call_chain[CALL_CANCEL]) {
			ret = (ele->call_chain[CALL_CANCEL])(ele, param);
		} else {
			//LOGGER(" no wakeup or finish handler of type %d \n", type);
		}
		if (ret == MBLUE_ELEMENT_EXIT) {
			queue_head->event_bm &= ~ele->type;
			__free_event_element(ele);
		}
		break;
	}

exit_point:
	return waked ? MBLUE_ERR_UNKNOWN : MBLUE_OK;
}

mblue_errcode event_queue_uninit(struct event_queue_head *queue_head)
{
	return MBLUE_OK;
}
