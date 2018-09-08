/*
 * =====================================================================================
 *
 *       Filename:  custom_system.h
 *
 *    Description:  platform related system operations
 *
 *        Version:  1.0
 *        Created:  01/10/2018 02:58:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  CUSTOM_SYSTEM_INC
#define  CUSTOM_SYSTEM_INC
#include "mblue_stddefine.h"
#include "system_stm32f4xx.h"

void platform_system_reset()
{
	SystemReset();
}

uint32_t platform_system_tick()
{
        return 0;
}
#endif   /* ----- #ifndef custom_system_INC  ----- */
