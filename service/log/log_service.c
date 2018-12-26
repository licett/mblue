#include <string.h>
#include <stdio.h>
#include "mblue_stddefine.h"
#include "mblue_task.h"
#include "mblue_message.h"
#include "mblue_heap.h"
#include "rpc_helper.h"
#include "event_helper.h"
#include "log_service.h"
#include "log.h"

struct LOG_SERVICE *ls;
static struct log_manager *log;

static void service_launch(struct mblue_segment *ms)
{
	ls = (struct LOG_SERVICE *)ms;

	log = logger_get_instance();
	log->set_type(log, LOG_STREAM);
}

static SMART_DATA _write_log(uint32_t level, uint8_t *buffer, uint32_t len)
{
	_ASSERT(ls);

	log->write(log, level, (char *)buffer, len);
	return MAKE_INTEGER(MBLUE_OK);
}

static SMART_DATA _set_log_type(uint32_t type)
{
	log->set_type(log, type);
	return MAKE_INTEGER(MBLUE_OK);
}

static SMART_DATA _set_log_level(uint32_t level)
{
	log->set_level(log, level);
	return MAKE_INTEGER(MBLUE_OK);
}

static SMART_DATA _hex_print(uint32_t tag, uint8_t *buffer, uint32_t len)
{
	_ASSERT(ls);
	_write_log(LOG_INFO, buffer, len);
	return MAKE_INTEGER(MBLUE_OK);
}

static struct handler_item rpc_items[] = {
	HANDLER_ITEM(MBLUE_LOG_SERVICE_LOG, _write_log),
	HANDLER_ITEM(MBLUE_LOG_SERVICE_SET_TYPE, _set_log_type),
	HANDLER_ITEM(MBLUE_LOG_SERVICE_SET_LEVEL, _set_log_level),
	HANDLER_ITEM(MBLUE_LOG_SERVICE_HEX, _hex_print),
	HANDLER_ITEM(END_ITEM_ID, NULL),
};

//@override
static int service_init(struct mblue_log_service *s)
{
	struct mblue_segment *ms;

	// 1.init super segment
	ms = &s->ms_entity;
	init_segment(ms);

	// 2.setup basic info and handler
	ms->major = MBLUE_LOG_SERVICE;
	ms->items_array[SYNC_CALL]	= rpc_items;
	ms->items_array[ASYNC_CALL]	= rpc_items;
	ms->on_launch			= service_launch;

	return 0;
}

DECLARE_SINGLETON_SERVICE(LOG_SERVICE)
