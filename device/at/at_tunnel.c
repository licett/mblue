/*
 * =====================================================================================
 *
 *       Filename:  at_device.c
 *
 *    Description:  basic definition of at devices, such as modem, gps etc  
 *
 *        Version:  1.0
 *        Created:  12/21/2017 01:40:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>
#include "mblue_stddefine.h"
#include "mblue_assert.h"
#include "mblue_heap.h"
#include "mblue_task.h"
#include "event_helper.h"
#include "rpc_helper.h"
#include "at_service.h"
#include "at_tunnel.h"
#include "at_set.h"
#include "at_parser.h"
#include "pb_schema.h"

const char *DUMMY_ACK = "OK";
static enum at_response_type at_tunnel_line_scanner(const char *line, size_t len, void *priv)
{
	struct at_tunnel *tunnel;
	struct at_sets *at_set;
	enum at_response_type rc;

	rc = AT_RESPONSE_UNKNOWN;
	tunnel = (struct at_tunnel *)priv;
	at_set = mblue_at_sets_get(tunnel->at_set_index);

	if (tunnel->cmd_index >= 0) {
		rc = (at_set->at_set_items[tunnel->cmd_index].scanner)(at_set, line, len);
	}
	if (!rc) {
		rc = at_set->urc_scan(at_set, line, len);
	}

	return rc;
}

static void at_tunnel_response_handler(const char *line, size_t len, void *priv)
{
	void *p;
	size_t l;
	const char *q;
	struct at_tunnel *at;
	struct mblue_segment *ms;
	struct event_queue_head *queue;

	_ASSERT(line);
	at = (struct at_tunnel *)priv;
	ms = at->priv;
	q = DUMMY_ACK;  // null means OK or data input pending
	l = strlen(q);

	if (line && len) {
		q = line;
		l = len;
	} 

	queue = ms->queue;
	if (queue->get_pending_event(queue) & ASYNC_EVENT_AT_EXECUTING) {
		at->cmd_index = -1;
		p = mblue_serialize_mono_buffer(q, l);
		queue->wakeup(queue, ASYNC_EVENT_AT_EXECUTING, p);
	}
}

static void at_tunnel_urc_handler(const char *line, size_t len, void *priv)
{
	struct at_tunnel *at;
	at = (struct at_tunnel *)priv;

	if (len) {
		at->ops.urc(priv, (uint8_t *)line, len);
	}
}

static void at_tunnel_check_data_prompt(void *priv)
{
	int len;
	struct at_parser *parser;
	struct at_tunnel *at;
	struct at_sets *at_set;

	at = (struct at_tunnel *)priv;
	parser = at->parser;
	at_set = mblue_at_sets_get(at->at_set_index);
	len = at_set->recv_scan(at_set, 
		parser->buf + parser->buf_current, parser->buf_used - parser->buf_current);
	if (len > 0) {
		parser->state = STATE_RAWDATA;
		parser->data_left = len;
	}

}

struct at_parser_callbacks parser_callback = {
	.scan_line = at_tunnel_line_scanner, 
	.handle_response = at_tunnel_response_handler,
	.handle_urc = at_tunnel_urc_handler,
	.check_data = at_tunnel_check_data_prompt,
};

struct at_tunnel *alloc_at_tunnel(uint16_t com, uint16_t at_set_index, void *priv,
	at_execute_t execute, at_urc_t urc)
{
	struct at_tunnel *at;
	struct at_parser *parser;

	at = mblue_malloc(sizeof(struct at_tunnel));
	if (at) {
		parser = at_parser_alloc(&parser_callback, 256, at);
		if (parser) {
			at->at_set_index = at_set_index;
			at->com = com;
			at->priv = priv;
			at->ops.execute = execute;
			at->ops.urc = urc;
			at->parser = parser;
			return at;
		}
	}
	if (at) {
		mblue_free(at);
	}
	return NULL;
}
