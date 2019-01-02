/*
 * =====================================================================================
 *
 *       Filename:  _assert.h
 *
 *    Description:  project assert
 *
 *        Version:  1.0
 *        Created:  2017/4/20 11:13:54
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  _assert_INC
#define  _assert_INC
#ifdef DEBUG
#include "mblue_logger.h"
#define	_ASSERT(x)									\
				do {							\
					if (!(x))	{		                \
						__asm volatile("BKPT #01");		\
						LOGGER(LOG_DEBUG, "assert failed\n");	\
                                        }						\
				}while(0)		/*  */


#else
#define	_ASSERT(x)									
#endif
#endif   /* ----- #ifndef _assert_INC  ----- */
