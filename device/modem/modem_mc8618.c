/*
 * =====================================================================================
 *
 *       Filename:  at_device.c
 *
 *    Description:  basic definition of at devices, such as modem, gps etc  
 *
 *        Version:  1.0
 *        Created:  12/21/2017 01:40:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "mblue_stddefine.h"
#include "mblue_assert.h"
#include "mblue_logger.h"
#include "mblue_heap.h"
#include "mblue_global_rpc_method.h"
#include "mblue_task.h"
#include "pb_schema.h"
#include "at_set.h"

#include "modem_mc8618.h"
#include "board_cdma_locator.h"

#define	MAX_CREG_RETRY				10			/*  */
#define	MC8618_TIME_LEN				24			/*  */
#define	MC8618_SOCKET_RECEIVE_BUFFER_LEN	1024			/* buffer to cache socket data */
#define	MC8618_MAX_AT_CMD_LEN			256			/* max cmd length supportd here */
#define _NUMARGS(...)									\
	(sizeof((void *[]){0, ##__VA_ARGS__})/sizeof(void *)-1)
#define	CMD(dest_at_set, index)								\
	dest_at_set->at_set_items[index].cmd
#define	_at_cmd(m, i, ...)								\
	__at_cmd(m, i, CMD(mblue_at_sets_get(m->at_set_index), i), ##__VA_ARGS__)
#define at_scanf(_response, code, format, ...)						\
	do {										\
		if (!_response) {							\
			_ASSERT(FALSE);							\
			return code;							\
		}									\
		if (sscanf(_response, format, ##__VA_ARGS__)				\
						!= _NUMARGS(__VA_ARGS__)) {		\
			_ASSERT(FALSE);							\
			return code;							\
		}									\
	} while (0)
#define	parse_at_result(pp, bu, err_code, fmt, ...)					\
	do {										\
		_ASSERT(PB_PAYLOAD(pp));						\
		if (PB_PAYLOAD(pp)) {							\
			mblue_unserialize_mono_buffer(pp, &bu);				\
				at_scanf((const char *)bu.byte, err_code, fmt, ##__VA_ARGS__);		\
				mblue_free((void *)p);							\
		}									\
	} while(0)
#define	at_cmd_with_return_check(mo, code, patt, in, ...)				\
	do {										\
		char *pp;struct mblue_byte_bundler bu;int ret;				\
		pp = _at_cmd(mo, in, ##__VA_ARGS__);					\
		_ASSERT(PB_PAYLOAD(pp));						\
		if (PB_PAYLOAD(pp)) {							\
			mblue_unserialize_mono_buffer(pp, &bu);					\
			ret = strncmp((char *)(bu.byte), patt, strlen(patt));			\
			mblue_free(pp);								\
			if(ret) {								\
				return code;							\
			}									\
		}									\
	} while(0)
#define	at_cmd_no_check(mo, in, ...)							\
	do {										\
		char *pp;								\
		pp = _at_cmd(mo, in, ##__VA_ARGS__);					\
		_ASSERT(PB_PAYLOAD(pp));						\
		mblue_free(pp);								\
	} while(0)

			/*_ASSERT(FALSE);							\*/

static uint8_t mc8618_at_cmd_buffer[MC8618_MAX_AT_CMD_LEN];
static void *__at_cmd(struct cellular *modem, enum mc8618_cmd_index index, const char *fmt, ...)
{
	void *p;
	int len;
	va_list ap;

	va_start(ap, fmt);
	len = vsnprintf((char *)mc8618_at_cmd_buffer, 
		sizeof(mc8618_at_cmd_buffer) - 1, fmt, ap);
	va_end(ap);

	/* Bail out if we run out of space. */
	if (len >= (int)(sizeof(mc8618_at_cmd_buffer) - 1)) {
		_ASSERT(FALSE);
		return NULL;
	}

	LOGGER(LOG_DEBUG, " >> %s\r\n", mc8618_at_cmd_buffer);
	/* Append modem-style newline. */
	mc8618_at_cmd_buffer[len++] = '\r';


	p = mblue_at_execute(modem->at_set_index, index, 
		(uint8_t *)mc8618_at_cmd_buffer, len);
	_ASSERT(p != NULL);
	//LOGGER(" < %s\r\n", p);

	return p;
}

static mblue_errcode mc8618_poweron(struct cellular *modem)
{
	modem_powerup();
        return MBLUE_OK;
}

static mblue_errcode mc8618_imei(struct cellular *modem, char *buf, size_t len)
{
	char fmt[32];
	const char *p;
	mblue_errcode rc;
	struct mblue_byte_bundler bundler;

	rc = MBLUE_ERR_STRING_FMT;
	if (snprintf(fmt, sizeof(fmt), 
			"+ZMEID: 0x%%[0-9A-F]%ds", (int) len) >= (int) sizeof(fmt)) {
		_ASSERT(FALSE);
		return rc;
	}

	rc = MBLUE_OK;
	p = _at_cmd(modem, GET_IMEI);
	parse_at_result(p, bundler, MBLUE_ERR_UNKNOWN_AT_RESP, fmt, buf);
	buf[len - 1] = '\0';

	return rc;
}

static mblue_errcode mc8618_iccid(struct cellular *modem, char *buf, size_t len)
{
	char fmt[32];
	const char *p;
	mblue_errcode rc;
	struct mblue_byte_bundler bundler;

	rc = MBLUE_ERR_STRING_FMT;
	if (snprintf(fmt, sizeof(fmt), 
			"+GETICCID:0x%%[0-9A-F]%ds", (int) len) >= (int) sizeof(fmt)) {
		_ASSERT(FALSE);
		return rc;
	}

	rc = MBLUE_OK;
	p = _at_cmd(modem, GET_ICCID);
	parse_at_result(p, bundler, MBLUE_ERR_UNKNOWN_AT_RESP, fmt, buf);
	buf[len - 1] = '\0';

	return rc;
}

static mblue_errcode mc8618_time(struct cellular *modem, char *buf, size_t len)
{
	char fmt[16];
	const char *p;
	mblue_errcode rc;
	struct mblue_byte_bundler bundler;

	//+CCLK: "2018/01/22,11:27:52.570"
	rc = MBLUE_ERR_STRING_FMT;
	_ASSERT(len >= MC8618_TIME_LEN);
	if (snprintf(fmt, sizeof(fmt), 
			"+CCLK: \"%%s\"") >= (int) sizeof(fmt)) {
		_ASSERT(FALSE);
		return rc;
	}

	rc = MBLUE_OK;
	p = _at_cmd(modem, GET_TIME);
	parse_at_result(p, bundler, MBLUE_ERR_UNKNOWN_AT_RESP, fmt, buf);
	buf[len - 1] = '\0';

	return rc;
}

static int mc8618_creg(struct cellular *modem)
{
	int creg;
	const char *p;
	struct mblue_byte_bundler bundler;

	p = _at_cmd(modem, GET_CREG);
	parse_at_result(
		p, bundler, MBLUE_ERR_UNKNOWN_AT_RESP, "+CREG: %*d,%d", &creg);

	return creg;
}

static int mc8618_rssi(struct cellular *modem)
{
	int rssi;
	const char *p;
	struct mblue_byte_bundler bundler;

	p = _at_cmd(modem, GET_CSQ);
	parse_at_result(p, bundler, 
		MBLUE_ERR_UNKNOWN_AT_RESP, "+CSQ: %d,%*d", &rssi);

	return rssi;
}

static mblue_errcode mc8618_hello(struct cellular *modem)
{
	char *p;								
	mblue_errcode rc;
	p = _at_cmd(modem, HELLO);
	if (PB_PAYLOAD(p)) {
		mblue_free(p);							
		rc = MBLUE_OK;
	} else if (INTEGER_P(p)) {
		rc = (mblue_errcode)GET_INTEGER(p);
	} else {
		rc = MBLUE_ERR_UNKNOWN_AT_RESP;
	}

	return rc;
}
/*
static mblue_errcode mc8618_sleep(struct cellular *modem)
{
	at_cmd_no_check(modem, MOD_SLEEP);
        return MBLUE_OK;
}
*/
static mblue_errcode mc8618_set_auto_sleep(struct cellular *modem)
{
	at_cmd_with_return_check(modem, MBLUE_ERR_UNKNOWN_AT_RESP, "OK", SET_AUTOSLEEP);
        return MBLUE_OK;
}

static mblue_errcode mc8618_attach(struct cellular *modem)
{
	int retry;
	int rssi, creg;
	mblue_errcode rc;

	// say hello
	rc = mc8618_hello(modem);
	if (rc != MBLUE_OK) {
		goto exited;
	}
	at_cmd_no_check(modem, SET_NOECHO);

	rssi = mc8618_rssi(modem);
	_ASSERT(rssi > 0);

	retry = 0;
	rc = MBLUE_ERR_TIMEOUT;
	// creg
	do {
		creg = mc8618_creg(modem);
		task_sleep(1000);	
		retry++;
	} while (creg != 1 && retry < MAX_CREG_RETRY);

	rc = (creg == 1 ? MBLUE_OK : rc);
exited:
	return rc;
}

static mblue_errcode mc8618_detach(struct cellular *modem)
{
	return MBLUE_OK;
}

static mblue_errcode mc8618_pdp_open(struct cellular *modem, const char *apn)
{
	int ret;
	void *p;
	const char *q;
	const char *state;
	const char *pattern;
	mblue_errcode rc;
	struct mblue_byte_bundler bundler = {0, NULL};

	ret = -1;
	rc = MBLUE_ERR_UNKNOWN;
	if (modem->ppp_status == MODEM_CONNECTION_STATUS_CONNECTED) {
		rc = MBLUE_OK;
		goto exited;
	}
	at_cmd_with_return_check(modem, MBLUE_ERR_UNKNOWN_AT_RESP, "OK", SET_NUM);
	at_cmd_with_return_check(modem, MBLUE_ERR_UNKNOWN_AT_RESP, "OK", SET_USERPASS);
	
	pattern = "+ZPPPSTATUS: ";
	state = "OPENED";
	p = _at_cmd(modem, GET_PPPS);
	if (!PB_PAYLOAD(p)) {
		goto exited;
	}
	mblue_unserialize_mono_buffer(p, &bundler);
	q = strstr((char *)bundler.byte, pattern);
	if (q) {
		q += strlen(pattern);
		ret = strncmp(q, state, strlen(state));
	}
	mblue_free((void *)p);
	if (!ret) {
		rc = MBLUE_OK;
		modem->ppp_status = MODEM_CONNECTION_STATUS_CONNECTED; 
		goto exited;
	}

	// open ppp connection	
	at_cmd_with_return_check(modem, MBLUE_ERR_UNKNOWN_AT_RESP, "OK", OPEN_PPP);
	rc = MBLUE_WAIT_ACK;

exited:
	return rc;
}

static mblue_errcode mc8618_pdp_close(struct cellular *modem)
{
	modem->ppp_status = MODEM_CONNECTION_STATUS_DISCONNECTED;
        return MBLUE_OK;
}

static mblue_errcode mc8618_socket_connect(struct cellular *modem, int connid, const char *host, uint16_t port)
{

	int ret, ip1, ip2, ip3, ip4;
	char *p, *q, *r;
	const char *ok = "OK";
	const char *dns = "+ZDNSGETIP:";
	const char *failed = "failed";
	mblue_errcode rc;
	struct mblue_byte_bundler bundler;				

	// at+zipsetup=0,120.76.100.197,10002
	_ASSERT(modem->socket_status[connid] == MODEM_CONNECTION_STATUS_UNKNOWN 
		|| modem->socket_status[connid] == MODEM_CONNECTION_STATUS_DISCONNECTED);

	if (modem->ppp_status != MODEM_CONNECTION_STATUS_CONNECTED) {
		mc8618_pdp_open(modem, NULL);
		rc = MBLUE_ERR_PPP;
		goto exited_return;
	}
	rc = MBLUE_ERR_SOCKET;
	modem->socket_status[connid] = MODEM_CONNECTION_STATUS_UNKNOWN;
	p = _at_cmd(modem, OPEN_TCP, connid, host, port);
	if (!PB_PAYLOAD(p)) {
		goto exited_return;
	}
	rc = mblue_unserialize_mono_buffer(p, &bundler);					
	_ASSERT(rc == MBLUE_OK);
	if (rc) {
		goto exited;
	}

	r = (char *)bundler.byte;
	// modem return OK, just wait..
	ret = strncmp(r, ok, strlen(ok));			
	if(!ret) {								
		rc = MBLUE_WAIT_ACK;
		goto exited;
	}									

	// modem return +ZDNSGETIP
	ret = strncmp(r, dns, strlen(dns));			
	if(ret) {								
		LOGGER(LOG_DEBUG," unknow response %s from modem\r\n", r);
                _ASSERT(FALSE);
		rc = MBLUE_ERR_UNKNOWN_AT_RESP;
		goto exited;
	}

	// if modem returns +ZDNSGETIP: failed
	q = strstr(r, failed);
	if (q) {
		rc = MBLUE_ERR_DNS;
		goto exited;
	}

	// if modem returns +ZDNSGETIP:xxx.xxx.xxx.xxx
	ret = sscanf(r, "+ZDNSGETIP:%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	if (ret == 4) {
		rc = MBLUE_WAIT_ACK;
		goto exited;
	}

	// unknown dns string
	rc = MBLUE_ERR_UNKNOWN_AT_RESP;
	LOGGER(LOG_DEBUG," unknow dns response %s \r\n", r);

exited:
	mblue_free(p);								
exited_return:
	return rc;
}

static int mc8618_socket_send(struct cellular *modem, int connid, const void *buffer, uint32_t amount)
{
	char *p;
	struct mblue_byte_bundler bundler;
	int bytes;
	_ASSERT(modem->socket_status[connid] 
		== MODEM_CONNECTION_STATUS_CONNECTED);
	_ASSERT(amount < MC8618_MAX_AT_CMD_LEN);	

	// send cmd "at+zipsend=0,12\rxxxxxxx"
	p = (char *)mc8618_at_cmd_buffer;
	bytes = snprintf(p, 
		sizeof(mc8618_at_cmd_buffer), 
		CMD(mblue_at_sets_get(modem->at_set_index), TCP_SEND),
		connid, amount);
	_ASSERT(bytes < sizeof(mc8618_at_cmd_buffer));
	if (strlen(p) + amount < sizeof(mc8618_at_cmd_buffer) - 1) {
		memcpy(p + strlen(p), buffer, amount);
	}
	p = mblue_at_execute(
		modem->at_set_index, TCP_SEND, mc8618_at_cmd_buffer, bytes + amount); 
	parse_at_result(p, bundler, 
		MBLUE_ERR_SOCKET, "+ZIPSEND: %d", &bytes);
	if (bytes < 0) {
		bytes = 0;
	}

	/*mc8618_sleep(modem);*/
	return bytes;
}

static size_t mc8618_socket_recv(struct cellular *modem, int connid, void *buffer, uint32_t length)
{
	struct mblue_fifo *fifo;
	size_t len, tmp, l;

	fifo = modem->recv_fifo;
	len = fifo->len(fifo);

	// fetch the buffered data
	if (len) {
		tmp = len < length ? len : length;
		l = fifo->get(fifo, buffer, tmp);
		_ASSERT(l == tmp);
	}
	return l;
}

size_t mc8618_socket_peek(struct cellular *modem, int connid)
{
	struct mblue_fifo *fifo;
	size_t len;

	fifo = modem->recv_fifo;
	len = fifo->len(fifo);
	return len;
}

static mblue_errcode mc8618_socket_waitack(struct cellular *modem, int connid)
{
        return MBLUE_OK;
}

static mblue_errcode mc8618_socket_close(struct cellular *modem, int connid)
{
	if (modem->socket_status[connid] 
		== MODEM_CONNECTION_STATUS_DISCONNECTED) {
		return MBLUE_OK;
	}

	// send cmd "at+zipclose=0"
	at_cmd_with_return_check(modem, MBLUE_ERR_SOCKET, "OK", TCP_CLOSE, connid);
	return MBLUE_WAIT_ACK;
}

static mblue_errcode mc8618_poweroff(struct cellular *modem)
{
	for (uint32_t i = 0; i < MODEM_NSOCKETS; i++) {
		modem->socket_status[i] = MODEM_CONNECTION_STATUS_DISCONNECTED;
	}
	modem->ppp_status = MODEM_CONNECTION_STATUS_DISCONNECTED;
	modem_powerdown();
	return MBLUE_OK;
}

struct cellular_ops ops = {
	.poweron = mc8618_poweron,
	.attach = mc8618_attach,
	.auto_sleep = mc8618_set_auto_sleep,
	.detach = mc8618_detach,

	.pdp_open = mc8618_pdp_open,
	.pdp_close = mc8618_pdp_close,

	.imei = mc8618_imei,
	.iccid = mc8618_iccid,
	.rssi = mc8618_rssi,
	.mtime = mc8618_time,
	.socket_connect = mc8618_socket_connect,
	.socket_send = mc8618_socket_send,
	.socket_recv = mc8618_socket_recv,
	.socket_peek = mc8618_socket_peek,
	.socket_waitack = mc8618_socket_waitack,
	.socket_close = mc8618_socket_close,
	.poweroff = mc8618_poweroff,
};

static struct cellular _modem_mc8618 = {
	.ops = &ops,
	.at_set_index = AT_SET_MC8618,
};

static struct cellular *modem_mc8618 = NULL;
struct cellular *modem_mc8618_get_instance(void)
{
	if (!modem_mc8618) {
		modem_mc8618 = &_modem_mc8618;
		modem_mc8618->recv_fifo = 
			mblue_fifo_alloc(MC8618_SOCKET_RECEIVE_BUFFER_LEN);
		_ASSERT(modem_mc8618->recv_fifo);
	}
	return modem_mc8618;
}
