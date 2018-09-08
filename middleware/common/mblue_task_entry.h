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
#include "task_template.h"

#include "gps_service.h"
#include "async_service.h"
#include "at_service.h"
#include "uart_service.h"
#include "i2c_service.h"
#include "system_bus_service.h"
#include "modem_service.h"
#include "iot_service.h"
#include "led_service.h"
#include "log_service.h"

#include "locator_scenario.h"
mblue_errcode user_space_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SCENARIO_INSTANCE(LOCATOR_SCENARIO));
        return MBLUE_OK;
}

mblue_errcode gps_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(GPS_SERVICE));
	return MBLUE_OK;
}

mblue_errcode async_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(ASYNC_SERVICE));
	return MBLUE_OK;
}

mblue_errcode modem_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(MODEM_SERVICE));
	return MBLUE_OK;
}

mblue_errcode at_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(AT_SERVICE));
	return MBLUE_OK;
}

mblue_errcode device_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(UART_SERVICE));
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(I2C_SERVICE));
	return MBLUE_OK;
}

mblue_errcode mqtt_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(MQTT_SERVICE));
	return MBLUE_OK;
}

mblue_errcode bus_on_create(struct mblue_task *task)
{
	segment_bind(task, (struct mblue_segment *)
                     get_system_bus_service_instance());
	return MBLUE_OK;
}

mblue_errcode log_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(LOG_SERVICE));
	return MBLUE_OK;
}

mblue_errcode led_on_create(struct mblue_task *task)
{
	segment_bind(task, GET_SEGMENT_FROM_SERVICE_INSTANCE(LED_SERVICE));
	return MBLUE_OK;
}

#endif   /* ----- #ifndef mblue_task_entry_INC  ----- */
