/*
 * =====================================================================================
 *
 *       Filename:  logger.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/4/20 11:11:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  logger_INC
#define  logger_INC
#include "log.h"
#include "mblue_global_rpc_method.h"
#define LOGGER(level, fmt, ...)								\
				do {							\
					log_service_write(level, __FILE__, fmt, ##__VA_ARGS__);	\
				}while(0)	

#endif   /* ----- #ifndef logger_INC  ----- */
