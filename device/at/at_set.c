/*
 * =====================================================================================
 *
 *       Filename:  at_set.c
 *
 *    Description:  definitions of commands and scanner about AT sets that are 
 *    supported in mblue
 *
 *        Version:  1.0
 *        Created:  12/22/2017 08:52:10 PM
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
#include "mblue_stddefine.h"
#include "at_set.h"
#include "modem_mc8618.h"

#define	AT_SET_ITEM(device, cmd)	\
	{device##_cmd_##cmd, device##_##cmd##_##scanner}

#define	CHECK_PATTERN(string, code)			\
	do {						\
		const char *p = string;			\
		if (!strncmp(line, p, strlen(p))) {	\
			return code;	\
		}					\
	} while (0)

const char *mc8618_urc_responses[] = {
	[ZIND_CHANGE]		= MC8618_URC_ZIND_CHANGE,
	[CREG_CHANGE]		= MC8618_URC_CREG_CHANGE,
	[SLEEP_INFO]		= MC8618_URC_SLEEP_INFO,
	[PPP_CHANGE]		= MC8618_URC_PPP_CHANGE,	
	[IP_GOT]		= MC8618_URC_IP_GOT,
	[DNS_GOT]		= MC8618_URC_DNS_GOT,
	[TCP_ESTABLISHED]	= MC8618_URC_TCP_ESTABLISHED,
	[TCP_CLOSED]		= MC8618_URC_TCP_CLOSED,
	[TCP_RECV]		= MC8618_URC_TCP_RECV,	
	[CONNECTION_INFO]	= MC8618_URC_CONNECTION_INFO,
	NULL
};

const char *mc8618_urc_response_get(size_t index)
{
	size_t enties;

	enties = sizeof(mc8618_urc_responses) / sizeof(mc8618_urc_responses[0]); 
	return index < enties ? 
		mc8618_urc_responses[index] : NULL;
}

static enum at_response_type mc8618_urc_scan(struct at_sets *set, const char *line, size_t len)
{
	if (at_prefix_in_table(line, mc8618_urc_responses) >= 0)
		return AT_RESPONSE_URC;
	return AT_RESPONSE_UNKNOWN;
}

static enum at_response_type mc8618_hello_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("at", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_set_noecho_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("ate0", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_set_autosleep_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("TRYING", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_csq_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+CSQ", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_creg_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+CREG", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_imei_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+ZMEID:", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_iccid_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+GETICCID:", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_time_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+CCLK:", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_ppps_scanner(struct at_sets *set, const char *line, size_t len)
{

	CHECK_PATTERN("+ZPPPSTATUS", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_tcps_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_get_ip_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_set_num_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_set_userpass_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_set_async_creg_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_open_ppp_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_open_tcp_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+ZDNSGETIP:", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_tcp_send_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("+ZIPSEND:", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_tcp_close_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_ppp_close_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_sleep_scanner(struct at_sets *set, const char *line, size_t len)
{
	CHECK_PATTERN("TRYING", AT_RESPONSE_FINAL);
	CHECK_PATTERN("READY", AT_RESPONSE_FINAL);
	return AT_RESPONSE_UNKNOWN;
}
static enum at_response_type mc8618_reset_scanner(struct at_sets *set, const char *line, size_t len)
{
	return AT_RESPONSE_UNKNOWN;
}
static int mc8618_recv_scan(struct at_sets *set, const char *line, size_t len)
{
	int rc;
	size_t data_len;
	uint32_t fmt_len;
	const char *p, *q, *r; 
	char tmp[24] = {0};
	const char *data_promt = "+ZIPRECV:";

	rc = -1;
	p = line;
	q = p;

	if (len < strlen(data_promt) + 4 ||
		memcmp(p, data_promt, strlen(data_promt))) {
		goto exited;
	}
	q += strlen(data_promt) + 2; // to skip the first ","
	r = strstr(q, ",");
	if (!r) {
		goto exited;
	}
	fmt_len = r - p + 1;
	if (fmt_len >= sizeof(tmp) || fmt_len > len) {
		goto exited;
	}
	memcpy(tmp, p, fmt_len);
	if (sscanf(tmp, "+ZIPRECV:%*d,%zu,", &data_len) != 1) {
		goto exited;
	}
	rc = data_len;
exited:
	return rc;
}

static struct at_set_item mc8618_at_set_items[] = {
	[HELLO]		= AT_SET_ITEM(mc8618, hello),
	[SET_NOECHO]	= AT_SET_ITEM(mc8618, set_noecho),
	[SET_AUTOSLEEP]	= AT_SET_ITEM(mc8618, set_autosleep),
	[GET_CSQ]	= AT_SET_ITEM(mc8618, get_csq),
	[GET_CREG]	= AT_SET_ITEM(mc8618, get_creg),
	[GET_IMEI]	= AT_SET_ITEM(mc8618, get_imei),
	[GET_ICCID]	= AT_SET_ITEM(mc8618, get_iccid),
	[GET_TIME]	= AT_SET_ITEM(mc8618, get_time),
	[GET_PPPS]	= AT_SET_ITEM(mc8618, get_ppps),
	[GET_TCPS]	= AT_SET_ITEM(mc8618, get_tcps),
	[GET_IP]	= AT_SET_ITEM(mc8618, get_ip),
	[SET_NUM]	= AT_SET_ITEM(mc8618, set_num),
	[SET_USERPASS]	= AT_SET_ITEM(mc8618, set_userpass),
	[SET_ASYNCCREG] = AT_SET_ITEM(mc8618, set_async_creg),
	[OPEN_PPP]	= AT_SET_ITEM(mc8618, open_ppp),
	[OPEN_TCP]	= AT_SET_ITEM(mc8618, open_tcp),
	[TCP_SEND]	= AT_SET_ITEM(mc8618, tcp_send),
	[TCP_CLOSE]	= AT_SET_ITEM(mc8618, tcp_close),
	[PPP_CLOSE]	= AT_SET_ITEM(mc8618, ppp_close),
	[MOD_SLEEP]	= AT_SET_ITEM(mc8618, sleep),
	[MOD_RESET]	= AT_SET_ITEM(mc8618, reset),
};
static struct at_sets at_set_mc8618 = {
	.at_set_items = mc8618_at_set_items,
	.urc_scan = mc8618_urc_scan,
	.recv_scan = mc8618_recv_scan,
};
static struct at_sets *mblue_at_sets[] = {
	[AT_SET_MC8618] = &at_set_mc8618,
};
struct at_sets *mblue_at_sets_get(uint16_t index)
{
	return mblue_at_sets[index];
}

