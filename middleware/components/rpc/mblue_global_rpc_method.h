/*
 * =====================================================================================
 *
 *       Filename:  global_rpc_method.h
 *
 *    Description:  declare of all global rpc method
 *
 *        Version:  1.0
 *        Created:  12/23/2017 01:15:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  global_rpc_method_INC
#define  global_rpc_method_INC
#include "mblue_stddefine.h"

size_t mblue_uart_write(uint16_t com, uint8_t *data, size_t len);
void *mblue_at_execute(
	uint16_t at_set_index, uint16_t cmd_index, uint8_t *data, size_t len);

mblue_errcode modem_poweron();
mblue_errcode modem_poweroff();
mblue_errcode modem_attach();
mblue_errcode modem_ppp_open();
mblue_errcode modem_tcp_connect(int socket, const char *server, uint16_t port);
mblue_errcode modem_iccid(char *buffer, size_t len);
mblue_errcode modem_imei(char *buffer, size_t len);
mblue_errcode modem_time(char *buffer, size_t len);
int16_t modem_rssi();
int16_t modem_tcp_send(int socket, const void *data, size_t len);
int16_t modem_tcp_read(int socket, void *data, size_t len, uint32_t timeout);
mblue_errcode modem_tcp_close(int socket);
mblue_errcode i2c_write(
	uint16_t i2c_index, uint16_t slave, uint8_t reg, const void *buffer, size_t len);
int i2c_read(uint16_t i2c_index, uint16_t slave, uint8_t reg, void *buffer, size_t len);
void *gps_get();
mblue_errcode gps_service_start();
mblue_errcode gps_service_stop();
mblue_errcode iot_send(uint8_t *data, size_t len);
mblue_errcode iot_start(void);
mblue_errcode log_service_set_type(uint32_t type);
mblue_errcode log_service_set_level(uint32_t level);
mblue_errcode log_service_write(uint32_t level, uint8_t *source, void *fmt, ...);
mblue_errcode log_service_hex(const char *prefix, const char *buffer, size_t len);
mblue_errcode led_action(int action);
#endif   /* ----- #ifndef global_rpc_method_INC  ----- */
