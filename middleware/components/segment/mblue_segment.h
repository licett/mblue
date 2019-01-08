/*
 * =====================================================================================
 *
 *       Filenams:  mblue_segment.h
 *
 *    Description:  a segment, an independent logical module, usually exists as a 
 *			micro-service or business processor unit.
 *
 *        Version:  1.0
 *        Created:  2017/10/19 11:15:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  mblue_segment_INC
#define  mblue_segment_INC
#define	MBLUE_SEGMENT_MAGIC		0x736D			/* 'm', 's' */
#define	END_ITEM_ID			0xEEFFEEFF			/*  */
#define	HANDLER_ITEM(id, function)	{id, (void *)function}			

#include "mblue_stddefine.h"
#include "mblue_message.h"
struct mblue_segment;
typedef mblue_errcode (*mblue_segment_handle_t)(struct mblue_segment *ms, void *data);
typedef mblue_errcode (*mblue_none_param_func_t)(void);

//TODO
struct mblue_runtime_handler {
	void *__f;	//function that casts to a pointer
	uint16_t data_layout;
	mblue_errcode (*parse)(struct mblue_runtime_handler *, void *input_stream, void *);
};

struct handler_item {
	uint32_t id;
	void *handler;
	/*int (*handler)(void *obj, void *data);*/
};

struct mblue_segment {
	uint16_t magic;
	uint16_t major;
	void *context;
	struct mblue_message *processing_msg;
	struct handler_item *items_array[MESSAGE_TYPE_MAX];
	struct event_queue_head *queue;
	mblue_segment_handle_t handler_array[MESSAGE_TYPE_MAX];

	void *(*get_context)(struct mblue_segment *ms);
	void (*set_context)(struct mblue_segment *ms, void *context);
	mblue_segment_handle_t (*get_handler)(struct mblue_segment *ms, int type);
	void (*set_handler)(struct mblue_segment *ms, 
		int type, mblue_segment_handle_t handler);
	void (*on_launch)(struct mblue_segment *ms);
	void (*on_exit)(struct mblue_segment *ms);

	struct list_head bus;
	struct list_head task;
};
void init_segment(struct mblue_segment *ms);
void *find_ipc_handler_from_service(struct mblue_segment *ms, struct mblue_message *msg);
#endif   /* ----- #ifndef mblue_segment_INC  ----- */
