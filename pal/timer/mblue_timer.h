/*
 * =====================================================================================
 *
 *       Filename:  mblue_timer.h
 *
 *    Description:  abstractive description of timer
 *
 *        Version:  1.0
 *        Created:  2017/5/16 17:28:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  mblue_timer_INC
#define  mblue_timer_INC

#define	MAX_TIMER_NUM				12			/*  */
struct mblue_timer {
	const uint8_t *name;
	//platform_timer_obj _obj;
	//platform_timer_param _param;
	void *_obj;
	void *_param;
	struct list_head list;
	
	int (*init)(struct mblue_timer *timer, void (*notify)(void *), 
		uint32_t period, bool auto_reload, void *data);
	int (*start)(struct mblue_timer *timer);
	int (*stop)(struct mblue_timer *timer);
	bool (*alive)(struct mblue_timer *timer);
	void (*set_name)(struct mblue_timer *timer, const uint8_t *name);
	int (*uninit)(struct mblue_timer *timer);
};

struct timer_list {
	int count;
	struct list_head timer_lists;
	struct mblue_timer timer_pool[MAX_TIMER_NUM];

	int (*init)(struct timer_list *list);
	struct mblue_timer *(*get)(struct timer_list *list);
	int (*put)(struct timer_list *list, struct mblue_timer *timer);
	int (*uninit)(struct timer_list *list);
};

struct mblue_timer *mblue_timer_get();
void mblue_timer_release(struct mblue_timer *timer);
#endif   /* ----- #ifndef mblue_timer_INC  ----- */
