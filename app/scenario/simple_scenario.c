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
#include "mblue_system.h"
#include "mblue_hash.h"
#include "mblue_clock.h"
#include "mblue_power_guard.h"
#include "rpc_helper.h"
#include "pb_schema.h"
#include "event_helper.h"

#include "locator_scenario.h"
#include "locator_constants.h"
#include "locator_packet.h"

#include "board_cdma_locator.h"

#define	REMOVE_COMPILER_ELEMENT_GUARD(a, b)		\
	(void)a;(void)b

static DECLARE_EVENT_QUEUE_HEAD(delayed_event_queue);
static struct LOCATOR_SCENARIO *ls;
static struct mblue_clock_entity current_time;
static uint16_t battery_window[BATTERY_WINDOW_SIZE];
static uint32_t battery_index = 0;
const uint16_t locator_battery_mark[10] = {
	3500, 3550, 3600, 3680, 3730, 3770, 3820, 3880, 3950, 4010
};

static struct mblue_clock_entity *parse_modem_time(char *timer)
{
	if (!timer)
		return NULL;

	char *p = timer;
	memset(&current_time, 0x0, sizeof(struct mblue_clock_entity));
	char buffer[8] = {0};

// year
	memcpy(buffer, p, 4);
	current_time.year = atoi(buffer);
// month
	p += 5;
	memset(buffer, 0x0, sizeof(buffer));
	memcpy(buffer, p, 2);
	current_time.month = atoi(buffer);
// day
	p += 3;
	memset(buffer, 0x0, sizeof(buffer));
	memcpy(buffer, p, 2);
	current_time.day = atoi(buffer);
// hour
	p += 3;
	memset(buffer, 0x0, sizeof(buffer));
	memcpy(buffer, p, 2);
	current_time.hour = atoi(buffer);
// minute
	p += 3;
	memset(buffer, 0x0, sizeof(buffer));
	memcpy(buffer, p, 2);
	current_time.minute = atoi(buffer);
//second
	p += 3;
	memset(buffer, 0x0, sizeof(buffer));
	memcpy(buffer, p, 2);
	current_time.second = atoi(buffer);

	return &current_time;
}

static void locator_sync_calendar()
{
	char time_buffer[24] = {0};
	if (MBLUE_OK == modem_time(time_buffer, 24)) {
		struct mblue_clock_entity *time = parse_modem_time(time_buffer);
		if (time) {
			struct mblue_clock *clock = mblue_clock_get_instance();
			if (clock) {
				clock->set_calendar(clock, time);
			}
		}
	}
}

static void locator_try_to_connect()
{
	ls->pg->inc(ls->pg);
	ls->online = LOCATOR_CONNECTING;
	led_action(LED_ACTION_CONNECTING);
	iot_start();
}

static void locator_network_error_handler()
{
	if (ls->err > LOCATOR_MAX_SEND_TRY) {
		mblue_system_reset();
	}
}

void *locator_get_pending_event(uint32_t type)
{
	if(__query_pending_event(type)) {
		return delayed_event_queue.get_element(&delayed_event_queue, type);
	}
	return NULL;
}

mblue_errcode locator_add_pending_data(
	uint32_t type, void *data, size_t len, enum data_add_direction direction)
{
	void *smart_data; 
	uint8_t *p, *q, *r;
	mblue_errcode rc;
	struct event_queue_element *ele;
	struct mblue_byte_bundler bundle;

	rc = MBLUE_ERR_CORRUPT_MEM;
	if(!(__query_pending_event(type))) {
		if (len >= LOCATOR_MAX_CACHE_DATA_LEN) {
			goto exited;
		}
		rc = MBLUE_ERR_NOMEM;
		smart_data = mblue_serialize_mono_buffer(data, len);
		if (!smart_data) {
			goto exited;
		}

		rc = MBLUE_OK;
		add_pending_event(&delayed_event_queue,					
			type,								
			EVENT_TIMEOUT_FOREVER,							
			NULL,
			smart_data, NULL, NULL, NULL,						
			NULL);
		goto exited;
	}

	ele = locator_get_pending_event(type);
	if (!ele) {
		goto exited;
	}
	smart_data = ele->smart_data;
	rc = mblue_unserialize_mono_buffer(smart_data, &bundle);
	if (rc != MBLUE_OK) {
		goto exited;
	}

	_ASSERT(bundle.byte);
	_ASSERT(bundle.len);
	rc = MBLUE_ERR_NOMEM;
	if (bundle.len + len >= LOCATOR_MAX_CACHE_DATA_LEN) {
		LOGGER(LOG_ERROR," too much data in cache, clean them...\r\n");
		r = mblue_serialize_mono_buffer(data, len);
		if (!r) {
			goto exited;
		}
		rc = MBLUE_OK;
		goto final_clean;
	}

	p = locator_data_cache;
	r = p;
	if (direction == DATA_ADD_APPEND) {
		q = p + bundle.len;	
	} else {
		q = p;
		p = q + len;
	}

	memcpy(q, data, len);
	memcpy(p, bundle.byte, bundle.len);
	r = mblue_serialize_mono_buffer(locator_data_cache, bundle.len + len);
	if (!r) {
		rc = MBLUE_ERR_NOMEM;
	}

final_clean:
	mblue_free(smart_data);
	ele->smart_data = r; 
exited:
	return rc;
}

