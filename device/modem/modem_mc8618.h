/*
 * =====================================================================================
 *
 *       Filename:  modem_mc8618.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/21/2017 01:42:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  modem_mc8618_INC
#define  modem_mc8618_INC
#include "mblue_stddefine.h"
#include "mblue_fifo.h"


/*-----------------------------------------------------------------------------
 *  mc8618 commands definition
 *-----------------------------------------------------------------------------*/
#define mc8618_cmd_hello		"at"				/* say hello to modem */
#define mc8618_cmd_set_noecho		"ate0"				/* do not echo */
#define mc8618_cmd_set_autosleep	"at+cta=1"			/* try to sleep automatically */
#define mc8618_cmd_get_csq		"at+csq?"			/* query signal strength */
#define mc8618_cmd_get_creg		"at+creg?"			/* query if modem has registered in base station */
#define mc8618_cmd_get_imei		"at+zmeid"			/* get modem's zmeid */
#define mc8618_cmd_get_iccid		"at+geticcid"			/* get modem's iccid */
#define mc8618_cmd_get_time		"at+cclk?"			/* query current time */
#define mc8618_cmd_get_ppps		"at+zpppstatus"			/* query status of ppp connection */
#define mc8618_cmd_get_tcps		"at+zipstatus=%d"		/* query status of tcp connection */
#define mc8618_cmd_get_ip		"at+zipgetip"			/* query current ip of modem */
#define mc8618_cmd_set_num		"at+zpnum=#777"			/* set business num */
#define mc8618_cmd_set_userpass		"at+zpidpwd=card,card"		/* set username and password */
#define mc8618_cmd_set_async_creg	"at+creg=1"			/* set business num */
#define mc8618_cmd_open_ppp		"at+zpppopen"			/* open a ppp connection */
#define mc8618_cmd_open_tcp		"at+zipsetup=%d,%s,%d"		/* open a tcp connection */
#define mc8618_cmd_tcp_send		"at+zipsend=%d,%zu\r"		/* send data to tcp server */
#define mc8618_cmd_tcp_close		"at+zipclose=%d"		/* close a tcp connection */
#define mc8618_cmd_ppp_close		"at+zpppclose"			/* close a ppp connection */
#define mc8618_cmd_sleep		"at+zdormant"			/* reset modem */
#define mc8618_cmd_reset		"at+cfun"			/* reset modem */
#define mc8618_cmd_tail			"\r\n"				/* tell modem that is all */

/*-----------------------------------------------------------------------------
 *  mc8618 urc definition
 *-----------------------------------------------------------------------------*/
#define	MC8618_URC_ZIND_CHANGE		"+ZIND:"			/* modem info */
#define	MC8618_URC_CREG_CHANGE		"+CREG:"			/* creg info */
#define	MC8618_URC_SLEEP_INFO		"+ZDORMANT: "			/* sleep automatically */
#define	MC8618_URC_PPP_CHANGE		"+ZPPPSTATUS: "			/* status of ppp connection changed */
/*#define	MC8618_URC_IP_GOT		"+ZIPGETIP:"			[> ip info <]*/
#define	MC8618_URC_IP_GOT		"+ZNEWIP:"			/* ip info */
#define	MC8618_URC_DNS_GOT		"+ZDNSGETIP: "			/* ip info */
#define	MC8618_URC_TCP_ESTABLISHED	"+ZTCPESTABLISHED:"		/* connection of tcp was established */
#define	MC8618_URC_TCP_CLOSED		"+ZTCPCLOSED:"			/* connection of tcp was closed */
#define MC8618_URC_TCP_RECV		"+ZIPRECV:"			/* receive data from tcp connection */
#define MC8618_URC_CONNECTION_INFO	"+ZCCNT:"			/* a connection has been established */

enum mc8618_urc_index {
	ZIND_CHANGE,
	CREG_CHANGE,
	SLEEP_INFO,
	PPP_CHANGE,
	IP_GOT,
	DNS_GOT,
	TCP_ESTABLISHED,
	TCP_CLOSED,
	TCP_RECV,
	CONNECTION_INFO,
	URC_MAX
};
enum mc8618_cmd_index {
	HELLO,
	SET_NOECHO,
	SET_AUTOSLEEP,
	GET_CSQ,
	GET_CREG,
	GET_IMEI,
	GET_ICCID,
	GET_TIME,
	GET_PPPS,
	GET_TCPS,
	GET_IP,
	SET_NUM,
	SET_USERPASS,
	SET_ASYNCCREG,
	OPEN_PPP,
	OPEN_TCP,
	TCP_SEND,
	TCP_CLOSE,
	PPP_CLOSE,
	MOD_SLEEP,
	MOD_RESET,
};

enum modem_connection_status {
    MODEM_CONNECTION_STATUS_DISCONNECTED = -1,
    MODEM_CONNECTION_STATUS_UNKNOWN = 0,
    MODEM_CONNECTION_STATUS_CONNECTED = 1,
};

#define MODEM_NSOCKETS			5
#define MODEM_CONNECT_TIMEOUT		5000
#define MODEM_CIPCFG_RETRIES		10

struct cellular {
	const struct cellular_ops *ops;
	const char *apn;
	uint16_t at_set_index;

	/* Private fields. */
	enum modem_connection_status ppp_status;
	enum modem_connection_status socket_status[MODEM_NSOCKETS];
	struct mblue_fifo *recv_fifo;
};

struct cellular_ops {
    mblue_errcode (*poweron)(struct cellular *modem);
    mblue_errcode (*attach)(struct cellular *modem);
    mblue_errcode (*auto_sleep)(struct cellular *modem);
    mblue_errcode (*detach)(struct cellular *modem);
    mblue_errcode (*pdp_open)(struct cellular *modem, const char *apn);
    mblue_errcode (*pdp_close)(struct cellular *modem);

    /** Read modem serial number (IMEI). */
    mblue_errcode (*imei)(struct cellular *modem, char *buf, size_t len);
    /** Read SIM serial number (ICCID). */
    mblue_errcode (*iccid)(struct cellular *modem, char *iccid, size_t len);
    /** Read modem time . */
    mblue_errcode (*mtime)(struct cellular *modem, char *time, size_t len);

    /** Get network registration status. */
    int (*creg)(struct cellular *modem);
    /** Get signal strength. */
    int (*rssi)(struct cellular *modem);

    mblue_errcode (*socket_connect)(struct cellular *modem, int connid, const char *host, uint16_t port);
    int (*socket_send)(struct cellular *modem, int connid, const void *buffer, uint32_t amount);
    size_t (*socket_recv)(struct cellular *modem, int connid, void *buffer, uint32_t length);
    size_t (*socket_peek)(struct cellular *modem, int connid);
    mblue_errcode (*socket_waitack)(struct cellular *modem, int connid);
    mblue_errcode (*socket_close)(struct cellular *modem, int connid);
    mblue_errcode (*poweroff)(struct cellular *modem);
};

struct cellular *modem_mc8618_get_instance(void);
#endif   /* ----- #ifndef modem_mc8618_INC  ----- */
