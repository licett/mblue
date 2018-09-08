/*
 * =====================================================================================
 *
 *       Filename:  event_helper.h
 *
 *    Description:  helper mblue_errcodeerface to schedule delayed event
 *
 *        Version:  1.0
 *        Created:  2017/5/17 17:56:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  event_helper_INC
#define  event_helper_INC
#include "event_queue.h"
#include "mblue_stddefine.h"
#include "mblue_errcode.h"
#include "mblue_message.h"

#define	default_timer_wakeup(param)								\
	do {											\
		struct event_queue_head *q;							\
		uint32_t type;									\
		q = &delayed_event_queue;							\
		if (POINTER_S(data)) {								\
			type = ((struct event_queue_element *)(GET_POINTER(data)))->type;	\
			q->wakeup(q, type, MAKE_INTEGER(MBLUE_ERR_TIMEOUT));			\
		}										\
	} while (0)

#define	__query_pending_event(type)						\
	delayed_event_queue.get_pending_event(&delayed_event_queue) & type

#define	__add_pending_event(type, timeout, data)				\
	add_pending_event(&delayed_event_queue,					\
		type,								\
		timeout,							\
		((struct mblue_segment *)(					\
			delayed_event_queue.context))->processing_msg,		\
		data, NULL, NULL, NULL,						\
		_op_complete)

#define	__pending_event_notify(type, data)					\
	delayed_event_queue.wakeup(&delayed_event_queue, type, data)

mblue_errcode add_pending_event(struct event_queue_head *q, uint32_t type, 
	uint32_t timeout, struct mblue_message *msg, void *data,
	mblue_errcode (*start_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*awaken_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*cancel_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*finish_cb)(struct event_queue_element *ele, void *param));
mblue_errcode add_pending_event_with_timeout(struct event_queue_head *q,
	uint32_t type, uint32_t timeout, struct mblue_message *msg,
	mblue_errcode (*start_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*awaken_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*cancel_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*finish_cb)(struct event_queue_element *ele, void *param));
mblue_errcode add_simple_pending_event(struct event_queue_head *q, uint32_t type, 
	struct mblue_message *msg, 
	mblue_errcode (*finish_cb)(struct event_queue_element *ele, void *param));

mblue_errcode schedule_event(struct event_queue_head *q, uint32_t type, 
	uint32_t timeout, void *data,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param));
mblue_errcode schedule_delayed_event_with_type(struct event_queue_head *q,
	uint32_t timeout, uint32_t type,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param));
mblue_errcode reschedule_delayed_event_with_type(struct event_queue_head *q, 
	uint32_t timeout, uint32_t type,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param));
mblue_errcode schedule_default_event(struct event_queue_head *q, 
	uint32_t timeout,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param));
mblue_errcode cancel_delayed_event(struct event_queue_head *q, uint32_t event);
void default_timer_event(void *param);
#endif   /* ----- #ifndef event_helper_INC  ----- */
