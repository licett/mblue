/*
 * =====================================================================================
 *
 *       Filename:  locator.h
 *
 *    Description:  application entry of locator
 *
 *        Version:  1.0
 *        Created:  12/29/2017 07:11:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  locator_INC
#define  locator_INC
#include "mblue_macro.h"
#include "mblue_segment.h"

#define	LOCATOR_SCENARIO		mblue_locator_scenario
#define	LOCATOR_MAX_SEND_TRY		3			/*  */
enum locator_network_state {
	LOCATOR_DISCONNECTED,
	LOCATOR_CONNECTING,
	LOCATOR_CONNECTED
};

enum data_add_direction {
	DATA_ADD_INSERT,
	DATA_ADD_APPEND
};

struct	LOCATOR_SCENARIO {
	struct mblue_segment ms_entity;
	struct mblue_power_guard *pg;
	struct locator_state_maching *locator_sm;
	uint32_t touch_interval;
	uint32_t touch_period;
	uint32_t battery:8;
	uint32_t acc:1;
	uint32_t power:1;
	uint32_t online:6;
	uint32_t err:8;
};

DECLARE_SCENARIO_INCLUDE(LOCATOR_SCENARIO);
#endif   /* ----- #ifndef locator_INC  ----- */