void locator_release_event(uint32_t type)
{
	if(__query_pending_event(type)) {
		delayed_event_queue.wakeup(&delayed_event_queue, type, NULL);
	}
}

static void locator_send_cached_packet(void)
{
	uint8_t *p;
	size_t length;
	mblue_errcode rc;
	struct event_queue_element *ele;
	struct mblue_byte_bundler bundle = {0, NULL};

	p = NULL;
	length = 0;
	ele = locator_get_pending_event(LOCATOR_ABOUTTO_SEND);
	if (ele->smart_data) {
		_ASSERT(PB_PAYLOAD(ele->smart_data));
		rc = mblue_unserialize_mono_buffer(ele->smart_data, &bundle);
		if (rc == MBLUE_OK) {
			_ASSERT(bundle.len < LOCATOR_MAX_CACHE_DATA_LEN);
			memcpy(locator_data_cache, bundle.byte, bundle.len);
			locator_release_event(LOCATOR_ABOUTTO_SEND);
			p = locator_data_cache;
			length = bundle.len;
		}
	}

	if (p && length) {
		rc = iot_send(p, length);
		if (rc != MBLUE_OK) {
			locator_add_pending_data(
				LOCATOR_ABOUTTO_SEND, p, length, DATA_ADD_INSERT);
			ls->err++;
			locator_network_error_handler();
		} else {
			ls->err = 0;
		}
	}
}

static void locator_send_packet(uint8_t *buffer, size_t len)
{
	locator_add_pending_data(
		LOCATOR_ABOUTTO_SEND, buffer, len, DATA_ADD_APPEND);

	if (ls->online == LOCATOR_CONNECTED) {
		locator_send_cached_packet();
	}
}

static mblue_errcode __touch_worker()
{
	int rssi;
	void *p, *q;
	dl_111 gps = {0};

	q = gps_get();
	if (q) {
		_ASSERT(PB_PAYLOAD(q));
		mblue_unserialize_struct_with_layout(q, dl_111_fields, &gps);
		mblue_free(q);
		
	}
	rssi = modem_rssi();
	p = locator_touch_packet(
		gps.num1, gps.num2, gps.num3, 
		ls->battery, rssi, ls->touch_interval, 0, ls->acc ? 0 : 1);
	locator_send_packet(p, packet_len(p));
	LOGGER(LOG_INFO," cmd1 %u\r\n", locator_packet_get_sequence(p));
	return MBLUE_OK;
}

static mblue_errcode __next_touch(struct event_queue_element *e, void *data)
{
	if ((__query_pending_event(LOCATOR_PENDING_TOUCH))) {
		local_signal(GET_CURRENT_CONTEXT(), 
			MBLUE_LOCATOR_TOUCH_LOOP, &ls->ms_entity, NULL);
	}
	return MBLUE_ELEMENT_EXIT;
}

static mblue_errcode next_touch(void)
{
	return	reschedule_delayed_event_with_type(
		&delayed_event_queue, 
		(ls->touch_interval) * 1000, 
		LOCATOR_PENDING_TOUCH, __next_touch);
}

static mblue_errcode alive_worker(struct event_queue_element *e, void *data)
{
	void *p;

	REMOVE_COMPILER_ELEMENT_GUARD(e, data);
	p = locator_event_packet(EVENT_KEEPALIVE, 0);
	locator_send_packet(p, packet_len(p));
	return MBLUE_OK;
}

void locator_touch_worker_start()
{
	gps_service_start();
	next_touch();
}

