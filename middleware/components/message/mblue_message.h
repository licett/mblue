/*
 * =====================================================================================
 *
 *       Filename:  mblue_message.h
 *
 *    Description:  structure of mblue_message
 *
 *        Version:  1.0
 *        Created:  2017/4/7 14:13:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  mblue_message_INC
#define  mblue_message_INC
#include "mblue_list.h"
#include "mblue_assert.h"
#include "custom_int.h"
#include "message_def.h"

#define	MBLUE_MESSAGE_MAGIC		0x12345678			/*  */
#define	MBLUE_MAX_MESSAGE_NUM		32				/*  */

#define	TYPE_OFFSET			28				/* 4bits	[28:31] */
#define	MAJOR_OFFSET			16				/* 12bits	[16:27]*/

#define	GET_TYPE(msg)			((msg->code) >> 28)		/*  */
#define	GET_MAJOR(msg)			(((msg->code) >> 16) & 0xFFF)		/*  */
#define	GET_MINOR(msg)			((msg->code) & 0xFFFF)	/*  */

struct mblue_message {
#ifdef DEBUG
	uint32_t magic;
#endif
	uint16_t seq;			//sequence id of mblue_message
	uint16_t ref;			//how many modules are using this mblue_message?
	uint32_t code;			//code = type << 28 | major << 16 | minor 
	void *src;			//source of the mblue_message
	void *dst;			//destination of the mblue_message

	/*struct mblue_payload *in;*/
	void *smart_payload;

	union {
		void *extra;			//extra infomation for other bus method(signal/subscribe)
		void **rpc_return;		//return payload for sync remote call
		struct pending_notifier *pn;    //notifier for aysnc remote call
	};
	struct list_head list;
};

struct mblue_message_list {
	int count;
	struct list_head msg_list;
	struct mblue_message mblue_message_pool[MBLUE_MAX_MESSAGE_NUM];

	int (*init)(struct mblue_message_list *list);
	struct mblue_message *(*get)(struct mblue_message_list *list);
	int (*put)(struct mblue_message_list *list, struct mblue_message *msg);
	int (*uninit)(struct mblue_message_list *list);
};

struct mblue_message_list *get_mblue_message_list_instance();
struct mblue_message *mblue_message_alloc();
void mblue_message_release(struct mblue_message *msg);
uint32_t mblue_message_get_sequence();
uint16_t get_mblue_message(struct mblue_message *msg);
uint16_t put_mblue_message(struct mblue_message *msg);
#endif   /* ----- #ifndef mblue_message_INC  ----- */
