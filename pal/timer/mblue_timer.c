/*
 * =====================================================================================
 *
 *       Filename:  mblue_timer.c
 *
 *    Description:  abstractive description of timer
 *
 *        Version:  1.0
 *        Created:  2017/5/16 17:28:25
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
#include "mblue_list.h"
#include "mblue_assert.h"
#include "custom_timer.h"
#include "custom_int.h"

static int mblue_timer_init(struct mblue_timer *timer, void (*notify)(void *), 
		uint32_t period, bool auto_reload, void *data)
{
	int rc;

	rc = platform_timer_init(timer, period, auto_reload, notify, data);
	_ASSERT(rc == 0);
	return rc;
}

static int mblue_timer_start(struct mblue_timer *timer)
{
	int rc;

	rc = platform_timer_start(timer);
	_ASSERT(rc == 0);
	return 0;
}

static int mblue_timer_stop(struct mblue_timer *timer)
{
	int rc;
	
	rc = platform_timer_stop(timer);
	_ASSERT(rc == 0);
	return 0;
}

static bool mblue_timer_alive(struct mblue_timer *timer)
{
	return platform_timer_alive(timer);
}

static int mblue_timer_uninit(struct mblue_timer *timer)
{
	int rc;
	
	rc = platform_timer_uninit(timer);
	_ASSERT(rc == 0);
	return rc;
}

static void mblue_timer_set_name(struct mblue_timer *timer, const uint8_t *name)
{
	timer->name = name;
}

static void mblue_timer_construct(struct mblue_timer *timer)
{
	timer->init = mblue_timer_init;
	timer->start = mblue_timer_start;
	timer->stop = mblue_timer_stop;
	timer->alive = mblue_timer_alive;
	timer->set_name = mblue_timer_set_name;
	timer->uninit = mblue_timer_uninit;
}

static int timer_list_init(struct timer_list *list)
{
	int i;

	INIT_LIST_HEAD(&list->timer_lists);
	for (i = 0; i < MAX_TIMER_NUM; i++) {
		list_add_tail(&(list->timer_pool[i].list), &list->timer_lists);
	}
	return 0;
}

static struct mblue_timer *timer_get(struct timer_list *list)
{
	struct mblue_timer *timer;
	struct list_head *node;

	timer = NULL;
	if (!list_empty(&list->timer_lists)) {
		node = list->timer_lists.next;

		list_del(node);
		timer = list_entry(node, struct mblue_timer, list);
	}
	return timer;
}

static int timer_put(struct timer_list *list, struct mblue_timer *timer)
{
	list_add_tail(&timer->list, &list->timer_lists);

	return 0;
}

static int timer_list_uninit(struct timer_list *list)
{
	INIT_LIST_HEAD(&list->timer_lists);
        return 0;
}

static struct timer_list *mlist;
static struct timer_list _timer_list = {
	.init = timer_list_init,	
	.get = timer_get,
	.put = timer_put,
	.uninit = timer_list_uninit,
};

struct timer_list *get_timer_list_instance()
{
	if (mlist == NULL) {
		mlist = &_timer_list;
		mlist->init(mlist);
	}
	return mlist;
}

struct mblue_timer *mblue_timer_get()
{
	struct mblue_timer *timer;
	struct timer_list *mlist;
	uint32_t key;

	mlist = get_timer_list_instance();

	key = os_enter_critical();
	timer = mlist->get(mlist);
	os_exit_critical(key);	

	if (!timer) {
		_ASSERT(0);
		return NULL;
	}
	memset(timer, 0, sizeof(struct mblue_timer));
	mblue_timer_construct(timer);
	return timer;
}

void mblue_timer_release(struct mblue_timer *timer)
{
	struct timer_list *mlist;
	uint32_t key;

	mlist = get_timer_list_instance();
	key = os_enter_critical();
	mlist->put(mlist, timer);
	os_exit_critical(key);	
}

