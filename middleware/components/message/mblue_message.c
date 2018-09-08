/*
 * =====================================================================================
 *
 *       Filename:  mblue_message.c
 *
 *    Description:  mblue_message manager which consists of basic operation of mblue_messages
 *
 *        Version:  1.0
 *        Created:  2017/5/10 14:11:03
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
#include "mblue_message.h"

static uint16_t sequence = 0;

uint32_t mblue_message_get_sequence()
{
	uint32_t key = os_enter_critical();
	if (++sequence & 0x8000) {
		sequence = 0;
	}
	os_exit_critical(key);	
	return sequence;
}

uint16_t get_mblue_message(struct mblue_message *msg)
{
	msg->ref++;
	return msg->ref;
}

uint16_t put_mblue_message(struct mblue_message *msg)
{
	if(msg->ref) {
		msg->ref--;
	}
	return msg->ref;
}

static int mblue_message_init(struct mblue_message_list *list)
{
	int i;

	list->count = MBLUE_MAX_MESSAGE_NUM;
	INIT_LIST_HEAD(&list->msg_list);
	for (i = 0; i < MBLUE_MAX_MESSAGE_NUM; i++) {
		list_add_tail(&(list->mblue_message_pool[i].list), &list->msg_list);
	}
	return 0;
}

static struct mblue_message *mblue_message_get(struct mblue_message_list *list)
{
	struct mblue_message *msg;
	struct list_head *node;

	msg = NULL;
	if (list->count) {
		node = list->msg_list.next;

		list->count--;
		list_del(node);
		msg = list_entry(node, struct mblue_message, list);
	}
	return msg;
}

static int mblue_message_put(struct mblue_message_list *list, struct mblue_message *msg)
{
	list_add_tail(&msg->list, &list->msg_list);
	list->count++;

	return 0;
}

static int mblue_message_uninit(struct mblue_message_list *list)
{
	list->count = 0;
	INIT_LIST_HEAD(&list->msg_list);
        return 0;
}

static struct mblue_message_list *mlist;
static struct mblue_message_list _mblue_message_list = {
	.init = mblue_message_init,	
	.get = mblue_message_get,
	.put = mblue_message_put,
	.uninit = mblue_message_uninit,
};

struct mblue_message_list *get_mblue_message_list_instance()
{
	if (mlist == NULL) {
		mlist = &_mblue_message_list;
		mlist->init(mlist);
	}
	return mlist;
}

struct mblue_message *mblue_message_alloc()
{
	struct mblue_message *msg;
	struct mblue_message_list *mlist;
	uint32_t key;

	key = os_enter_critical();
	mlist = get_mblue_message_list_instance();
	msg = mlist->get(mlist);
	os_exit_critical(key);	

	if (!msg) {
		_ASSERT(0);
		return NULL;
	}
	memset(msg, 0, sizeof(struct mblue_message));
#ifdef DEBUG
	msg->magic = MBLUE_MESSAGE_MAGIC;
#endif
	return msg;
}

void mblue_message_release(struct mblue_message *msg)
{
	uint32_t key;
	struct mblue_message_list *mlist;

	key = os_enter_critical();
	mlist = get_mblue_message_list_instance();
	if (!put_mblue_message(msg)) {
		mlist->put(mlist, msg);
	}
	os_exit_critical(key);	
}
