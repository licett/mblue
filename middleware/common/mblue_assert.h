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
#define	_ASSERT(x)									\
				do {							\
					if (!(x))	{		                \
						__asm volatile("BKPT #01");		\
                                        }						\
				}while(0)		/*  */


#else
#include "mblue_system.h"
#define	_ASSERT(x)									\
				do {							\
					if (!(x))	{		                \
						mblue_system_reset();		\
                                        }						\
				}while(0)		/*  */
#endif
#endif   /* ----- #ifndef _assert_INC  ----- */
