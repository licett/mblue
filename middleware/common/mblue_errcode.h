/*
 * =====================================================================================
 *
 *       Filename:  mblue_errcode.h
 *
 *    Description:  defines all errcodes here
 *
 *        Version:  1.0
 *        Created:  12/18/2017 02:18:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  mblue_errcode_INC
#define  mblue_errcode_INC
typedef enum {
	MBLUE_REMOTE_CALL_PENDING = -64,
	MBLUE_WAIT_ACK,
	MBLUE_ERR_RESET,
	MBLUE_ERR_STATE_NEED_DELAY,
	MBLUE_ERR_UNKNOWN,
	MBLUE_ERR_UNKNOWN_AT_RESP,
	MBLUE_ERR_PPP,
	MBLUE_ERR_DNS,
	MBLUE_ERR_SOCKET,
	MBLUE_ERR_INVALID_PARAMETER,
	MBLUE_ERR_PB_ENCODE,
	MBLUE_ERR_PB_DECODE,
	MBLUE_ERR_TIMEOUT,
	MBLUE_ERR_CORRUPT_MEM,
	MBLUE_ERR_NOMEM,
	MBLUE_ERR_NOELEMENT,
	MBLUE_ERR_SYSBUS,
	MBLUE_ERR_SEMAPHORE,
	MBLUE_ERR_STRING_FMT,
	MBLUE_ELEMENT_EXIT = -1,
	MBLUE_OK = 0,
} mblue_errcode;
#define	IN_ERRCODE_RANGE(v)	(v >= MBLUE_REMOTE_CALL_PENDING && v <= MBLUE_OK)					/*  */
#endif   /* ----- #ifndef mblue_errcode_INC  ----- */
