/*
 * =====================================================================================
 *
 *       Filename:  custom_int_ti.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/4/18 15:11:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_int_ti_INC
#define  custom_int_ti_INC
#include <ti/sysbios/hal/Hwi.h>
static uint32_t os_enter_critical()
{
        return Hwi_disable();
}

static void os_exit_critical(uint32_t key)
{
	Hwi_restore(key);
}
#endif   /* ----- #ifndef custom_int_ti_INC  ----- */
