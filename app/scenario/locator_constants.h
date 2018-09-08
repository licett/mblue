/*
 * =====================================================================================
 *
 *       Filename:  locator_constants.h
 *
 *    Description:  constants definition of locator
 *
 *        Version:  1.0
 *        Created:  01/01/2018 04:47:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  LOCATOR_CONSTANTS_INC
#define  LOCATOR_CONSTANTS_INC

/*-----------------------------------------------------------------------------
 *  protocol 
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_PROTOCOL_MAGIC			0xFE			/*  */
#define	LOCATOR_PROTOCAL_VERSION		5			/*  */

#define	LOCATOR_SERVER_URL			"tracker.madaogo.com"
#define	LOCATOR_SERVER_PORT			8090			
/*#define	LOCATOR_SERVER_URL			"120.76.100.197"	
#define	LOCATOR_SERVER_PORT			10002			*/

/*-----------------------------------------------------------------------------
 *  business
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_CHANNEL				"YONGCHUAN"		/* 永川电信 */
#define	LOCATOR_PRODUCT				"TRACKER"		/* locator used in tracking vechile  */
enum locator_device_type {
	LOCATOR_CDMA,
	LOCATOR_GPRS,
	LOCATOR_NB
};

/*-----------------------------------------------------------------------------
 *  build
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_MAX_VERSION_NUM_LEN		15			/* xxx.xxx.xxx.xxx */
#define	LOCATOR_VERSION_MAJOR			1			/* major version */
#define	LOCATOR_VERSION_MINOR			2			/* minor version */
#define	LOCATOR_VERSION_HARDWARE		2			/* hardware version */
#define	LOCATOR_VERSION_BUILD			24			/* build version */

/*-----------------------------------------------------------------------------
 *  cache
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_MAX_CACHE_DATA_LEN		256			/*  */

/*-----------------------------------------------------------------------------
 *  event
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_ABOUTTO_SEND				0x00000001		/*  */
#define	LOCATOR_ALREADY_RECV				0x00000002		/*  */
#define	LOCATOR_PENDING_TOUCH				0x00000004		/*  */
#define	LOCATOR_PENDING_WATCHDOG_FEED			0x08000000		/*  */
#define	LOCATOR_PENDING_BATTERY_SCAN			0x10000000		/*  */
#define	LOCATOR_ACTIVATE_VIBRATION_GUARD		0x20000000		/*  */
#define	LOCATOR_ABOUTO_SLEEP				0x40000000		/*  */
#define	LOCATOR_KEEP_ALIVE				0x80000000		/*  */

/*-----------------------------------------------------------------------------
 *  other
 *-----------------------------------------------------------------------------*/
#define	LOCATOR_MIN_SERVER_CMD_LEN			12			/*  */
#define	LOCATOR_MAX_SERVER_CMD_LEN			128			/*  */
#define	GPS_WORKER_INTERVAL				5			/* 1 second */
#define	WATCH_DOG_INTERVAL				20			/* 20 second */
#define	BATTERY_WORKER_INTERVAL				60			/* 1 minute */
#define	TOUCH_ACTIVE_LASTS				60			/* 5 minutes */
#define	KEEPALIVE_INTERVAL				1000 * 180		/* 2 minutes */
#define	BATTERY_WINDOW_SIZE				8			/* display battery of window * interval */
extern uint8_t locator_data_cache[];
#endif   /* ----- #ifndef locator_constants_INC  ----- */
