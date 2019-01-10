/*
 * =====================================================================================
 *
 *       Filename:  system_bus.c
 *
 *    Description:  
 *
 *	System bus is a msg queue. It should be given the highest priority in user space.
 *	The main feature of system bus:
 *
 *	1.receive all messages from different segments and distribute these messages
 *	to segments that need these messages(a typical sub/pub).
 *
 *	2.provide ipc interface to other threads.
 *
 *	TODO
 *	3.System bus is also the factory of the whole system and it manages the 
 *	dependency of each other.
 *
 *        Version:  1.0
 *        Created:  2017/4/7 11:46:48
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
#include "mblue_list.h"
#include "mblue_assert.h"
#include "mblue_message.h"
#include "mblue_segment.h"
#include "task_manager.h"
#include "system_bus.h"
#include "mblue_heap.h"
#include "mblue_semaphore.h"
#include "mblue_logger.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_init
 *  Description:  init the system bus
 *  @param bus: object of system bus
 *  @return 0:success
 *	   -1:fail
 * =====================================================================================
 */
static mblue_errcode bus_init(struct system_bus *bus)
{
	int i;

	for (i = 0; i < SEGMENT_MAJOR_MAX; i++) {
		INIT_LIST_HEAD(&(bus->_segment_node[i].publish_list));
		INIT_LIST_HEAD(&(bus->_segment_node[i].call_list));
	}
	mblue_semaphore_construct(&bus->init_lock);
	return MBLUE_OK;
}

/* 
 * ===  function  ======================================================================
 *         name:  bus_register
 *  description:  register current segment to global segments list
 *  @param bus:object of system bus
 *  @param ms:object of current segment
 * =====================================================================================
 */
