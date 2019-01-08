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
	target_signal(src, get_current_context(), 0, mi, dest, data, NULL)

#define	broadcast_signal_with_context(c, ma, mi, data)			\
	mblue_message_post(c,						\
		mblue_message_get_sequence(), SIGNAL,			\
		ma, mi, data, NULL, NULL, NULL)

#define	broadcast_signal(ma, mi, data)					\
	mblue_message_post(get_current_context(),			\
		mblue_message_get_sequence(), SIGNAL,			\
		ma, mi, data, NULL, NULL, NULL)

#define	ipc_return(major, seq, data)					\
	mblue_message_post(get_current_context(),			\
		seq, CALL_RETURN,					\
		major, 0, data, NULL, NULL, NULL)

mblue_errcode mblue_subscribe(struct mblue_segment *ms, uint16_t major);
mblue_errcode mblue_message_post(
	struct mblue_task *task, uint16_t seq, uint8_t type,
	uint16_t major, uint16_t minor, 
	void *in, void **out, 
	void *extra, struct pending_notifier *pn);
void *mblue_remote_call(uint16_t major, uint16_t minor, void *in);
mblue_errcode mblue_remote_call_async(uint16_t major, uint16_t minor, 
	void *in,
	void (*notify)(void *, void *user_data),
	void *data);
mblue_errcode target_signal(struct mblue_task *source, struct mblue_task *target,
	uint16_t major, uint16_t minor, 
	void *dest,
	void *data, void *extra);
#endif   /* ----- #ifndef ipc_helper_INC  ----- */
