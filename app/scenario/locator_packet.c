/*
 * =====================================================================================
 *
 *       Filename:  locator_packet.c
 *
 *    Description:  locator packet related operations
 *
 *        Version:  1.0
 *        Created:  01/01/2018 04:38:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "locator_packet.h"
#include "locator_constants.h"
#include "mblue_heap.h"
#include "mblue_assert.h"
#include "mblue_logger.h"
#include "mblue_hash.h"
#include "mblue_global_rpc_method.h"
#include "pb_schema.h"

#include "board_cdma_locator.h"
#define	LOCATOR_MAX_PACKET_LEN		128		/*  */

static uint16_t locator_seqence = 0;
uint8_t locator_data_cache[LOCATOR_MAX_CACHE_DATA_LEN];
static uint8_t locator_packet_cache[LOCATOR_MAX_PACKET_LEN];
static const pb_field_t *locator_fields[] = {
	[CMD_INIT]	= dl_server_init_fields,
	[CMD_TOUCH]	= dl_server_touch_fields,
	[CMD_EVENT]	= dl_server_event_fields,
	[CMD_SETUP]	= dl_server_setup_fields,
	[CMD_ACK]	= dl_ack_fields,
};

mblue_errcode decode_packet(void *data, void *dest_struct)
{
	uint8_t *p;
	mblue_errcode rc;
	pb_istream_t stream;
	enum locator_cmd cmd;
	const pb_field_t *fields;
	struct locator_packet_head *ph;
	uint32_t *server_sign, client_sign;

	rc = MBLUE_ERR_INVALID_PARAMETER;
	ph = (struct locator_packet_head *)data;
	_ASSERT(ph->len < LOCATOR_MAX_SERVER_CMD_LEN);
	_ASSERT(ph->len > LOCATOR_MIN_SERVER_CMD_LEN);
	if (ph->len < LOCATOR_MIN_SERVER_CMD_LEN ||
		ph->len > LOCATOR_MAX_SERVER_CMD_LEN) {
		goto exited;
	}

	p = (uint8_t *)data;
	p += ph->len - 4;
	server_sign = (uint32_t *)p;
	client_sign = sdbm_mem(data, ph->len - 4);
	if (*server_sign != client_sign) {
		_ASSERT(FALSE);
		LOGGER(LOG_DEBUG," err sign detection\r\n");
		goto exited;
	}

	cmd = (enum locator_cmd)ph->cmd;
	_ASSERT(cmd > CMD_EVENT);
	_ASSERT(cmd < CMD_MAX);
	rc = MBLUE_ERR_PB_DECODE;
	fields = locator_fields[cmd];
	stream = pb_istream_from_buffer(ph->data, 
		ph->len - sizeof(struct locator_packet_head) - 4);
	if (!pb_decode(&stream, fields, dest_struct)) {
		goto exited;
	}
	rc = MBLUE_OK;
exited:
	return rc;
}

// static char test_array[512];
// void hex_print(const char *prefix, const char* buf, size_t len)
// {
// 	size_t i;
// 	char *p, *q;

// 	p = test_array;
// 	memset(test_array, 0, sizeof(test_array));
// 	_ASSERT(p != NULL);
	
// 	for(i = 0; i < len; i++) {
// 		q = p + strlen(p);
// 		unsigned char c = buf[i]; // must use unsigned char to print >128 value
// 		if( c < 16) {
// 			snprintf(q, 3, "0%x", c);
// 		} else {
// 			snprintf(q, 3, "%x", c);
// 		}
// 	}
// 	LOGGER(LOG_DEBUG, " %s : %s\r\n", prefix, p);
// }

static void *_construct_packet(enum locator_cmd cmd, void *data)
{
	uint32_t sign;
	uint8_t *p;
	size_t body, head;
	pb_ostream_t stream;
	struct locator_packet_head *ph;
	const pb_field_t *fields;

	p = NULL;
	_ASSERT(cmd < CMD_MAX);
	fields = locator_fields[cmd];
	if (!pb_get_encoded_size(&body, fields, data)) {
		goto exited;
	}

	head = sizeof(struct locator_packet_head);
	memset(locator_packet_cache, 0, sizeof(locator_packet_cache));
	p = locator_packet_cache;
	ph = (struct locator_packet_head *)locator_packet_cache;
	ph->magic = LOCATOR_PROTOCOL_MAGIC;
	ph->ver = LOCATOR_PROTOCAL_VERSION;
	ph->cmd = cmd;
	ph->seq = locator_seqence++;
	ph->len = body + head + 4;
	
	if (ph->len > sizeof(locator_packet_cache)) {
		_ASSERT(FALSE);
		goto exited;
	}

	stream = pb_ostream_from_buffer((void *)(p + head), body);
	if (!pb_encode(&stream, fields, data)) {
		goto exited;
	}
	sign = sdbm_mem(p, ph->len - 4);	
	memcpy(p + head + body, &sign, 4);
	/*hex_print("send", (const char *)p, ph->len);*/
exited:
	return p;
}

