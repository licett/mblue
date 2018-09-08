/*
 * =====================================================================================
 *
 *       Filename:  semaphore.h
 *
 *    Description:  abstractive description of semaphore
 *
 *        Version:  1.0
 *        Created:  2017/4/9 19:22:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  semaphore_INC
#define  semaphore_INC

#include "mblue_stddefine.h"
#include "custom_semaphore_type.h"

/*-----------------------------------------------------------------------------
 * platform related type define 
 *-----------------------------------------------------------------------------*/
#define	MBLUE_SEMAPHORE_MAGIC		0xFEEEAAA0			/*  */
#define	SEMAPHORE_PEND_FOREVER		0xFFFFFFFF			/*  */

struct mblue_semaphore {
	uint32_t magic;
	platform_semaphore_obj _obj;

	mblue_errcode (*init)(struct mblue_semaphore *sem);
	mblue_errcode (*pend)(struct mblue_semaphore *sem, uint32_t timeout);
	mblue_errcode (*post)(struct mblue_semaphore *sem);
	mblue_errcode (*uninit)(struct mblue_semaphore *sem);
};

struct mblue_semaphore *mblue_semaphore_alloc();
mblue_errcode mblue_semaphore_free(struct mblue_semaphore *sem);
mblue_errcode mblue_semaphore_construct(struct mblue_semaphore *sem);
#endif   /* ----- #ifndef semaphore_INC  ----- */
