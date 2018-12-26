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

	select(0, NULL, NULL, NULL, &tm);
}

static BOOL custom_current_task_equal(task_handle task)
{
	return pthread_equal(pthread_self(), task) == 0 ? FALSE : TRUE;
}

#define	_task_construct(t)									\
        do {											\
		int rc;										\
		pthread_attr_t attr;								\
		struct sched_param param;							\
		rc = pthread_attr_init(&attr);							\
		_ASSERT(!rc);									\
		rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);		\
		_ASSERT(!rc);									\
		rc = pthread_attr_setstacksize(&attr, t->task_stack);				\
		_ASSERT(!rc);									\
		param.sched_priority = sched_get_priority_min(SCHED_FIFO) + t->priority;	\
		rc = pthread_attr_setschedparam(&attr,&param);					\
		_ASSERT(!rc);									\
		rc = pthread_create(&t->task_obj, &attr, t->tproc, t);				\
		_ASSERT(!rc);									\
	}while(0)
#endif   /* ----- #ifndef CUSTOM_TASK_PTHREAD_INC  ----- */
