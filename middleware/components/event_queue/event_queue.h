/*
 * =====================================================================================
 *
 *       Filename:  event_queue.h
 *
 *    Description:  basic definition of event queue
 *
 *        Version:  1.0
 *        Created:  2017/5/15 19:44:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  event_queue_INC
#define  event_queue_INC
#include "mblue_stddefine.h"

#define	EVENT_TIMEOUT_FOREVER		0xFFFFFFFF			/*  */
#define	EVENT_QUEUE_ELEMENT_MAGIC	0x4D454C45			/* asc of 'ELEM' */
enum EVENT_CALL_CHAIN_INDEX {
	CALL_START,
	CALL_WAKEUP,
	CALL_CANCEL,
	CALL_FINISH,
	CALL_CHAIN_MAX
};

enum EVENT_MODE {
	EVENT_MODE_PENDING_EVENT,
	EVENT_MODE_EVENT_PERIODICAL,
	EVENT_MODE_MAX
};

enum EVENT_ACTION {
	EVENT_ACTION_WAKEUP,
	EVENT_ACTION_REMOVE,
	EVENT_ACTION_FINISH,
	EVENT_ACTION_MAX
};

struct event_queue_element {
#ifdef DEBUG
	uint32_t magic;
#endif
	uint32_t type;
	uint32_t mode;
	uint32_t period;
	struct mblue_timer *tick;
	void *src;
	void *head;
	void *smart_data;

	mblue_errcode (*call_chain[CALL_CHAIN_MAX])(struct event_queue_element *ele, void *param);
	struct list_head element_list;
};

struct event_queue_head {
	uint32_t event_bm;
	struct list_head queue;
	void *private;
	void *context;
	//timer call back that will be shared by all elements
	//so make sure that this function must be reentrant
	void (*__timer_function)(void *);

	mblue_errcode (*init)(struct event_queue_head *head, void *context);
	uint32_t (*get_pending_event)(struct event_queue_head *head);
	struct event_queue_element *(*get_element)(struct event_queue_head *head, uint32_t type);
	mblue_errcode (*add)(struct event_queue_head *head, struct event_queue_element *ele);
	mblue_errcode (*del)(struct event_queue_head *head, uint32_t type);
	mblue_errcode (*wakeup)(struct event_queue_head *head, uint32_t type, void *param);
	mblue_errcode (*uninit)(struct event_queue_head *head);
};

mblue_errcode event_queue_init(struct event_queue_head *queue_head, void *context);
mblue_errcode event_queue_add(struct event_queue_head *queue_head, struct event_queue_element *element);
mblue_errcode event_queue_delete(struct event_queue_head *queue_head, uint32_t type);
uint32_t event_queue_get_pending_event(struct event_queue_head *queue_head);
struct event_queue_element *event_queue_get_element(
	struct event_queue_head *head, uint32_t type);
mblue_errcode event_queue_wakeup(struct event_queue_head *queue_head, uint32_t type, void *param);
mblue_errcode event_queue_uninit(struct event_queue_head *queue_head);

#define __EVENT_QUEUE_HEAD_INITIALIZER(name) {					\
        .event_bm		= 0,						\
        .context		= NULL,						\
        .queue			= {&(name).queue, &(name).queue},		\
	.__timer_function	= NULL,						\
	.init			= event_queue_init,				\
	.add			= event_queue_add,				\
	.del			= event_queue_delete,				\
	.get_pending_event	= event_queue_get_pending_event,		\
	.get_element		= event_queue_get_element,			\
	.wakeup			= event_queue_wakeup,				\
	.uninit			= event_queue_uninit				\
}
#define DECLARE_EVENT_QUEUE_HEAD(name)						\
        struct event_queue_head name = __EVENT_QUEUE_HEAD_INITIALIZER(name)
#endif   /* ----- #ifndef event_queue_INC  ----- */
