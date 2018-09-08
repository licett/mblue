/*
 * =====================================================================================
 *
 *       Filename:  mblue_segment.c
 *
 *    Description:  a segment, an independent logical module, usually exists as a 
 *			micro-service or business processor unit.
 *
 *        Version:  1.0
 *        Created:  2017/10/19 11:07:11
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
#include "mblue_message.h"
#include "mblue_segment.h"
#include "mblue_heap.h"
#include "task_manager.h"
#include "data_schema.h"
#include "rpc_helper.h"
#include "mblue_assert.h"

#include "pb_schema.h"
#include "pb_decode.h"

static void *mblue_segment_get_context(struct mblue_segment *ms)
{
	return ms->context;
}

static void mblue_segment_set_context(struct mblue_segment *ms, void *context)
{
	ms->context = context;
}

static mblue_segment_handle_t mblue_segment_get_handler(
	struct mblue_segment *ms, int type)
{
	return ms->handler_array[type];
}

static void mblue_segment_set_handler(struct mblue_segment *ms, int type, 
			mblue_segment_handle_t external_handle)
{
	_ASSERT(type < MESSAGE_TYPE_MAX);
	ms->handler_array[type] = external_handle;
}

static void __link_msg(struct mblue_segment *ms, struct mblue_message *msg)
{
	ms->processing_msg = msg;
}

static mblue_errcode default_signal_handler(struct mblue_segment *ms, void *data)
{
	mblue_errcode rc;
	uint16_t minor;
	struct mblue_message *msg;
	struct handler_item *signal_array, *p;
	int (*signal_handler)(void *obj, void *data);

	_ASSERT(ms && data);
	msg = (struct mblue_message *)data;
	minor = GET_MINOR(msg);	

	__link_msg(ms, msg);
	signal_array = ms->items_array[SIGNAL];
	for (p = signal_array; p != NULL && p->id != END_ITEM_ID; p++) {
		// pickup the dest handler
		if (p->id == minor) {
			signal_handler = (int (*)(void *obj, void *data))p->handler;
			rc =  (mblue_errcode)((signal_handler)(ms, msg->smart_payload));
			_ASSERT(rc == MBLUE_OK);
			break;
		}
	}
	if (PB_PAYLOAD(msg->smart_payload)) {
		mblue_free(msg->smart_payload);
	}
	mblue_message_release(msg);
	return rc;
}

static mblue_errcode default_rpc_handler(struct mblue_segment *ms, void *data)
{
	void *rpc_func;
	mblue_errcode rc;
	SMART_DATA ret;
	uint16_t index;
	struct mblue_message *msg;
	struct _dl_rpc_header rpc_header;
	pb_istream_t stream;
	mblue_pb_parser_func_t data_parser;

	rc = MBLUE_ERR_PB_DECODE;
	_ASSERT(ms && data);
	index = PB_MAX;
	msg = (struct mblue_message *)data;

	__link_msg(ms, msg);
	if (PB_PAYLOAD(msg->smart_payload)) {
		stream = pb_istream_from_buffer(msg->smart_payload, SIZE_MAX);
		if (!pb_decode_delimited(&stream, dl_rpc_header_fields, &rpc_header)) {
			goto exited;
		}
		index = rpc_header.layout_index;
	}

	rpc_func = find_rpc_handler_from_service(ms, msg);
	if (rpc_func) {
		/*ms->pending_caller = msg;*/
		_ASSERT(index != PB_MAX);
		data_parser = mblue_pb_data_parser_get(index);
		ret = data_parser(&stream, rpc_func);
		if (!(INTEGER_P(ret) && 
			((mblue_errcode)GET_INTEGER(ret)) == MBLUE_REMOTE_CALL_PENDING)) {
			rc = rpc_return(GET_MAJOR(msg), msg->seq, ret);
		}
	}

	if (PB_PAYLOAD(msg->smart_payload)) {
		mblue_free(msg->smart_payload);
	}
	mblue_message_release(msg);
exited:
	return rc;
}

static void default_launch(struct mblue_segment *ms)
{
	_ASSERT(ms->magic == MBLUE_SEGMENT_MAGIC);
	//_ASSERT(ms->major);
}

void *find_rpc_handler_from_service(struct mblue_segment *ms, struct mblue_message *msg)
{
	uint16_t minor;
	struct handler_item *rpc_array, *p;

	minor = GET_MINOR(msg);	
	rpc_array = ms->items_array[SYNC_CALL];

	for (p = rpc_array; p != NULL && p->id != END_ITEM_ID; p++) {
		if (p->id == minor) {
			return p->handler;
		}
	}
	return NULL;
}

void init_segment(struct mblue_segment *ms)
{
	memset(ms, 0, sizeof(struct mblue_segment));
	ms->set_context			= mblue_segment_set_context;
	ms->get_context			= mblue_segment_get_context;
	ms->get_handler			= mblue_segment_get_handler;
	ms->set_handler			= mblue_segment_set_handler;

	ms->magic			= MBLUE_SEGMENT_MAGIC;
	ms->handler_array[SIGNAL]	= default_signal_handler;
	ms->handler_array[SYNC_CALL]	= default_rpc_handler;
	ms->handler_array[ASYNC_CALL]	= default_rpc_handler;
	ms->on_launch			= default_launch;
}
