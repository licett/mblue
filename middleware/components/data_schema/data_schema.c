/*
 * =====================================================================================
 *
 *       Filename:  data_schema.c
 *
 *    Description:  data schema
 *
 *
 *        Version:  1.0
 *        Created:  2017/10/12 23:05:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_stddefine.h"
#include "data_schema.h"
#include "mblue_heap.h"
#include "mblue_assert.h"
#include "pb_encode.h"
#include "pb_schema.h"
#include "mblue_logger.h"


/*struct mblue_packet *alloc_mblue_packet(int code, 
	uint32_t num1, uint32_t num2, uint8_t *data, void *extra)
{
	struct mblue_packet *mp;			

	mp = (struct mblue_packet *)mblue_malloc(	
			sizeof(struct mblue_packet));		
	if (mp) {
		mp->code = code;						
		mp->num1 = num1;						
		mp->num2 = num2;						
		mp->data = data;						
		mp->extra = extra;						
	}
	return mp;
}

void release_mblue_packet(struct mblue_packet *mp)
{
	if (mp) {
		mblue_free(mp);
	}
}*/