void locator_touch_worker_stop()
{
	gps_service_stop();
	cancel_delayed_event(&delayed_event_queue, LOCATOR_PENDING_TOUCH);
}

static void __cancel_touch(void)
{
	locator_touch_worker_stop();
	schedule_delayed_event_with_type(
		&delayed_event_queue, 
		KEEPALIVE_INTERVAL, 
		LOCATOR_KEEP_ALIVE, alive_worker);
}

static mblue_errcode cancel_touch(struct event_queue_element *e, void *data)
{
	REMOVE_COMPILER_ELEMENT_GUARD(e, data);

	__cancel_touch();
	return MBLUE_ELEMENT_EXIT;
}

static void locator_process_server_setup(dl_server_setup *packet)
{
	uint32_t value;
	enum server_setup_cmd key;

	key = (enum server_setup_cmd)packet->key;
	value = packet->value;
	_ASSERT(key < SETUP_MAX);
	switch(key) {
	case SETUP_TOUCH_INTERVAL:
		if (value > 86400) {
			LOGGER(LOG_ERROR," error interval %u\r\n", value);
			return;
		}
		ls->touch_interval = value;
		next_touch();
		LOGGER(LOG_DEBUG," server setup interval %d\r\n", value);
		break;
	case SETUP_ACC:
		if (value > ACC_SWITCH_MAX) {
			LOGGER(LOG_ERROR," error acc control %u\r\n", value);
			return;
		}
		if (value) {
			vehicle_unlock();
		} else {
			vehicle_lock();
		}
		break;
	case SETUP_LOG:
		if (value > LOG_LEVEL_MAX) {
			LOGGER(LOG_ERROR," error log level %u\r\n", value);
		}

		{
			struct log_manager *lm = logger_get_instance();
			lm->set_level(lm, value);
		}
		break;
	case SETUP_REBOOT:
		LOGGER(LOG_INFO," server acquired reboot... %u\r\n", value);
		mblue_system_reset();
		break;
	case SETUP_TOUCH_PERIOD:
		if (value > 86400) {
			LOGGER(LOG_ERROR," error period %u\r\n", value);
			return;
		}
		ls->touch_period = value;
		LOGGER(LOG_DEBUG," server setup interval %d\r\n", value);
		break;
	case SETUP_VIBRATION_THRESHOLD:
		vibration_set_threshold(value);
		LOGGER(LOG_DEBUG," server setup vibration %d\r\n", value);
		break;
	case SETUP_FORCED_TOUCH:
		if(!(__query_pending_event(LOCATOR_PENDING_TOUCH))) {
			locator_touch_worker_start();
		}
		if (ls->acc) {
			reschedule_delayed_event_with_type(
				&delayed_event_queue, 
				(ls->touch_period) * 1000, 
				LOCATOR_ABOUTO_SLEEP, cancel_touch);
			LOGGER(LOG_DEBUG," server forced touch \r\n");
		}
		break;
	default:
		break;
	}

}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  locator_process_server_packet
 *  Description:  process server push
 *	 return:  bytes that are processed   
 * =====================================================================================
 */
size_t locator_process_server_packet(uint8_t *buffer, size_t len)
{
	size_t i, consumed;
	uint8_t value[12] = {0};
	struct locator_packet_head *head;

	i = 0;
	consumed = 0;
	head = (struct locator_packet_head *)buffer;
	if (head->magic != LOCATOR_PROTOCOL_MAGIC) {
		for (i = 1; i < len; i++) {
			if (buffer[i] ==  LOCATOR_PROTOCOL_MAGIC) {
				break;
			}
		}
		if (i == len) {
			LOGGER(LOG_ERROR," server packet find no magic\r\n");
			return len;
		}
		head = (struct locator_packet_head *)(buffer + i);
		consumed += i;
	}
	_ASSERT(head->magic == LOCATOR_PROTOCOL_MAGIC);
	if (head->ver != LOCATOR_PROTOCAL_VERSION) {
		consumed++;
		goto exited;
	}
//	LOGGER(LOG_DEBUG," server.magic : 0x%x\r\n", head->magic);
//	LOGGER(LOG_DEBUG," server.version : 0x%d\r\n", head->ver);
//	LOGGER(LOG_DEBUG," server.cmd : 0x%x\r\n", head->cmd);
//	LOGGER(LOG_DEBUG," server.packet.length : 0x%d\r\n", head->len);
//	LOGGER(LOG_DEBUG," server.packet.seqence : 0x%d\r\n", head->seq);

	if (len < head->len) {
		// need more data
		//_ASSERT(FALSE);
		goto exited;
	}
	consumed += head->len;
	if (decode_packet(head, value) != MBLUE_OK) {
		LOGGER(LOG_ERROR," server pushed an unknown packet\r\n");
		goto exited;
	}
	switch (head->cmd) {
	case CMD_SETUP: {
			dl_server_setup *setup_packet = (dl_server_setup *)value;
			locator_process_server_setup(setup_packet);
		}
		break;
	case CMD_ACK:	{
			dl_ack *ack_packet = (dl_ack *)value;
			if (ack_packet->ack) {
				LOGGER(LOG_ERROR," server acked %u error\r\n", ack_packet->seq);
			}
			LOGGER(LOG_INFO," ack %u\r\n", ack_packet->seq);
		}
		break;
	default:
		LOGGER(LOG_ERROR," server cmd unknown\r\n");
		break;
	}
exited:
	return consumed; 
}

