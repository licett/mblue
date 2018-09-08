/*
 * =====================================================================================
 *
 *       Filename:  custom_task_pthread.h
 *
 *    Description:  customized task related operation
 *
 *        Version:  1.0
 *        Created:  09/07/18 15:55:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  CUSTOM_TASK_PTHREAD_INC
#define  CUSTOM_TASK_PTHREAD_INC

#include "mblue_stddefine.h"
#include <pthread.h>
#include <sys/select.h>

#define	task_handle			pthread_t 			/*  */
typedef void *(* task_proc)(void *);

#define	task_sleep(ms)					\
		mblue_pthread_sleep(ms)
#define	GET_CURRENT_CONTEXT()				\
		(struct mblue_task *)list_entry(	\
		xTaskGetCurrentTaskHandle(),		\
		struct mblue_task,			\
		task_obj)
#define	GET_CURRENT_TASK_ID()				\
		GET_CURRENT_CONTEXT()->task_id
#define	GET_CURRENT_TASK_PRIORITY()			\
		GET_CURRENT_CONTEXT()->priority

void mblue_pthread_sleep(uint32_t delay_ms)
{
	struct timeval tm;

	tm.tv_sec = delay_ms / 1000;
	tm.tv_usec = (delay_ms - tm.tv_sec * 1000) * 1000;

	select(0,NULL,NULL,NULL,&tm);
}

#define	_task_construct(t)							\
        do {                                                                    \
		TaskHandle_t th;						\
		th = xTaskCreateStatic(t->tproc, t->name, t->stack_size,	\
			t, t->priority, (StackType_t * const)t->task_stack,	\
			&t->task_obj);						\
		_ASSERT(th);							\
	}while(0)
#endif   /* ----- #ifndef CUSTOM_TASK_PTHREAD_INC  ----- */
