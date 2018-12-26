/*
 * =====================================================================================
 *
 *       Filename:  semaphore.c
 *
 *    Description:  abstractive description of semaphore
 *    NOTE:You should reimplement this file when you port mblue to a new platform
 *
 *        Version:  1.0
 *        Created:  2017/4/9 19:20:24
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "custom_semaphore.h"
#include "mblue_heap.h"
#include "mblue_assert.h"
	
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sem_init
 *  Description:  initialize a mblue semaphore
 *     @sem[IN]:  mblue semaphore object
 * =====================================================================================
 */
static mblue_errcode semaphore_init(struct mblue_semaphore *sem)
{
	_ASSERT(sem);
	_ASSERT(!sem->magic);

	sem->magic = MBLUE_SEMAPHORE_MAGIC;
	return platform_semaphore_init(sem);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sem_pend
 *  Description:  pend a task on a semaphore
 *     @sem[IN]:  mblue semaphore object
 * @timeout[IN]:  timeout
 * =====================================================================================
 */
static mblue_errcode semaphore_pend(struct mblue_semaphore *sem, uint32_t timeout)
{
	_ASSERT(sem);
	_ASSERT(sem->magic == MBLUE_SEMAPHORE_MAGIC);
	return platform_semaphore_pend(sem, timeout);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sem_post
 *  Description:  send a post signal to a semaphore
 *     @sem[IN]:  mblue semaphore object
 * =====================================================================================
 */
static mblue_errcode semaphore_post(struct mblue_semaphore *sem)
{
	_ASSERT(sem);
	_ASSERT(sem->magic == MBLUE_SEMAPHORE_MAGIC);
	return platform_semaphore_post(sem);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  sem_uninit
 *  Description:  destruct a mblue semaphore
 *     @sem[IN]:  mblue semaphore object
 * =====================================================================================
 */
static mblue_errcode semaphore_uninit(struct mblue_semaphore *sem)
{
	_ASSERT(sem);
	_ASSERT(sem->magic == MBLUE_SEMAPHORE_MAGIC);
	platform_semaphore_uninit(sem);
	sem->magic = 0;

	return MBLUE_OK;
}

struct mblue_semaphore *mblue_semaphore_alloc()
{
	struct mblue_semaphore *sem = (struct mblue_semaphore *)
		mblue_malloc(sizeof(struct mblue_semaphore));
	if (!sem) {
		goto fail_wrapper;
	}

	sem->init = semaphore_init;
	sem->pend = semaphore_pend;
	sem->post = semaphore_post;
	sem->uninit = semaphore_uninit;

	if (!sem->init(sem)) {
		return sem;
	}

fail_wrapper:
	return NULL;
}

mblue_errcode mblue_semaphore_free(struct mblue_semaphore *sem)
{
	sem->uninit(sem);
	mblue_free(sem);
	return MBLUE_OK;
}

mblue_errcode mblue_semaphore_construct(struct mblue_semaphore *sem)
{
	sem->init = semaphore_init;
	sem->pend = semaphore_pend;
	sem->post = semaphore_post;
	sem->uninit = semaphore_uninit;

	if (!sem->init(sem)) {
		return MBLUE_OK;
	}
	return MBLUE_ERR_SEMAPHORE;
}

mblue_errcode mblue_semaphore_destruct(struct mblue_semaphore *sem)
{
	return sem->uninit(sem);
}
