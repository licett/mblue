/*
 * =====================================================================================
 *
 *       Filename:  locator.c
 *
 *    Description:  application entry of locator
 *
 *        Version:  1.0
 *        Created:  12/29/2017 07:09:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_stddefine.h"
#include "mblue_macro.h"
#include "mblue_task.h"
#include "mblue_message.h"
#include "mblue_heap.h"
#include "mblue_logger.h"
#include "mblue_global_rpc_method.h"

#include "simple_scenario.h"

//@override
static void scenario_launch(struct mblue_segment *ms)
{
	LOGGER(LOG_DEBUG, " hello again\n");
}

//@override
static int scenario_init(struct	SIMPLE_SCENARIO *s)
{
	struct mblue_segment *ms;

	// 1.init super segment
	ms = &s->ms_entity;
	init_segment(ms);

	// 2.setup basic info and handler
	ms->major = MBLUE_SIMPLE_SCENARIO;
	ms->on_launch			= scenario_launch;
	
	return 0;
}

DECLARE_SINGLETON_SCENARIO(SIMPLE_SCENARIO)
