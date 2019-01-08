/*
 * =====================================================================================
 *
 *       Filename:  message_def.h
 *
 *    Description:  definition of messages
 *
 *        Version:  1.0
 *        Created:  2017/5/11 17:33:28
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  message_def_INC
#define  message_def_INC

/*-----------------------------------------------------------------------------
 *  message type
 *-----------------------------------------------------------------------------*/
enum MESSAGE_TYPE {
	SUBSCRIBE,
	SIGNAL,
	SYNC_CALL,
	ASYNC_CALL,
	CALL_RETURN,
	MESSAGE_TYPE_MAX
};

/*-----------------------------------------------------------------------------
 *  segment major number
 *-----------------------------------------------------------------------------*/
enum MESSAGE_MAJOR {
	MBLUE_BUS,
	MBLUE_SYSTEM,
	MBLUE_LOG_SERVICE, 
	MBLUE_SIMPLE_SCENARIO,

	MBLUE_USER_DEFINE1,
	MBLUE_USER_DEFINE2,
	MBLUE_USER_DEFINE3,
	SEGMENT_MAJOR_MAX
};

enum SYSTEM_MESSAGE_SOURCE {
	MESSAGE_SOURCE_TIMER = 0xEEF0,
	MESSAGE_SOURCE_HWI,
        MESSAGE_SOURCE_SWI, 
        MESSAGE_SOURCE_STACK,
};

/*delayed event type shared by all event queue*/
#define	MBLUE_EVENT_DELAYED_EVENT		0x80000000			

/*-----------------------------------------------------------------------------
 *  system messages
 *-----------------------------------------------------------------------------*/
#define	MBLUE_MINOR_DEFAULT_GAP			0x100			/*  */
// system message ids
#define	MBLUE_SYS_TURN_ON			(1U)	// system turn on
#define	MBLUE_SYS_TURN_OFF			(2U)	// system turn off
#define	MBLUE_SYS_LOW_POWER			(3U)	/*  */
#define	MBLUE_SYS_HIGH_POWER			(4U)	/*  */
#define	MBLUE_SYS_POWER_DOWN			(5U)	/*  */
#define	MBLUE_SYS_WDT				(6U)	/*  */
#define	MBLUE_SYS_WDT_OVERFLOW			(7U)	/*  */
#define	MBLUE_SYS_POWER_PLUGIN			(8U)	// power plugin
#define	MBLUE_SYS_POWER_PLUGOUT			(9U)	// power plugout

/*-----------------------------------------------------------------------------
 *  timer messages
 *-----------------------------------------------------------------------------*/
enum TIMER_MESSAGE {
	MBLUE_TIMER_SIGNAL_BASE,
	MBLUE_TIMER_WAKEUP,

	MBLUE_TIMER_RPC_BASE = MBLUE_TIMER_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_TIMER_GET,
        MBLUE_TIMER_INIT,	
        MBLUE_TIMER_START,
        MBLUE_TIMER_STOP,
        MBLUE_TIMER_UNINIT,
        MBLUE_TIMER_RELEASE
};


/*-----------------------------------------------------------------------------
 *  async messages
 *-----------------------------------------------------------------------------*/
enum ASYNC_MESSAGE {
	MBLUE_ASYNC_SIGNAL_BASE = MBLUE_TIMER_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_ASYNC_RPC_BASE = MBLUE_ASYNC_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_ASYNC_SCHEDULE,
	MBLUE_ASYNC_ADD,
};

enum GPIO_MODULE_MESSAGE {
	MBLUE_GPIO_MODULE_SIGNAL_BASE = MBLUE_ASYNC_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_GPIO_VIBTATION_EVENT,
	MBLUE_GPIO_POWER_EVENT,
	MBLUE_GPIO_ACC_EVENT,
};
/*-----------------------------------------------------------------------------
 *  uart module messages
 *-----------------------------------------------------------------------------*/
enum UART_MODULE_MESSAGE {
	MBLUE_UART_MODULE_SIGNAL_BASE = MBLUE_GPIO_MODULE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_UART1_RECV_COMPLETE,
	MBLUE_UART1_SEND_COMPLETE,
	MBLUE_UART1_DATA_FRAGMENT,
	MBLUE_UART2_RECV_COMPLETE,
	MBLUE_UART2_SEND_COMPLETE,
	MBLUE_UART2_DATA_FRAGMENT,
	MBLUE_UART3_RECV_COMPLETE,
	MBLUE_UART3_SEND_COMPLETE,
	MBLUE_UART3_DATA_FRAGMENT,

};

/*-----------------------------------------------------------------------------
 *  uart service messages
 *-----------------------------------------------------------------------------*/
enum UART_SERVICE_MESSAGE {
	MBLUE_UART_SERVICE_SIGNAL_BASE = MBLUE_UART_MODULE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_MODEM_DATA_FRAGMENT,
	MBLUE_MODEM_DATA_COMPLETE,

