/*
 * =====================================================================================
 *
 *       Filename:  custom_int.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/4/18 15:10:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_int_INC
#define  custom_int_INC
#include "mblue_stddefine.h"
/*
#if defined(TI_RTOS_TASK)
#include "custom_int_ti.h"
#elif defined(MT_NUCLEUS_TASK)
#include "custom_int_mt.h"
#endif
*/
uint32_t os_enter_critical();
void os_exit_critical(uint32_t key);

#endif   /* ----- #ifndef custom_int_INC  ----- */
