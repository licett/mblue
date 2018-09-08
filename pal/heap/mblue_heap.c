/*
 * =====================================================================================
 *
 *       Filename:  heap.c
 *
 *    Description:  abstractive description of heap
 *    NOTE:You should reimplement this file when you port mblue to a new platform
 *
 *        Version:  1.0
 *        Created:  2017/4/10 17:56:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "mblue_stddefine.h"
#include "mblue_macro.h"
#include "mblue_assert.h"
#include "mblue_heap.h"
#include "mblue_logger.h"

#define	SIMPLE_HEAP_COUNT_CHECK		0	/* simple count to detect memory leak */
#if (SIMPLE_HEAP_COUNT_CHECK == 1)
static uint32_t malloc_count = 0;
#endif

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  heap_init
 *  Description:  init wrapper of heap
 * =====================================================================================
 */
static int heap_init(struct mblue_heap *heap)
{
	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  heap_malloc
 *  Description:  malloc wrapper of heap
 * =====================================================================================
 */
static void *heap_malloc(struct mblue_heap *heap, uint32_t size)
{
	return imalloc(heap, size);
}

/* 
 * ===  function  ======================================================================
 *         name:  heap_malloc
 *  description:  free wrapper of heap
 * =====================================================================================
 */
static void heap_free(struct mblue_heap *heap, void *p)
{
	_ASSERT(p);
	ifree(heap, p);
}

/* 
 * ===  function  ======================================================================
 *         name:  heap_malloc
 *  description:  uninit wrapper of heap
 * =====================================================================================
 */
static int heap_uninit(struct mblue_heap *heap)
{
	return 0;
}

static struct mblue_heap mheap = {
	.init = heap_init,
	._malloc = heap_malloc,
	._free = heap_free,
	.uninit = heap_uninit,
};

void *mblue_malloc(uint32_t size)
{
	void *p = mheap._malloc(&mheap, MBLUE_ALIGN(size));
#if (SIMPLE_HEAP_COUNT_CHECK == 1)
	malloc_count++;
	LOGGER(LOG_WARN, " malloc %d, 0x%x, %u\r\n", malloc_count, p, size);
#endif
	if (p) {
		memset(p, 0, size);
		return p;
	}
	_ASSERT(0);
	return NULL;
}

void mblue_free(void *p)
{
#if (SIMPLE_HEAP_COUNT_CHECK == 1)
	malloc_count--;
	LOGGER(LOG_WARN, " free %d, 0x%x\r\n", malloc_count, p);
#endif
	_ASSERT(p);
	mheap._free(&mheap, p);
}
