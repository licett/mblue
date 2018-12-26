/*
 * =====================================================================================
 *
 *       Filename:  mblue_task_entry.h
 *
 *    Description:  task entries of mblue
 *
 *        Version:  1.0
 *        Created:  12/29/2017 04:44:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  mblue_task_entry_INC
#define  mblue_task_entry_INC

#include "mblue_task.h"
#include "mblue_stdtask.h"

#include "system_bus_service.h"
#include "simple_scenario.h"

mblue_errcode user_space_on_create(struct mblue_task *task)
{
		segment_bind(task, GET_SEGMENT_FROM_SCENARIO_INSTANCE(SIMPLE_SCENARIO));
        return MBLUE_OK;
}

mblue_errcode bus_on_create(struct mblue_task *task)
{
	segment_bind(task, (struct mblue_segment *)
                     get_system_bus_service_instance());
	return MBLUE_OK;
}

#endif   /* ----- #ifndef mblue_task_entry_INC  ----- */
