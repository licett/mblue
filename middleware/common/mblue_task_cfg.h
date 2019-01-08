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
uint8_t stack_bus[MBLUE_TASK_DEFAULT_STACK_SIZE];
uint8_t stack_log[MBLUE_TASK_DEFAULT_STACK_SIZE];

extern mblue_errcode user_space_on_create(struct mblue_task *task);
extern mblue_errcode bus_on_create(struct mblue_task *task);

#define	DECLARE_PRELOAD_TASK(n, p)				\
	{							\
		.name = #n,					\
		.priority = p,					\
		.task_stack = stack_##n,			\
		.stack_size = sizeof(stack_##n),		\
		.on_create = n##_on_create,			\
	}

/* priority is from high to two */
struct mblue_task mblue_preload_tasks[] = {
	DECLARE_PRELOAD_TASK(bus, MBLUE_MAX_TASKS_IN_SYSTEM - 1),
	DECLARE_PRELOAD_TASK(user_space, 4),
	DECLARE_PRELOAD_TASK(log, 3),
};
#endif   /* ----- #ifndef task_INC  ----- */
