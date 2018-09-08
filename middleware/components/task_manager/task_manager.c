/*
 * =====================================================================================
 *
 *       Filename:  task_manager.c
 *
 *    Description:  task manager
 *
 *        Version:  1.0
 *        Created:  2017/4/15 14:26:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "mblue_macro.h"
#include "task_manager.h"
#include "mblue_assert.h"

static uint8_t task_id = 0;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  task_manager_init
 *  Description:  alloc ram for task array
 * =====================================================================================
 */
static int task_manager_init(struct task_manager *tm, int task_number)
{
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  task_create
 *  Description:  create a new task
 * =====================================================================================
 */
static struct mblue_task *task_create(struct task_manager *tm, struct mblue_task *task)
{
	task->task_id = task_id;
	tm->task[task_id++] = task;

	_task_construct(task);
	return task;
}

static struct task_manager tm = {
	.init = task_manager_init,
	.create_task = task_create,
};

static struct task_manager *tm_instance = NULL;
struct task_manager *get_task_manager_instance()
{
	if (!tm_instance) {
		tm_instance = &tm;
		tm_instance->init(tm_instance, MBLUE_MAX_TASKS_IN_SYSTEM);
	}
	return tm_instance;
}
