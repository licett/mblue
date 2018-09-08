/*
 * =====================================================================================
 *
 *       Filename:  task.h
 *
 *    Description:  task related definition
 *
 *        Version:  1.0
 *        Created:  2017/4/18 19:23:35
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  task_INC
#define  task_INC
#include "mblue_task.h"
#include "mblue_task_entry.h"

#define	DEFAULT_INTERFACE_RESPONSE_TIMEOUT_IN_MS		10000			/*10s*/

#define	MBLUE_TASK_DEFAULT_STACK_SIZE				1024			/*  */
#define	MBLUE_TASK_MAX_MSG_IN_QUEUE				16			/*  */

uint8_t stack_user_space[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_device[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_async[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_at[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_modem[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_mqtt[8 * MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_gps[2 * MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_bus[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_log[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_led[MBLUE_TASK_DEFAULT_STACK_SIZE];

extern mblue_errcode user_space_on_create(struct mblue_task *task);
extern mblue_errcode bus_on_create(struct mblue_task *task);
extern mblue_errcode device_on_create(struct mblue_task *task);
extern mblue_errcode at_on_create(struct mblue_task *task);
extern mblue_errcode async_on_create(struct mblue_task *task);
extern mblue_errcode modem_on_create(struct mblue_task *task);
extern mblue_errcode gps_on_create(struct mblue_task *task);
extern mblue_errcode mqtt_on_create(struct mblue_task *task);
extern mblue_errcode log_on_create(struct mblue_task *task);

#define	DECLARE_PRELOAD_TASK(n, p)				\
	{							\
		.name = #n,					\
		.priority = p,					\
		.task_stack = stack_##n,			\
		.stack_size = sizeof(stack_##n),		\
		.on_create = n##_on_create,			\
	}

/* priority is from low to high */
struct mblue_task mblue_preload_tasks[] = {
	DECLARE_PRELOAD_TASK(user_space, 4),
	DECLARE_PRELOAD_TASK(led, 5),
	DECLARE_PRELOAD_TASK(gps, 6),
	DECLARE_PRELOAD_TASK(mqtt, 7),
	DECLARE_PRELOAD_TASK(log, 8),
	DECLARE_PRELOAD_TASK(modem, MBLUE_MAX_TASKS_IN_SYSTEM - 4),
	DECLARE_PRELOAD_TASK(at, MBLUE_MAX_TASKS_IN_SYSTEM - 3),
	DECLARE_PRELOAD_TASK(device, MBLUE_MAX_TASKS_IN_SYSTEM - 2),
	DECLARE_PRELOAD_TASK(bus, MBLUE_MAX_TASKS_IN_SYSTEM - 1),
};
#endif   /* ----- #ifndef task_INC  ----- */
