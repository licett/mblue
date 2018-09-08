/*
 * =====================================================================================
 *
 *       Filename:  custom_int_mt.h
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

#ifndef  custom_int_mt_INC
#define  custom_int_mt_INC

static uint32_t os_enter_critical()
{
	return disable_irq();
}

static void os_exit_critical(uint32_t key)
{
	restore_irq(key);
}
#endif   /* ----- #ifndef custom_int_ti_INC  ----- */
