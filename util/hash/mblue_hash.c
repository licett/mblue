/*
 * =====================================================================================
 *
 *       Filename:  mblue_hash.c
 *
 *    Description:  simple string hash interface
 *
 *        Version:  1.0
 *        Created:  2017/10/8 10:36:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mblue_stddefine.h"
#include "mblue_hash.h"

uint32_t sdbm_hash(char *str)
{
	uint32_t hash = 0;
	while (*str)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}

	return (hash & 0x7FFFFFFF);
}

uint32_t sdbm_mem(uint8_t *str, size_t len)
{
	uint32_t hash = 0;
	while (len--)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}

	return (hash & 0x7FFFFFFF);
}

