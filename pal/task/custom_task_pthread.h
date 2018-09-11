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
#include "mblue_errcode.h"
#include <pthread.h>
#include <sys/select.h>

#define	task_handle			pthread_t 			/*  */
typedef void *(* task_proc)(void *);

static void custom_task_sleep(uint32_t delay_ms)
{
	struct timeval tm;

	tm.tv_sec = delay_ms / 1000;
	tm.tv_usec = (delay_ms - tm.tv_sec * 1000) * 1000;

	select(0,NULL,NULL,NULL,&tm);
}

static BOOL custom_current_task_equal(task_handle task)
{
	return pthread_equal(pthread_self(), task) == 0 ? FALSE : TRUE;
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