static void activate_vibration_guard(void)
{
	vibration_init();
}

static void cancel_vibration_guard(void)
{
	vibration_uninit();
	cancel_delayed_event(&delayed_event_queue, LOCATOR_ACTIVATE_VIBRATION_GUARD);
}

static uint32_t battery_percent_get(void)
{
	uint8_t count;
	uint32_t sum, i;
	if (!battery_index) {
		return 0;
	}

	sum = 0;
	count = MIN(battery_index, BATTERY_WINDOW_SIZE);
	for(i = 0; i < count; i++) {
		sum += battery_window[i];
	}
	sum = sum / count;
	if (sum < locator_battery_mark[0]) {
		return 0;
	}
	if (sum > locator_battery_mark[9]) {
		return 100;
	}

	i = 0;
	while (locator_battery_mark[i++] < sum);
	return i * 10;
}

static void battery_touch()
{
	uint32_t index;

	index = battery_index++ % BATTERY_WINDOW_SIZE;
	battery_window[index] = locator_get_battery();
	ls->battery = battery_percent_get();
}

static mblue_errcode watchdog_feeder(struct event_queue_element *e, void *data)
{
	REMOVE_COMPILER_ELEMENT_GUARD(e, data);

	watchdog_feed();
	return MBLUE_OK;
}

static mblue_errcode battery_worker(struct event_queue_element *e, void *data)
{
	REMOVE_COMPILER_ELEMENT_GUARD(e, data);

	battery_touch();
	return MBLUE_OK;
}

static void locator_sensor_init()
{
	ls->acc = acc_init();	
	if (ls->acc) {
		activate_vibration_guard();
	}
	ls->power = power_init();
	adc_init();
#ifndef DEBUG
	watchdog_init();
#endif
}
/*
static void locator_sensor_uninit()
{
	acc_uninit();
	vibration_uninit();
	power_uninit();
}
*/
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  power_event
 *  Description:  power signal change event
 * =====================================================================================
 */
