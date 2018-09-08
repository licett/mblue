/*
 * =====================================================================================
 *
 *       Filename:  custom_task_mt.h
 *
 *    Description:  custom definition of task operation in mtk nucleus RTOS
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

#ifndef  custom_task_mt_INC
#define  custom_task_mt_INC

#include "ql_system.h"
#define	task_param			int
#define	task_sleep			Ql_Sleep			/*  */
#define	get_current_task_priority	Ql_OS_GetActiveTaskId			/*  */
#define	GET_CURRENT_CONTEXT() 			\
	get_task_manager_instance()->task[get_current_task_priority()]

#define	_task_construct(t, param)						
#endif   /* ----- #ifndef custom_task_mt_INC  ----- */