static void bus_register(struct system_bus *bus, struct mblue_segment *ms)
{
	mblue_errcode rc;
	struct mblue_semaphore *init;

	_ASSERT(ms);
	_ASSERT(ms->magic == MBLUE_SEGMENT_MAGIC);

	init = &bus->init_lock;
	if (ms->major != MBLUE_BUS) {
		rc = init->pend(init, SEMAPHORE_PEND_FOREVER);
		_ASSERT(!rc);
	}
	bus->_segment_node[ms->major].ms = ms;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_init_notify
 *  Description:  give a notification to other segments that bus is ready
 *  @return 0:	success
 *     others:	fail
 * =====================================================================================
 */
static mblue_errcode bus_contex_notify(struct system_bus *bus)
{
	size_t i;
	mblue_errcode rc;
	struct mblue_semaphore *init;

	init = &bus->init_lock;
	for (i = 0; i < SEGMENT_MAJOR_MAX; i++) {
		rc |= init->post(init);		
		_ASSERT(!rc);
	}

	return rc;
}

/* 
 * ===  function  ======================================================================
 *         name:  bus_get_segment
 *  description:  get mblue segment from global segment list
 *  @param bus:object of system bus
 *  @param major:index of global segment list
 *  @return mblue segment object
 * =====================================================================================
 */
static struct mblue_segment *bus_get_segment(struct system_bus *bus, uint16_t major)
{
	return bus->_segment_node[major].ms;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_subscribe
 *  Description:  register current segment to target segment's publish list
 *  @param bus:object of system bus
 *  @param major:the major number of target segment 
 *  @param ms:object of current segment
 *  @return errcode
 * =====================================================================================
 */
static mblue_errcode bus_subscribe(struct system_bus *bus, struct mblue_segment *ms, uint16_t major)
{
	struct subscriber_info *suber;

	_ASSERT(ms);
	_ASSERT(ms->magic == MBLUE_SEGMENT_MAGIC);
	_ASSERT(ms->major != major); // what a strange logic !?
	//_ASSERT(bus->_segment_node[major].ms);

	suber = (struct subscriber_info *)mblue_malloc(sizeof(struct subscriber_info));
	suber->ms = ms;
	list_add_tail(&suber->list, &(bus->_segment_node[major].publish_list));
	return MBLUE_OK;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_signal
 *  Description:  send a signal to system bus
 *  @param bus:object of system bus
 *  @param msg:signal carrier
 *  @return errcode
 * =====================================================================================
 */
static mblue_errcode bus_signal(struct system_bus *bus, struct mblue_message *msg)
{
	uint16_t major;
	struct list_head *node;
	struct mblue_segment *ms;
	struct subscriber_info *suber;
	struct mblue_message *msg_replica;
	struct mblue_task *src, *dst;
	mblue_errcode rc;

	src = (struct mblue_task *)msg->src;
	major = GET_MAJOR(msg);
	_ASSERT(src);
	_ASSERT(major);

	rc = MBLUE_ERR_UNKNOWN;
	if (list_empty(&(bus->_segment_node[major].publish_list))) {
		LOGGER(LOG_WARN," ms:%d has no publish list\r\n", major);
		goto exit_point;
	}

	list_for_each(node, &(bus->_segment_node[major].publish_list)) {
		suber = list_entry(node, struct subscriber_info, list);
		ms = suber->ms;
		if (!ms 
#ifdef DEBUG
			|| (ms->magic != MBLUE_SEGMENT_MAGIC)
#endif
			) {
			LOGGER(LOG_ERROR," found err in publish list err\n");
			_ASSERT(FALSE);
			rc = MBLUE_ERR_CORRUPT_MEM;
			goto exit_point;
		}

		rc = MBLUE_OK;
		dst = (struct mblue_task *)(ms->get_context(ms));
		if (dst && dst->nproc) {
			//TODO add deep copy to meet broadcast demands
			msg_replica = mblue_message_alloc();
			memcpy(msg_replica, msg, sizeof(struct mblue_message));
			get_mblue_message(msg_replica);
			msg_replica->dst = (void *)ms;
			(dst->nproc)(dst, msg_replica);
		}
	}

exit_point:
	mblue_message_release(msg);
	return rc;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_call
 *  Description:  call a remote method 
 *  @param bus:object of system bus
 *  @param type:call type
 *  @param callback:function will be called within bus context when ipc is done 
 *	NOTE that you should not do time-consuming operation in callback
 *  @return 0:success
 *	   -1:fail
 * =====================================================================================
 */
/*static int bus_call(struct system_bus *bus, struct mblue_message *msg,
		int (*callback)(struct mblue_message *msg1, struct mblue_message *msg2))*/
static mblue_errcode bus_call(struct system_bus *bus, struct mblue_message *msg, void *union_info)
{
	int type;
	mblue_errcode rc;
	uint16_t major;
	struct mblue_segment *ms;
	struct pending_call *call;
	struct pending_notifier *p, *q;
	struct mblue_task *dst;

	rc = MBLUE_ERR_UNKNOWN;
	type = GET_TYPE(msg);
	major = GET_MAJOR(msg);

	ms = bus->_segment_node[major].ms;
	if (!ms
#ifdef DEBUG
		|| (ms->magic != MBLUE_SEGMENT_MAGIC)
#endif
		|| (ms->get_context(ms) == NULL)
		) {
		_ASSERT(FALSE);
		LOGGER(LOG_ERROR," uninit or corrupt root segment:%d\r\n", major);
		goto exit_point;
	}

	if (!(type == ASYNC_CALL && union_info == NULL)) {
		call = (struct pending_call *)
			mblue_malloc(sizeof(struct pending_call));
		if (!call) {
			_ASSERT(FALSE);
			LOGGER(LOG_ERROR," malloc pending call failed\r\n");
			goto exit_point;
		}

		call->msg = msg;
                call->call_type = type;
		switch(type) {
		case SYNC_CALL:
			msg->ipc_return = (void **)union_info;
			break;
		case ASYNC_CALL:
			q = (struct pending_notifier *)union_info;
			/*p = (struct pending_notifier *)mblue_malloc(sizeof(struct pending_notifier));
			if (p) {
				memcpy(p, q, sizeof(struct pending_notifier));
			}*/
			call->notifier = q;
			break;
		default:
			_ASSERT(FALSE);
			break;
		}
		list_add_tail(&(call->list), 
			&(bus->_segment_node[major].call_list));
	}

	dst = (struct mblue_task *)(ms->get_context(ms));
	if (!dst->nproc) {
		goto free_call;
	}

	rc = MBLUE_OK;
	get_mblue_message(msg);
	msg->dst = ms;
	(dst->nproc)(dst, msg);

free_call:
	if (rc) {
		mblue_free(call);
	}
exit_point:
	return rc;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  bus_call_return
 *  Description:  a remote call return 
 *  @param bus:object of system bus
 *  @param msg:return mblue_message
 *  @return 0:success
 *	   -1:fail
 * =====================================================================================
 */
static mblue_errcode bus_call_return(struct system_bus *bus, struct mblue_message *dmsg)
{
	mblue_errcode rc;
	uint16_t major;
	uint8_t **p, *q;
	struct list_head *node;
	struct mblue_task *task;
	struct pending_call *call;
	struct pending_notifier *notifier;
	struct mblue_message *smsg;

	rc = MBLUE_ERR_UNKNOWN;
	major = GET_MAJOR(dmsg);
	if (list_empty(&(bus->_segment_node[major].call_list))) {
		_ASSERT(FALSE);
		LOGGER(LOG_ERROR," no pending call\r\n");
		goto exit_point;
	}

	list_for_each(node, &(bus->_segment_node[major].call_list)) {
		call = list_entry(node, struct pending_call, list);
		if (!call || !(call->msg)) {
			_ASSERT(FALSE);
			rc = MBLUE_ERR_CORRUPT_MEM;
			LOGGER(LOG_ERROR," corrupt call list\r\n");
			break;
		}
		smsg = call->msg;
		if (!smsg || dmsg->seq != smsg->seq) {
			continue;
		}

		switch(call->call_type) {
		case SYNC_CALL:
			task = (struct mblue_task *)smsg->src;
			if (!task) {
				break;
			}
			p = (uint8_t **)smsg->ipc_return;
			q = (uint8_t *)dmsg->smart_payload;
			*p = q;
			if (task->tpost) {
				(task->tpost)(task);
			}
			break;
		case ASYNC_CALL:
			_ASSERT(call->notifier);
			notifier = call->notifier;
			notifier->notify(dmsg->smart_payload, notifier->user_data);
			mblue_free(notifier);
			break;
		default:
			_ASSERT(FALSE);
			break;
		}

		rc = MBLUE_OK;
		mblue_message_release(smsg);
		list_del(&call->list);
		mblue_free(call);
		break;
	}

exit_point:
	mblue_message_release(dmsg);
	return rc;
}

static struct system_bus _system_bus = {
	.init = bus_init,
	.contex_notify = bus_contex_notify,
	.get_segment = bus_get_segment,
	.regist = bus_register,
	.subscribe = bus_subscribe,
	.signal = bus_signal,
	.call = bus_call,
	.call_return = bus_call_return,
};

static struct system_bus *sb_instance = NULL;
struct system_bus *get_system_bus_instance(void)
{
	if (!sb_instance) {
		sb_instance = &_system_bus;	
		sb_instance->init(sb_instance);
	}
	return sb_instance;
}
