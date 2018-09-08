/*
 * =====================================================================================
 *
 *       Filename:  custom_task_ti.h
 *
 *    Description:  custom definition of task operation in TI RTOS
 *
 *        Version:  1.0
 *        Created:  2017/4/15 11:50:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_task_ti_INC
#define  custom_task_ti_INC


#include "FreeRTOS.h"
#include "task.h"
#define	task_handle			StaticTask_t 			/*  */
#define	task_proc			TaskFunction_t			/*  */
#define	task_sleep(ms)					\
		vTaskDelay(pdMS_TO_TICKS(ms))
#define	GET_CURRENT_CONTEXT()				\
		(struct mblue_task *)list_entry(	\
		xTaskGetCurrentTaskHandle(),		\
		struct mblue_task,			\
		task_obj)
#define	GET_CURRENT_TASK_ID()				\
		GET_CURRENT_CONTEXT()->task_id
#define	GET_CURRENT_TASK_PRIORITY()			\
		GET_CURRENT_CONTEXT()->priority

#define	_task_construct(t)							\
        do {                                                                    \
		TaskHandle_t th;						\
		th = xTaskCreateStatic(t->tproc, t->name, t->stack_size,	\
			t, t->priority, (StackType_t * const)t->task_stack,	\
			&t->task_obj);						\
		_ASSERT(th);							\
	}while(0)
#endif   /* ----- #ifndef custom_task_ti_INC  ----- */
