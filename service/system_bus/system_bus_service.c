/*
 * =====================================================================================
 *
 *       Filename:  system_bus_service.c
 *
 *    Description:  system bus service is the core module of mblue, 
 *			acting as a arbitrator to dispatch messages to different tasks
 *
 *        Version:  1.0
 *        Created:  2017/10/23 20:31:00
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mblue_stddefine.h"
#include "mblue_message.h"
#include "mblue_segment.h"
#include "ipc.h"
#include "system_bus.h"
#include "system_bus_service.h"

#define	SYSBUS_SERVICE_PARSE_INPUT_PARAMETER(ms, data)	\
	uint16_t major;					\
	struct system_bus *sb;				\
	struct mblue_message *msg;			\
	struct system_bus_service *service;		\
        msg = (struct mblue_message *)data;		\
	major = GET_MAJOR(msg);				\
	service = (struct system_bus_service *)ms;	\
	sb = service->sb;				

static mblue_errcode handle_subscribe(struct mblue_segment *ms, void *data)
{
	SYSBUS_SERVICE_PARSE_INPUT_PARAMETER(ms, data);
	struct mblue_segment *coming_ms;

	coming_ms = (struct mblue_segment *)msg->extra;
	mblue_message_release(msg);
	/*if (!sb->get_segment(sb, major)) {
		sb->regist(sb, ms);
	}*/
	return sb->subscribe(sb, coming_ms, major);
}

static mblue_errcode handle_signal(struct mblue_segment *ms, void *data)
{
	SYSBUS_SERVICE_PARSE_INPUT_PARAMETER(ms, data);
	(void)major;
	return sb->signal(sb, msg);
}

static mblue_errcode handle_call(struct mblue_segment *ms, void *data)
{
	SYSBUS_SERVICE_PARSE_INPUT_PARAMETER(ms, data);
	(void)major;
	return sb->call(sb, msg, msg->extra);
}

static mblue_errcode handle_call_return(struct mblue_segment *ms, void *data)
{
	SYSBUS_SERVICE_PARSE_INPUT_PARAMETER(ms, data);
	(void)major;
	return sb->call_return(sb, msg);
}

//@override
static void service_launch(struct mblue_segment *ms)
{
	struct mblue_task *ts;
	struct mblue_ipc *ipc;

	ts = ms->get_context(ms);
	ipc = ipc_facade_create_instance(ts);
	_ASSERT(ipc);
}

static void system_bus_service_init(struct system_bus_service *service)
{
	struct mblue_segment *ms;

	ms = &service->ms_entity;
	init_segment(ms);
	service->sb = get_system_bus_instance();

	ms->major = MBLUE_BUS;
	ms->set_handler(ms, SUBSCRIBE,		handle_subscribe);
	ms->set_handler(ms, SIGNAL,		handle_signal);
	ms->set_handler(ms, SYNC_CALL,		handle_call);
	ms->set_handler(ms, ASYNC_CALL,		handle_call);
	ms->set_handler(ms, CALL_RETURN,	handle_call_return);
	ms->on_launch = service_launch;
}

/*-----------------------------------------------------------------------------
 *  static service instance : singleton
 *-----------------------------------------------------------------------------*/
static struct system_bus_service _system_bus_service_instance;
static struct system_bus_service *system_bus_service_instance = NULL;
struct system_bus_service *get_system_bus_service_instance(void)
{
	if (system_bus_service_instance == NULL) {
		system_bus_service_instance = 
			&_system_bus_service_instance;
		system_bus_service_init(
			system_bus_service_instance);
	}
	return system_bus_service_instance; 
}
