/*
 * =====================================================================================
 *
 *       Filename:  custom_task.h
 *
 *    Description:  task typedef route
 *
 *        Version:  1.0
 *        Created:  2017/4/15 15:51:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#if defined(TI_RTOS_TASK)
#include "custom_task_ti.h"
#elif defined(MT_NUCLEUS_TASK)
#include "custom_task_mt.h"
#elif defined(FREERTOS_TASK)
#include "custom_task_freertos.h"
#endif
