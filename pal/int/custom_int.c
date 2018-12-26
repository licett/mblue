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

uint32_t os_enter_critical()
{
	return 0;
}

void os_exit_critical(uint32_t key)
{
	(void) key;
}
