/*
 * =====================================================================================
 *
 *       Filename:  event_helper.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/8/9 19:09:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_stddefine.h"
#include "mblue_macro.h"
#include "task_manager.h"
#include "mblue_assert.h"
#include "mblue_list.h"
#include "event_helper.h"
#include "mblue_message.h"
#include "ipc_helper.h"

#define	INIT_EVENT_ELEMENT_WITH(ele, y, m, p, s, d, t, st, aw, ca, fi)		\
	do {									\
		ele.type = y;							\
		ele.mode = m;							\
		ele.period = p;							\
		ele.src = s;							\
		ele.smart_data = d;						\
		ele.tick = t;							\
		ele.call_chain[CALL_START] = st;				\
		ele.call_chain[CALL_WAKEUP] = aw;				\
		ele.call_chain[CALL_CANCEL] = ca;				\
		ele.call_chain[CALL_FINISH] = fi;				\
	} while(0)

mblue_errcode add_pending_event(struct event_queue_head *q, uint32_t type, 
	uint32_t timeout, struct mblue_message *msg, void *data,
	mblue_errcode (*start_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*awaken_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*cancel_cb)(struct event_queue_element *ele, void *param),
	mblue_errcode (*finish_cb)(struct event_queue_element *ele, void *param))

{
	struct event_queue_element ele;
	INIT_EVENT_ELEMENT_WITH(ele, type, EVENT_MODE_PENDING_EVENT, timeout, 
		(void *)msg, data, NULL, start_cb, awaken_cb, cancel_cb, finish_cb);

	return q->add(q, &ele);
}

mblue_errcode add_simple_pending_event(struct event_queue_head *q, uint32_t type, 
	struct mblue_message *msg, 
	mblue_errcode (*finish_cb)(struct event_queue_element *ele, void *param))
{
	struct event_queue_element ele;
	INIT_EVENT_ELEMENT_WITH(ele, type, EVENT_MODE_PENDING_EVENT, 0, 
		(void *)msg, NULL, NULL, NULL, NULL, NULL, finish_cb);

	return q->add(q, &ele);
}

mblue_errcode schedule_event(struct event_queue_head *q, uint32_t type, 
	uint32_t timeout, void *data,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param))
{
	struct event_queue_element ele;
	INIT_EVENT_ELEMENT_WITH(ele, type, EVENT_MODE_EVENT_PERIODICAL, 
		timeout, NULL, data, NULL, NULL, _delayed_event, NULL, NULL);

	return q->add(q, &ele);
}

mblue_errcode schedule_delayed_event_with_type(struct event_queue_head *q, 
	uint32_t timeout, uint32_t type,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param))
{
	struct event_queue_element ele;
	INIT_EVENT_ELEMENT_WITH(ele, type, EVENT_MODE_EVENT_PERIODICAL, 
		timeout, NULL, NULL, NULL, NULL, _delayed_event, NULL, NULL);

	if (!(q->get_pending_event(q) & type)) {
		return q->add(q, &ele);
	}
	return MBLUE_OK;
}

mblue_errcode schedule_default_event(struct event_queue_head *q, 
	uint32_t timeout,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param))
{
	return schedule_delayed_event_with_type(q, 
		timeout, MBLUE_EVENT_DELAYED_EVENT, _delayed_event);
}

mblue_errcode reschedule_delayed_event_with_type(struct event_queue_head *q, 
	uint32_t timeout, uint32_t type,
	mblue_errcode (*_delayed_event)(struct event_queue_element *ele, void *param))
{
	if (q->get_pending_event(q) & type) {
		q->del(q, type); 
	}
	return schedule_delayed_event_with_type(q, timeout, type, _delayed_event);
}

mblue_errcode cancel_delayed_event(struct event_queue_head *q, uint32_t event)
{
	if (q->get_pending_event(q) & event) {
		return q->del(q, event); 
	}
	return MBLUE_ERR_INVALID_PARAMETER;
}

void default_timer_event(void *param)
{
	struct event_queue_head *q;
	struct event_queue_element *e;
	struct mblue_segment *ms;

	e = (struct event_queue_element *)param;
	q = e->head;
	_ASSERT(e);
#ifdef DEBUG
	_ASSERT(e->magic == EVENT_QUEUE_ELEMENT_MAGIC);
#endif
	_ASSERT(q);
	_ASSERT(q->context);

	ms = q->context;
	target_signal(
		(struct mblue_task *)MESSAGE_SOURCE_TIMER,
		MBLUE_SYSTEM, MBLUE_TIMER_WAKEUP, 
		ms, MAKE_POINTER_S(param), NULL);
}
