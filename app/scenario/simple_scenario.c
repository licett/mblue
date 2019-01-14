/*
 * =====================================================================================
 *
 *       Filename:  locator.c
 *
 *    Description:  application entry of locator
 *
 *        Version:  1.0
 *        Created:  12/29/2017 07:09:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_stddefine.h"
#include "mblue_macro.h"
#include "mblue_task.h"
#include "mblue_message.h"
#include "mblue_heap.h"
#include "mblue_logger.h"
#include "mblue_global_ipc_method.h"
#include "event_helper.h"

#include "simple_scenario.h"

#define	REMOVE_COMPILER_ELEMENT_GUARD(a, b)		\
	(void)a;(void)b

static DECLARE_EVENT_QUEUE_HEAD(delayed_event_queue);
static size_t count = 0;

static mblue_errcode delayed_work(struct event_queue_element *e, void *data)
{
	REMOVE_COMPILER_ELEMENT_GUARD(e, data);

	LOGGER(LOG_INFO, "delayed work");
	/*if (count++ < 10) {
		return MBLUE_OK;
	}*/
	return MBLUE_OK;
	//return MBLUE_ELEMENT_EXIT;
}

static mblue_errcode timer_wakeup(void *obj, void *data)
{
	struct event_queue_head *q;
	mblue_errcode rc;
	uint32_t type;

	q = &delayed_event_queue;
	if (POINTER_S(data)) {
		type = ((struct event_queue_element *)(GET_POINTER(data)))->type;
		rc = MBLUE_ERR_TIMEOUT;
		q->wakeup(q, type, MAKE_INTEGER(rc));
	}

	return MBLUE_OK;
}

static struct handler_item signal_items[] = {
	HANDLER_ITEM(MBLUE_TIMER_WAKEUP, timer_wakeup),
	HANDLER_ITEM(END_ITEM_ID, NULL),
};

//@override
static void scenario_launch(struct mblue_segment *ms)
{
	delayed_event_queue.init(&delayed_event_queue, ms);
	delayed_event_queue.__timer_function = default_timer_event;
	schedule_default_event(&delayed_event_queue, 1000, delayed_work);
}

//@override
static int scenario_init(struct	SIMPLE_SCENARIO *s)
{
	struct mblue_segment *ms;

	// 1.init super segment
	ms = &s->ms_entity;
	init_segment(ms);

	// 2.setup basic info and handler
	ms->major	= MBLUE_SIMPLE_SCENARIO;
	ms->on_launch	= scenario_launch;
	ms->items_array[SIGNAL]		= signal_items;
	
	return 0;
}

DECLARE_SINGLETON_SCENARIO(SIMPLE_SCENARIO)
