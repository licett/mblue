/*
 * =====================================================================================
 *
 *       Filename:  mblue_task.h
 *
 *    Description:  definition of mblue_task
 *
 *        Version:  1.0
 *        Created:  11/18/2017 02:32:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#ifndef  mblue_task_INC
#define  mblue_task_INC
#include "mblue_stddefine.h"
#include "mblue_message.h"
#include "mblue_semaphore.h"
#include "custom_task.h"

#ifndef  MBLUE_MAX_TASKS_IN_SYSTEM
#define  MBLUE_MAX_TASKS_IN_SYSTEM			16
#endif   /* ----- #ifndef MBLUE_MAX_TASKS_IN_SYSTEM  ----- */

struct mblue_task {
	int task_id;					//task id
	int priority;                                   //task priority
	char *name;                            //task name
	uint32_t stack_size;				//size of task stack
	uint8_t *task_stack;				//task stack
	task_handle task_obj;				//tcb or pcb
	task_proc tproc;				//entrance of current task
	mblue_errcode (*on_create)(struct mblue_task *task);	//task create hook
	mblue_errcode (*mproc)(struct mblue_task *task, struct mblue_message *msg);//main body of task
	mblue_errcode (*nproc)(struct mblue_task *task, struct mblue_message *msg);//notification of task
	mblue_errcode (*tpend)(struct mblue_task *task);		//alternative of sem_pend(task->call_sem)
	mblue_errcode (*tpost)(struct mblue_task *task);		//alternative of sem_post(task->call_sem)
	struct mblue_queue *mq;	 //todo list of current task
	struct mblue_semaphore msg_sem;  //pend current task when mq is empty
	struct mblue_semaphore call_sem; //pend current task when invoke remote call
	void *extra;
	struct list_head segments;	 //segments list
};

#endif   /* ----- #ifndef mblue_task_INC  ----- */
