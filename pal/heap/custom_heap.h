/*
 * =====================================================================================
 *
 *       Filename:  custom_heap.h
 *
 *    Description:  custom implementation of heap
 *
 *        Version:  1.0
 *        Created:  2017/4/10 21:04:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CUSTOM_HEAP_INC
#define CUSTOM_HEAP_INC

#define	imalloc(heap, size)	malloc(size)
#define	ifree(heap, p)	        free(p)

#endif
