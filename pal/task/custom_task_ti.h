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

#include <ti/sysbios/knl/Task.h>
#define	task_handle		Task_Struct 			/*  */
#define	task_param		Task_Params
#define	task_proc		Task_FuncPtr			/*  */
#define	task_sleep			Task_sleep			/*  */
#define	get_current_task_priority()	Task_getPri(Task_self())			/*  */
#define	GET_CURRENT_CONTEXT() 			\
	get_task_manager_instance()->task[get_current_task_priority()]

#define	_task_construct(t, param)						\
        do {                                                                    \
		Task_Params_init(&param);					\
		param.stack = t->task_stack;				        \
		param.stackSize = t->stack_size;			        \
		param.priority = t->priority;					\
		param.arg0 = (UArg)t;						\
		param.instance->name = (char const *)t->name;			\
		Task_construct(&t->task_obj, t->tproc, &param, NULL);		\
	}while(0)
#endif   /* ----- #ifndef custom_task_ti_INC  ----- */
