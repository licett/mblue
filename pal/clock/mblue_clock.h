/*
 * =====================================================================================
 *
 *       Filename:  mblue_clock.h
 *
 *    Description:  abstractive description of clock
 *
 *        Version:  1.0
 *        Created:  2017/8/7 16:42:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  mblue_clock_INC
#define  mblue_clock_INC
#include <stdint.h>

struct mblue_clock_entity {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t time_zone;
};

struct mblue_clock {
	struct mblue_clock_entity _mce;

	int (*init)(struct mblue_clock *mc);
	uint64_t (*get_utc)(struct mblue_clock *mc);
	int (*set_utc)(struct mblue_clock *mc, uint64_t seconds);
	int (*set_calendar)(struct mblue_clock*mc, struct mblue_clock_entity *calendar);
	struct mblue_clock_entity *(*get_calendar)(struct mblue_clock *mc);
	int (*uninit)(struct mblue_clock *mc);
};

struct mblue_clock *mblue_clock_get_instance(void);
#endif   /* ----- #ifndef mblue_clock_INC  ----- */
