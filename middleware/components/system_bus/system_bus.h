/*
 * =====================================================================================
 *
 *       Filename:  system_bus.h
 *
 *    Description:  structure of element of the system bus
 *
 *        Version:  1.0
 *        Created:  2017/4/7 14:37:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  system_bus_INC
#define  system_bus_INC
#include "mblue_list.h"
#include "task_manager.h"
#include "mblue_errcode.h"

/*-----------------------------------------------------------------------------
 *  pending notify that will be called in async remote call context
 *  @payload : return value of remote method
 *  @user_data : user data that caller passes
 *-----------------------------------------------------------------------------*/
struct pending_notifier {
	void (*notify)(void *payload, void *user_data);
	void *user_data;
};

struct subscriber_info {
	struct mblue_segment *ms;
	struct list_head list;
};
struct pending_call {
	struct mblue_message *msg;
	struct mblue_task *src;
	uint32_t call_type;
	//int (*notify)(struct mblue_message *src, struct mblue_message *dst);
	struct pending_notifier *notifier;
	struct list_head list;
};

/*-----------------------------------------------------------------------------
 *  system bus is consist of a few segments, or seperated modules that 
 *  could be running in a task. Each segment node in bus here symbolize 
 *  coresponding.
 *-----------------------------------------------------------------------------*/
struct segment_node {
	struct mblue_segment *ms;
	struct list_head publish_list;
	struct list_head call_list;
};

struct system_bus {
	struct segment_node _segment_node[SEGMENT_MAJOR_MAX];
	struct mblue_semaphore init_lock;

	mblue_errcode (*init)(struct system_bus *bus);
	mblue_errcode (*contex_notify)(struct system_bus *bus);
	struct mblue_segment *(*get_segment)(struct system_bus *bus, uint16_t major);
	void (*regist)(struct system_bus *bus, struct mblue_segment *ms);
	mblue_errcode (*subscribe)(struct system_bus *bus, struct mblue_segment *ms, uint16_t major);
	mblue_errcode (*signal)(struct system_bus *bus, struct mblue_message *msg);
	mblue_errcode (*call)(struct system_bus *bus, struct mblue_message *msg, void *union_info);
	mblue_errcode (*call_return)(struct system_bus *bus, struct mblue_message *msg);
};

struct system_bus *get_system_bus_instance(void);
#endif   /* ----- #ifndef system_bus_INC  ----- */