	MBLUE_UART_SERVICE_RPC_BASE = MBLUE_UART_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_UART_WRITE,
};

/*-----------------------------------------------------------------------------
 *  at service messages
 *-----------------------------------------------------------------------------*/
enum AT_SERVICE_MESSAGE {
	MBLUE_AT_SERVICE_SIGNAL_BASE = MBLUE_UART_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_AT_URC,
	/*MBLUE_AT_URC_AT_SET_MC8618 = MBLUE_AT_URC_AT + AT_SET_MC8618,
	MBLUE_AT_URC_AT_SET_ESP8266 = MBLUE_AT_URC_AT + AT_SET_ESP8266,*/

	MBLUE_AT_SERVICE_RPC_BASE = MBLUE_AT_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_AT_EXECUTE,
};

enum MODEM_SERVICE_MESSAGE {
	MBLUE_MODEM_SERVICE_SIGNAL_BASE = MBLUE_AT_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_MODEM_SERVICE_RPC_BASE = MBLUE_MODEM_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP, //B00
	MBLUE_MODEM_POWERON,
	MBLUE_MODEM_INFO,
	MBLUE_MODEM_RSSI,
	MBLUE_MODEM_IMEI,
	MBLUE_MODEM_ICCID,
	MBLUE_MODEM_TIME,
	MBLUE_MODEM_ATTACH,
	MBLUE_MODEM_DETTACH,
	MBLUE_MODEM_PPP_OPEN,
	MBLUE_MODEM_TCP_CONNECT,
	MBLUE_MODEM_TCP_SEND,
	MBLUE_MODEM_TCP_RECV,
	MBLUE_MODEM_TCP_CLOSE,
	MBLUE_MODEM_POWEROFF,
};

enum I2C_SERVICE_MESSAGE {
	MBLUE_I2C_SERVICE_SIGNAL_BASE = MBLUE_MODEM_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_I2C_SERVICE_RPC_BASE = MBLUE_I2C_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_I2C_READ,
	MBLUE_I2C_WRITE,
};
enum GPS_SERVICE_MESSAGE {
	MBLUE_GPS_SERVICE_SIGNAL_BASE = MBLUE_I2C_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_GPS_NOTIFY,

	MBLUE_GPS_SERVICE_RPC_BASE = MBLUE_GPS_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_GPS_SERVICE_START,
	MBLUE_GPS_SERVICE_STOP,
	MBLUE_GPS_GET,
};
enum IOT_SERVICE_MESSAGE {
	MBLUE_IOT_SERVICE_SIGNAL_BASE = MBLUE_GPS_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP, //1000
	MBLUE_IOT_STATE_MIGRATION,
	MBLUE_IOT_STATE_STOP,
	MBLUE_IOT_NETWORK_CONNECTED, 
	MBLUE_IOT_NETWORK_DISCONNECTED, 
	MBLUE_IOT_DATA_ARRIVED,
	MBLUE_IOT_YIELD, 

	MBLUE_IOT_SERVICE_RPC_BASE = MBLUE_IOT_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_IOT_SERVICE_SEND,
	MBLUE_IOT_SERVICE_START,
	MBLUE_IOT_SERVICE_STOP,
};
enum LOCATOR_SCENARIO_MESSAGE {
	MBLUE_LOCATOR_SCENARIO_SIGNAL_BASE = MBLUE_IOT_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP, //1200
	MBLUE_LOCATOR_SCENARIO_LOOP,
	MBLUE_LOCATOR_TOUCH_LOOP,

	MBLUE_LOCATOR_SCENARIO_RPC_BASE = MBLUE_LOCATOR_SCENARIO_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
};

enum LOG_SERVICE_MESSAGE
{
	MBLUE_LOG_SERVICE_SIGNAL_BASE = MBLUE_LOCATOR_SCENARIO_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_LOG_SERVICE_RPC_BASE = MBLUE_LOG_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_LOG_SERVICE_LOG,
	MBLUE_LOG_SERVICE_SET_TYPE,
        MBLUE_LOG_SERVICE_SET_LEVEL,
        MBLUE_LOG_SERVICE_HEX,
};

enum LED_SERVICE_MESSAGE
{
	MBLUE_LED_SERVICE_SIGNAL_BASE = MBLUE_LOG_SERVICE_RPC_BASE + MBLUE_MINOR_DEFAULT_GAP,

	MBLUE_LED_SERVICE_RPC_BASE = MBLUE_LED_SERVICE_SIGNAL_BASE + MBLUE_MINOR_DEFAULT_GAP,
	MBLUE_LED_SERVICE_ACTION,
};
#endif   /* ----- #ifndef message_def_INC  ----- */
