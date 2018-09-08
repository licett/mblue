/*
 * =====================================================================================
 *
 *       Filename:  rpc_helper.h
 *
 *    Description:  rpc helper
 *
 *        Version:  1.0
 *        Created:  2017/4/19 11:30:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mblue_stddefine.h"
#include "task_manager.h"
#include "rpc_helper.h"
#include "system_bus.h"

#define	INIT_MESSAGE_WITH(msg, task, s, r, d, c, in, e)			\
		do {							\
			msg->seq = s;					\
			msg->ref = r;					\
			msg->code = c;					\
			msg->src = task;				\
			msg->dst = d;					\
			msg->smart_payload = in;			\
			msg->extra = e;					\
		} while (0)

static struct mblue_message *init_message(
	struct mblue_task *task, 
	uint16_t seq, uint8_t type,
	uint16_t major, uint16_t minor, 
	void *dst,
	void *in, 
	void **out, 
	void *extra, struct pending_notifier *pn)
{
	uint32_t code;
	uint16_t ref;
	struct mblue_message *msg = mblue_message_alloc();
	if (!msg) {
		goto fail;
	}

	ref = 0;
	code = type << TYPE_OFFSET;
	code |= major << MAJOR_OFFSET;
	code |= minor;

	INIT_MESSAGE_WITH(msg, task, seq, ref, dst, code, in, extra);
	if (type == SYNC_CALL || type == ASYNC_CALL) {
		msg->ref++;
		if (type == SYNC_CALL) {
			msg->rpc_return = out;
		} else {
			msg->pn = pn;
		}
	}
fail:
	return msg;
}

mblue_errcode mblue_message_post(
	struct mblue_task *task, 
	uint16_t seq, uint8_t type,
	uint16_t major, uint16_t minor, 
	void *in, 
	void **out, void *extra, struct pending_notifier *pn)
{
	struct mblue_rpc *r;
	struct mblue_message *msg;
	struct system_bus *sb;

	sb = get_system_bus_instance();
	r = rpc_get_instance();
	/*msg = init_message(task, seq, type, major, minor, 
				sb->get_segment(sb, MBLUE_BUS), data, extra);*/
	msg = init_message(task, seq, type, major, minor, 
			sb->get_segment(sb, MBLUE_BUS), in, out, extra, pn);
	if (!msg) {
		return MBLUE_ERR_NOMEM;
	}
	return r->invoke(r, msg);
}

mblue_errcode mblue_subscribe(struct mblue_segment *ms, uint16_t major)
{
	return mblue_message_post(
		ms->get_context(ms), 
		mblue_message_get_sequence(), 
		SUBSCRIBE, major, 0, NULL, NULL, ms, NULL);
}

void *mblue_remote_call(uint16_t major, uint16_t minor, void *in)
{
	mblue_errcode rc;
	void *out;

	rc = mblue_message_post(
		GET_CURRENT_CONTEXT(),			
		mblue_message_get_sequence(), 
		SYNC_CALL,		
		major, minor, 
		in, &out, 
		NULL, NULL);
	_ASSERT(rc == MBLUE_OK);

	return out;
}

mblue_errcode mblue_remote_call_async(uint16_t major, uint16_t minor, 
	void *in,
	void (*notify)(void *, void *user_data),
	void *data)
{
	mblue_errcode rc;
	struct pending_notifier pn = {notify, data};

	rc = mblue_message_post(
		GET_CURRENT_CONTEXT(),			
		mblue_message_get_sequence(), 
		ASYNC_CALL,		
		major, minor, 
		in, 
		NULL, NULL, &pn);
	_ASSERT(rc == MBLUE_OK);

	return rc;
}


mblue_errcode target_signal(struct mblue_task *source, struct mblue_task *target,
	uint16_t major, uint16_t minor, 
	void *dest,
	void *data, void *extra)
{
	struct mblue_message *msg;

	msg = init_message(source, 
		mblue_message_get_sequence(), 
		SIGNAL, 
		major, minor, 
		dest,
		data, NULL, extra, NULL);
	if (!msg) {
		return MBLUE_ERR_NOMEM;
	}
	msg->ref++;
	return (target->nproc)(target, msg);
}

/*int message_register(struct mblue_task *task, uint16_t major)
{
	return system_message_post(task, mblue_message_get_sequence(), 
					REGISTER, major, 0, NULL, NULL);
}*/

/*int uart_write(struct mblue_task *task, uint8_t *data, int len, 
	int (*notify)(struct message *msg))
{
	return system_message_post(task, message_get_sequence(), SIGNAL, 
		DEVICE, UART, MBLUE_DEVICE_UART_WRITE, data, (void *)notify);
}*/
