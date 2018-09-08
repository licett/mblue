/*
 * =====================================================================================
 *
 *       Filename:  event.h
 *
 *    Description:  definitions of events
 *
 *        Version:  1.0
 *        Created:  2017/5/23 19:40:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  event_INC
#define  event_INC
#define	_MSGID2EVENT(_id)		0x1 << (_id - 1)		/*  */
#define	msgid2event(id)			_MSGID2EVENT(id)		/*  */

static uint32_t event2msgid(uint32_t event) 
{
	int i = 1;

	while(!(event & 0x1)){++i; event >>= 1;}
	return i;
}
#endif   /* ----- #ifndef event_INC  ----- */
