/*
 * =====================================================================================
 *
 *       Filename:  ipc_helper.h
 *
 *    Description:  ipc helper
 *
 *        Version:  1.0
 *        Created:  2017/4/19 11:30:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  ipc_helper_INC
#define  ipc_helper_INC
/*#include "mblue_errcode.h"*/
#include "mblue_stddefine.h"
#include "mblue_message.h"
#include "mblue_stdtask.h"
#include "ipc.h"
#include "mblue_segment.h"

//NOTE: You should only use these macros in task context.
#define	local_signal(src, mi, dest, data)				\
	target_signal(src, 0, mi, dest, data, NULL)

#define	broadcast_signal_with_context(c, ma, mi, data)			\
	send_message(c,						\
		mblue_message_get_sequence(), SIGNAL,			\
		ma, mi, NULL, data, NULL, NULL, NULL)

#define	broadcast_signal(ma, mi, data)					\
	send_message(get_current_context(),			\
		mblue_message_get_sequence(), SIGNAL,			\
		ma, mi, NULL, data, NULL, NULL, NULL)

#define	ipc_return(major, seq, data)					\
	send_message(get_current_context(),			\
		seq, CALL_RETURN,					\
		major, 0, NULL, data, NULL, NULL, NULL)

mblue_errcode send_message(
	struct mblue_task *src, 
	uint16_t seq, uint8_t type,
	uint16_t major, uint16_t minor, 
	void *dst,
	void *in, 
	void **out, 
	void *extra, struct pending_notifier *pn);
mblue_errcode mblue_subscribe(struct mblue_segment *ms, uint16_t major);
void *mblue_remote_call(uint16_t major, uint16_t minor, void *in);
mblue_errcode mblue_remote_call_async(uint16_t major, uint16_t minor, 
	void *in,
	void (*notify)(void *, void *user_data),
	void *data);
mblue_errcode target_signal(struct mblue_task *source,
	uint16_t major, uint16_t minor, 
	void *dest,
	void *data, void *extra);
#endif   /* ----- #ifndef ipc_helper_INC  ----- */
