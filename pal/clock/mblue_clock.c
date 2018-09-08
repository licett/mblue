/*
 * =====================================================================================
 *
 *       Filename:  mblue_clock.c
 *
 *    Description:  abstractive description of clock
 *
 *        Version:  1.0
 *        Created:  2017/8/8 10:51:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
//#include "macro.h"
#include <stdio.h>
#include "mblue_assert.h"
#include "mblue_clock.h"
#include "custom_clock.h"

static int mblue_clock_init(struct mblue_clock *mc)
{
	rtc_init();
	return 0;
}

static uint64_t mblue_get_utc(struct mblue_clock *mc)
{
	uint64_t tick;

	tick = get_system_utc_tick();
	return tick;
}

static int mblue_set_utc(struct mblue_clock *mc, uint64_t seconds)
{
	return set_system_utc_tick(seconds);
}

static int mblue_set_calendar(struct mblue_clock*mc, struct mblue_clock_entity *calendar)
{
	return set_system_calendar(calendar->year,
	                           calendar->month,
	                           calendar->day,
	                           calendar->hour,
	                           calendar->minute,
	                           calendar->second);
}

struct mblue_clock_entity *mblue_get_calendar(struct mblue_clock *mc) 
{
	struct mblue_clock_entity *mce;

	mce = &mc->_mce;
	if (!get_system_calendar(&mce->year, &mce->month, &mce->day,
			&mce->hour, &mce->minute, &mce->second, &mce->time_zone)) {
		return &mc->_mce;
	}
	return NULL;
}

static int mblue_clock_uninit(struct mblue_clock *mc)
{
	return 0;
}

static struct mblue_clock *_mblue_clock = NULL;
static struct mblue_clock _mc = {
	.init = mblue_clock_init,
	.get_utc = mblue_get_utc,
	.set_utc = mblue_set_utc,
	.set_calendar = mblue_set_calendar,
	.get_calendar = mblue_get_calendar,
	.uninit = mblue_clock_uninit,
};
struct mblue_clock *mblue_clock_get_instance()
{
	if (_mblue_clock == NULL) {
		_mblue_clock = &_mc;
		_mblue_clock->init(_mblue_clock);
	}
	return _mblue_clock;
}
