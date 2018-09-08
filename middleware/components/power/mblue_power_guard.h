/*
 * =====================================================================================
 *
 *       Filename:  locator_power.h
 *
 *    Description:  power manager
 *
 *        Version:  1.0
 *        Created:  03/07/2018 09:24:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  POWER_GUARD_INC
#define  POWER_GUARD_INC
#include "mblue_stddefine.h"
struct mblue_power_guard {
	uint32_t power_holder;

	void (*init)(struct mblue_power_guard *);
	void (*inc)(struct mblue_power_guard *);
	void (*dec)(struct mblue_power_guard *);
	bool (*sleep)(struct mblue_power_guard *);
};
struct mblue_power_guard *mblue_power_guard_get_instance();
#endif   /* ----- #ifndef POWER_GUARD_INC  ----- */