uint16_t packet_len(void *packet)
{
	struct locator_packet_head *ph;

	ph = (struct locator_packet_head *)packet;
	_ASSERT(ph->magic == LOCATOR_PROTOCOL_MAGIC);
	_ASSERT(ph->ver == LOCATOR_PROTOCAL_VERSION);

	return ph->len;
}

void *locator_touch_packet(
	int lng, int lat, int satellites, int power, int rssi, int interval, int type, int acc)
{
	dl_server_touch touch = {lng, lat, satellites, power, rssi, interval, type, acc};
	return _construct_packet(CMD_TOUCH, &touch);
}

uint16_t locator_packet_get_sequence(void *packet)
{
	struct locator_packet_head *ph;

	ph = (struct locator_packet_head *)packet;
	_ASSERT(ph->magic == LOCATOR_PROTOCOL_MAGIC);
	_ASSERT(ph->ver == LOCATOR_PROTOCAL_VERSION);
	_ASSERT(ph->len < LOCATOR_MAX_SERVER_CMD_LEN);
	_ASSERT(ph->len > LOCATOR_MIN_SERVER_CMD_LEN);

	return ph->seq;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  locator_event_packet
 *  Description:  here we add gps point to satisfy server developer
 * =====================================================================================
 */
void *locator_event_packet(enum locator_report_event type, int value)
{
	_ASSERT(type < EVENT_MAX);
	dl_server_event event = {type, value ? 1 : 0, value};
	return _construct_packet(CMD_EVENT, &event);
}

static void mem2hex(void *in, size_t len, void *out)
{
	uint32_t i;
	uint8_t *in_data, *dst;
	static const char *HEXCHARS = "0123456789abcdef";

	_ASSERT(in);
	_ASSERT(out);
	in_data = (uint8_t *)in;
	dst = (uint8_t *)out;
	for (i = 0; i < len; ++i) {
		*dst++ = HEXCHARS[*in_data >> 4 & 0xF];
		*dst++ = HEXCHARS[*in_data & 0xF];
		in_data++;
	}
}

void *locator_init_packet()
{
	int ret;
	dl_server_init *p, init_data;
	uint32_t board_id[3];

	memset(&init_data, 0, sizeof(init_data));
	p = &init_data;
	memcpy(p->channel, LOCATOR_CHANNEL, strlen(LOCATOR_CHANNEL));
	memcpy(p->product, LOCATOR_PRODUCT, strlen(LOCATOR_PRODUCT));
	p->device_type = LOCATOR_CDMA;
	if (locator_board_get_id(board_id, sizeof(board_id))) {
		goto info_err;
	}
	mem2hex((void *)board_id, sizeof(board_id), p->device_id);
	if (modem_imei(p->imei, CDMA_MEID_LEN + 1) != MBLUE_OK 
		|| strlen(p->imei) != CDMA_MEID_LEN) {
		_ASSERT(FALSE);
		goto info_err;
	}
	if (modem_iccid(p->iccid, 24) != MBLUE_OK) {
		_ASSERT(FALSE);
		goto info_err;
	}
	ret = snprintf(p->firmware_version, 
		LOCATOR_MAX_VERSION_NUM_LEN, 
		"%d.%d.%d.%d", 
		LOCATOR_VERSION_MAJOR, 
		LOCATOR_VERSION_MINOR, 
		LOCATOR_VERSION_HARDWARE, 
		LOCATOR_VERSION_BUILD);
	if (ret < 0 || ret > LOCATOR_MAX_VERSION_NUM_LEN) {
		_ASSERT(FALSE);
		goto info_err;
	}
	return _construct_packet(CMD_INIT, p);

info_err:
	return NULL;
}
