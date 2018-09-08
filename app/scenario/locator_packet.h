/*
 * =====================================================================================
 *
 *       Filename:  locator_packet.h
 *
 *    Description:  locator packet related operations
 *
 *        Version:  1.0
 *        Created:  01/01/2018 04:39:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  LOCATOR_PACKET_INC
#define  LOCATOR_PACKET_INC
#include "mblue_stddefine.h"

#define	CDMA_MEID_LEN				15			/*  */
enum locator_report_event {
	EVENT_KEEPALIVE,
	EVENT_VIBRATION,
	EVENT_ACC_ON,
	EVENT_ACC_OFF,
	EVENT_POWER_LOSTS,
	EVENT_POWER_RESUMED,
	//EVENT_WAKEUP_BY_VIBRATION,
	EVENT_MAX
};

enum locator_cmd {
	CMD_INIT,
	CMD_TOUCH,
	CMD_EVENT,
	CMD_SETUP = 0x10,
	CMD_ACK = 0x80,
	CMD_MAX
};
enum server_setup_cmd {
	SETUP_TOUCH_INTERVAL,
	SETUP_ACC,
	SETUP_LOG,
	SETUP_REBOOT,
	SETUP_TOUCH_PERIOD,
	SETUP_VIBRATION_THRESHOLD,
	SETUP_FORCED_TOUCH,
	SETUP_MAX,
};
enum acc_switch {
	ACC_OFF,
	ACC_ON,
	ACC_SWITCH_MAX
};

struct locator_packet_head {
	uint8_t magic;
	uint8_t ver;
	uint8_t cmd;
	uint8_t pad;
	uint16_t len;
	uint16_t seq;
	uint8_t data[0];
};

// #ifdef DEBUG
// void hex_print(const char *prefix, const char* buf, size_t len);
// #endif
mblue_errcode decode_packet(void *data, void *dest_struct);
uint16_t packet_len(void *packet);
void *locator_touch_packet(
	int lng, int lat, int satellites, int power, int rssi, int interval, int type, int acc);
void *locator_event_packet(enum locator_report_event type, int value);
void *locator_init_packet();
uint16_t locator_packet_get_sequence(void *packet);
#endif   /* ----- #ifndef locator_packet_INC  ----- */
