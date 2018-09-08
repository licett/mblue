/*
 * =====================================================================================
 *
 *       Filename:  custom_clock.h
 *
 *    Description:  custom definition of clock struct
 *
 *        Version:  1.0
 *        Created:  2017/5/16 19:09:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_timer_INC
#define  custom_timer_INC

#include "mblue_timer.h"
/*-----------------------------------------------------------------------------
 * platform related type define 
 *-----------------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "timers.h"
extern int32_t NVIC_IsInterrupt(void);
static int platform_timer_stop(struct mblue_timer *timer)
{
	if (NVIC_IsInterrupt()) {
		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;
		if (xTimerStopFromISR(timer->_obj, &xHigherPriorityTaskWoken) == pdTRUE) {
			return 0;
		}
	} else {
		if (xTimerStop(timer->_obj, 0) == pdPASS) {
			return 0;
		}
	}
	return -1;
}

static int platform_timer_start(struct mblue_timer *timer)
{
	if (NVIC_IsInterrupt()) {
		BaseType_t xHigherPriorityTaskWoken;
		xHigherPriorityTaskWoken = pdFALSE;
		if (xTimerStartFromISR(timer->_obj, &xHigherPriorityTaskWoken) == pdTRUE) {
			return 0;
		}
	} else {
		if (xTimerStart(timer->_obj, 0) == pdPASS) {
			return 0;
		}
	}
	return -1;
}

static int platform_timer_alive(struct mblue_timer *timer)
{
	return (xTimerIsTimerActive(timer->_obj) != pdFALSE);
}

static int platform_timer_init(struct mblue_timer *timer, uint32_t period, 
	bool auto_reload, void (*notify)(void *), void *data)
{
	timer->_obj = xTimerCreate(NULL, 
		pdMS_TO_TICKS(period), 
		auto_reload ? pdTRUE : pdFALSE, 
		data,
		notify);
	return timer->_obj == NULL ? -1 : 0;
}

static int platform_timer_uninit(struct mblue_timer *timer)
{
	if (xTimerDelete(timer->_obj, 0) == pdPASS) {
		timer->_obj = NULL;
		return 0;
	}
	return -1;
}
#endif   /* ----- #ifndef custom_timer_INC  ----- */
