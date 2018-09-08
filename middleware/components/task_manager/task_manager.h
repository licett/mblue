/*
 * =====================================================================================
 *
 *       Filename:  task_manager.h
 *
 *    Description:  task manager
 *
 *        Version:  1.0
 *        Created:  2017/4/15 14:32:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  task_manager_INC
#define  task_manager_INC
#include "mblue_task.h"

/*typedef int (*msg_proc)(struct mblue_task *task, struct mblue_message *msg);
typedef int (*msg_notify)(struct mblue_task *task, struct mblue_message *msg, void *info);
typedef int (*task_control)(struct mblue_task *task);*/

struct task_manager {
	struct mblue_task *task[MBLUE_MAX_TASKS_IN_SYSTEM];

	int (*init)(struct task_manager *tm, int task_number);
	struct mblue_task *(*create_task)(struct task_manager *tm, struct mblue_task *task);
	int (*delete_task)(struct task_manager *tm, struct mblue_task *task);
	struct mblue_task *(*get_task)(struct task_manager *tm, int task_id);
	int (*uninit)(struct task_manager *tm);
};

struct task_manager *get_task_manager_instance(void);
#endif   /* ----- #ifndef task_manager_INC  ----- */
