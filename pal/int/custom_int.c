/*
 * =====================================================================================
 *
 *       Filename:  custom_int.c
 *
 *    Description:  implementation of entering and exiting critical section
 *
 *        Version:  1.0
 *        Created:  11/14/2017 03:36:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "custom_int.h"

#include "FreeRTOS.h"
#include "task.h"
extern int32_t NVIC_IsInterrupt(void);
	 
uint32_t os_enter_critical()
{
	uint32_t s;
	if (NVIC_IsInterrupt()) {
		s = taskENTER_CRITICAL_FROM_ISR();
	} else {
		taskENTER_CRITICAL();
	}
	return s;
}

void os_exit_critical(uint32_t key)
{
	if (NVIC_IsInterrupt()) {
		taskEXIT_CRITICAL_FROM_ISR(key);
	} else {
		taskEXIT_CRITICAL();
	}
}
