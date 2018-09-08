/*
 * =====================================================================================
 *
 *       Filename:  mblue_system.c
 *
 *    Description:  this file exports system operation such as reset, get_tick
 *
 *        Version:  1.0
 *        Created:  01/10/2018 02:30:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_system.h"
#include "custom_system.h"

void mblue_system_reset(void)
{
	platform_system_reset();
}