static mblue_errcode power_event(void *obj, void *data)
{
	void *p;
	uint32_t state;
	enum locator_report_event event;

	state = GET_INTEGER(data);
	event = (enum locator_report_event)((uint16_t)EVENT_POWER_LOSTS + state);
	_ASSERT(event < EVENT_MAX);
	if (ls->power != state) {
		LOGGER(LOG_INFO," POWER %u\r\n", state);
		ls->power = state;
		p = locator_event_packet(event, 0);
		locator_send_packet(p, packet_len(p));
	}

	return MBLUE_OK;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  vibration_event
 *  Description:  vibration event 
 * =====================================================================================
 */
static mblue_errcode vibration_event(void *obj, void *data)
{
	void *p;
	uint32_t value;
	
	LOGGER(LOG_INFO," !@#$%^&*\r\n");

	if (ls->acc) {
		value = GET_INTEGER(data);
		p = locator_event_packet(EVENT_VIBRATION, value);
		locator_add_pending_data(
			LOCATOR_ABOUTTO_SEND, p, packet_len(p), DATA_ADD_APPEND);

		switch(ls->online) {
		case LOCATOR_CONNECTED:
			if(!(__query_pending_event(LOCATOR_PENDING_TOUCH))) {
				locator_touch_worker_start();
			}
			reschedule_delayed_event_with_type(
				&delayed_event_queue, 
				(ls->touch_period) * 1000, 
				LOCATOR_ABOUTO_SLEEP, cancel_touch);
			break;
		case LOCATOR_DISCONNECTED:
			locator_try_to_connect();
			reschedule_delayed_event_with_type(
				&delayed_event_queue, 
				(ls->touch_period) * 1000, 
				LOCATOR_PENDING_TOUCH, NULL);
			break;
		default:
			break;
		}
		if((__query_pending_event(LOCATOR_KEEP_ALIVE))) {
			cancel_delayed_event(&delayed_event_queue, LOCATOR_KEEP_ALIVE);
		}
	}
	return MBLUE_OK;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  acc_event
 *  Description:  acc change event 
 * =====================================================================================
 */
static mblue_errcode acc_event(void *obj, void *data)
{
	void *p;
	uint32_t state;
	enum locator_report_event event;

	state = GET_INTEGER(data);
	event = (enum locator_report_event)(
		(uint16_t)EVENT_ACC_ON + state);
	_ASSERT(event < EVENT_MAX);
	if (ls->acc != state) {
		if (state) { // ACC OFF
			LOGGER(LOG_INFO," ACC OFF \r\n");
			// 1.stop touch worker
			if((__query_pending_event(LOCATOR_PENDING_TOUCH))) {
				reschedule_delayed_event_with_type(
					&delayed_event_queue, 
					(ls->touch_period) * 1000, 
					LOCATOR_ABOUTO_SLEEP, cancel_touch);
			}
			// 2.activate vibration guard
			activate_vibration_guard();

		} else { // ACC ON
			LOGGER(LOG_INFO," ACC ON \r\n");
			// 1.start touch worker
			if (ls->online == LOCATOR_DISCONNECTED) {
				locator_try_to_connect();
			} else {
				locator_touch_worker_start();
			}
			// 2.cancel vibration guard
			cancel_vibration_guard();
			// 3.cancel sleep timer
			cancel_delayed_event(&delayed_event_queue, LOCATOR_ABOUTO_SLEEP);
			// 4.cancel keepalive timer
			cancel_delayed_event(&delayed_event_queue, LOCATOR_KEEP_ALIVE);
		}
		ls->acc = state;
		p = locator_event_packet(event, 0);
		locator_add_pending_data(
			LOCATOR_ABOUTTO_SEND, p, packet_len(p), DATA_ADD_APPEND);
	}

	return MBLUE_OK;
}

static mblue_errcode touch_loop(void *obj, void *data)
{
	__touch_worker();
	next_touch();

	return MBLUE_OK;
}

static mblue_errcode timer_wakeup(void *obj, void *data)
{
	struct event_queue_head *q;
	mblue_errcode rc;
	uint32_t type;

	q = &delayed_event_queue;
	if (POINTER_S(data)) {
		type = ((struct event_queue_element *)(GET_POINTER(data)))->type;
		rc = MBLUE_ERR_TIMEOUT;
		q->wakeup(q, type, MAKE_INTEGER(rc));
	}

	return MBLUE_OK;
}

static mblue_errcode locator_network_connected_handler(void *obj, void *data)
{
	void *packet;

	ls->online = LOCATOR_CONNECTED;
	locator_sync_calendar();
	led_action(LED_ACTION_CONNECTED);
	packet = locator_init_packet();
	if (packet) {
		locator_add_pending_data(
			LOCATOR_ABOUTTO_SEND, packet, packet_len(packet), DATA_ADD_INSERT);
		locator_send_cached_packet();
	}
	if (!ls->acc || (__query_pending_event(LOCATOR_PENDING_TOUCH))) {
		locator_touch_worker_start();
		if (ls->acc) {
			reschedule_delayed_event_with_type(
				&delayed_event_queue, 
				(ls->touch_period) * 1000, 
				LOCATOR_ABOUTO_SLEEP, cancel_touch);
		}
	} else {
		// start keepalive worker
		reschedule_delayed_event_with_type(
			&delayed_event_queue, 
			KEEPALIVE_INTERVAL, 
			LOCATOR_KEEP_ALIVE, alive_worker);
	}
	return MBLUE_OK;
}

static mblue_errcode locator_network_disconnected_handler(void *obj, void *data)
{
	ls->pg->dec(ls->pg);
	ls->online = LOCATOR_DISCONNECTED;
	led_action(LED_ACTION_SLEEP);
	locator_touch_worker_stop();
	cancel_delayed_event(&delayed_event_queue, LOCATOR_KEEP_ALIVE);
	if (!ls->acc) {
		locator_try_to_connect();
	} 
	return MBLUE_OK;
}

static mblue_errcode locator_data_handler(void *obj, void *data)
{
	mblue_errcode rc;
	struct mblue_byte_bundler bundle = {0, NULL};

	_ASSERT(PB_PAYLOAD(data));
	rc = MBLUE_ERR_PB_DECODE;
	if (mblue_unserialize_mono_buffer(
			data,  &bundle)) {
		_ASSERT(FALSE);
		goto exited;
	}

#ifdef DEBUG
	log_service_hex("iot_recv", (const char *)bundle.byte, bundle.len);
#endif
	locator_process_server_packet(bundle.byte, bundle.len);
	rc = MBLUE_OK;
exited:
	return rc;

}

//TODO add wakeup signal from ext gpio from int 
int locator_try_to_sleep(uint32_t tick)
{
#ifdef LOCATOR_LOW_POWER
	if (ls->pg->sleep(ls->pg)) {
		uint32_t tick_num;

		tick_num = 2 * ( tick - 1UL ); 

		RTC_WakeUpCmd(DISABLE);
		RTC_SetWakeUpCounter(tick_num);
		RTC_WakeUpCmd(ENABLE);

		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		return 0;
	}
#endif
	return -1;
}

void locator_exit_sleep()
{
#ifdef LOCATOR_LOW_POWER
	RTC_WakeUpCmd(DISABLE);
#endif
}

static struct handler_item signal_items[] = {
	HANDLER_ITEM(MBLUE_TIMER_WAKEUP, timer_wakeup),
	HANDLER_ITEM(MBLUE_LOCATOR_TOUCH_LOOP, touch_loop),
	HANDLER_ITEM(MBLUE_GPIO_ACC_EVENT, acc_event),
	HANDLER_ITEM(MBLUE_GPIO_POWER_EVENT, power_event),
	HANDLER_ITEM(MBLUE_GPIO_VIBTATION_EVENT, vibration_event),
	HANDLER_ITEM(MBLUE_IOT_NETWORK_CONNECTED, locator_network_connected_handler),
	HANDLER_ITEM(MBLUE_IOT_NETWORK_DISCONNECTED, locator_network_disconnected_handler),
	HANDLER_ITEM(MBLUE_IOT_DATA_ARRIVED, locator_data_handler),
	HANDLER_ITEM(END_ITEM_ID, NULL),
};

//@override
static void scenario_launch(struct mblue_segment *ms)
{
	ls = (struct LOCATOR_SCENARIO *)ms;
	ls->pg = mblue_power_guard_get_instance();

	ls->touch_interval = GPS_WORKER_INTERVAL;
	ls->touch_period = TOUCH_ACTIVE_LASTS;
	ms->queue = &delayed_event_queue;
	delayed_event_queue.init(ms->queue, ms);
	delayed_event_queue.__timer_function = default_timer_event;
	mblue_subscribe(ms, MBLUE_GPIO_MODULE);
	mblue_subscribe(ms, MBLUE_IOT_SERVICE);

	locator_sensor_init();
	battery_touch();
	mblue_clock_get_instance();
	schedule_delayed_event_with_type(
		&delayed_event_queue, 
		BATTERY_WORKER_INTERVAL * 1000, 
		LOCATOR_PENDING_BATTERY_SCAN, battery_worker);
	schedule_delayed_event_with_type(
		&delayed_event_queue, 
		WATCH_DOG_INTERVAL * 1000, 
		LOCATOR_PENDING_WATCHDOG_FEED, watchdog_feeder);

	LOGGER(LOG_DEBUG, " Locator %d.%d.%d.%d\r\n", 
				LOCATOR_VERSION_MAJOR, 
				LOCATOR_VERSION_MINOR, 
				LOCATOR_VERSION_HARDWARE, 
				LOCATOR_VERSION_BUILD);
	if (!ls->acc) {
		locator_try_to_connect();
	}
}

//@override
static int scenario_init(struct	LOCATOR_SCENARIO *s)
{
	struct mblue_segment *ms;

	// 1.init super segment
	ms = &s->ms_entity;
	init_segment(ms);

	// 2.setup basic info and handler
	ms->major = MBLUE_LOCATOR_SCENARIO;
	ms->items_array[SIGNAL]		= signal_items;
	ms->on_launch			= scenario_launch;
	
	return 0;
}

DECLARE_SINGLETON_SCENARIO(LOCATOR_SCENARIO)
