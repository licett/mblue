/*
 * =====================================================================================
 *
 *       Filename:  heap.h
 *
 *    Description:  abstractive description of heap
 *    NOTE:You should reimplement this file when you port mblue to a new platform
 *
 *        Version:  1.0
 *        Created:  2017/4/10 17:58:10
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  heap_INC
#define  heap_INC

#include "custom_heap.h"

struct mblue_heap {
	void *data;

	int (* init)(struct mblue_heap *heap);
	void* (*_malloc)(struct mblue_heap *heap, uint32_t size);
	void (* _free)(struct mblue_heap *heap, void *p);
	int (* uninit)(struct mblue_heap *heap);
};

void *mblue_malloc(uint32_t size);
void mblue_free(void *p);

#endif   /* ----- #ifndef heap_INC  ----- */
