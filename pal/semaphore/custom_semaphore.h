/*
 * =====================================================================================
 *
 *       Filename:  custom_semaphore.h
 *
 *    Description:  custom port of semaphore 
 *
 *        Version:  1.0
 *        Created:  11/14/2017 06:40:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  custom_semaphore_INC
#define  custom_semaphore_INC

#include "mblue_assert.h"
#include "mblue_semaphore.h"

static mblue_errcode platform_semaphore_init(struct mblue_semaphore *sem)
{
	return sem_init(&sem->_obj, 0, 0) == 0 ? 
				MBLUE_OK : MBLUE_ERR_SEMAPHORE;
}

static mblue_errcode platform_semaphore_pend(struct mblue_semaphore *sem, uint32_t timeout)
{
	// we will support pend with timeout in future
	_ASSERT(timeout == SEMAPHORE_PEND_FOREVER);

	return sem_wait(&sem->_obj) == 0 ? 
				MBLUE_OK : MBLUE_ERR_SEMAPHORE;
}

static mblue_errcode platform_semaphore_post(struct mblue_semaphore *sem)
{
	return sem_post(&sem->_obj) == 0 ? 
				MBLUE_OK : MBLUE_ERR_SEMAPHORE;
}

static mblue_errcode platform_semaphore_uninit(struct mblue_semaphore *sem)
{
	return sem_destroy(&sem->_obj) == 0 ? 
				MBLUE_OK : MBLUE_ERR_SEMAPHORE;
}
#endif   /* ----- #ifndef custom_semaphore_INC  ----- */
