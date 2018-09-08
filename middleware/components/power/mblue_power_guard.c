/*
 * =====================================================================================
 *
 *       Filename:  locator_power.c
 *
 *    Description:  locator power manager
 *
 *        Version:  1.0
 *        Created:  03/07/2018 09:18:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_power_guard.h"

static void power_init(struct mblue_power_guard *guard)
{
	guard->power_holder = 0;
}

static void power_inc(struct mblue_power_guard *guard)
{
	guard->power_holder++;
}

static void power_dec(struct mblue_power_guard *guard)
{
	if (guard->power_holder) {
		guard->power_holder--;
	}
}

static bool should_sleep(struct mblue_power_guard *guard)
{
	return guard->power_holder == 0;
}

static struct mblue_power_guard *power_guard = NULL;
static struct mblue_power_guard _power_guard = {
	.init = power_init,
	.inc = power_inc,
	.dec = power_dec,
	.sleep = should_sleep, 
};
struct mblue_power_guard *mblue_power_guard_get_instance()
{
	if (power_guard == NULL) {
		power_guard = &_power_guard;
		power_guard->init(power_guard);
	}
	return power_guard;
}
