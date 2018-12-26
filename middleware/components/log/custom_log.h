/*
 * =====================================================================================
 *
 *       Filename:  custom_log.h
 *
 *    Description:  implementation of real print stream 
 *
 *        Version:  1.0
 *        Created:  09/11/2018 10:58:47 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  CUSTOM_LOG_INC
#define  CUSTOM_LOG_INC

#include "log.h"

#include <stdio.h>
static int __std_print(char *buffer)
{
	printf("%s", buffer);
	return strlen(buffer);
}

static int custom_print(char *buffer)
{
	return __std_print(buffer);
}
#endif   /* ----- #ifndef custom_log_INC  ----- */
