/*
 * =====================================================================================
 *
 *       Filename:  global_rpc_method.c
 *
 *    Description:  declare of all global rpc method
 *    TODO design a template to register global rpc
 *
 *        Version:  1.0
 *        Created:  12/23/2017 01:14:45 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "mblue_global_rpc_method.h"
#include "mblue_heap.h"
#include "mblue.pb.h"
#include "mblue_logger.h"
#include "rpc_helper.h"
#include "pb_schema.h"

size_t mblue_uart_write(uint16_t com, uint8_t *data, size_t len)
{
	void *payload, *return_data;
	size_t bytes;
	struct mblue_byte_bundler bundler = {len, data};
	dl_uart_request param = {
		com, {
			{.encode = __mblue_pb_write_bytes}, 
			(void *)&bundler
		}
	};

	bytes = 0;
	payload = mblue_serialize_rpc("uart_write", PB_12, &param);
	if (payload) {
		return_data = mblue_remote_call(MBLUE_UART_SERVICE, MBLUE_UART_WRITE, payload);
		bytes = GET_INTEGER(return_data);
	}
        return bytes;

}

void *mblue_at_execute(
	uint16_t at_set_index, uint16_t cmd_index, uint8_t *data, size_t len)
{
	void *payload, *return_data;
	struct mblue_byte_bundler bundler = {len, data};
	dl_at_request param = {
		at_set_index, 
		cmd_index, {
			{.encode = __mblue_pb_write_bytes}, 
			(void *)&bundler
		}
	};


	return_data = NULL;
	payload = mblue_serialize_rpc("at_execute", PB_112, &param);
	if (payload) {
		return_data = mblue_remote_call(MBLUE_AT_SERVICE, MBLUE_AT_EXECUTE, payload);
	}
        return return_data;
}

mblue_errcode modem_poweron()
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;

	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("modem_poweron", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_POWERON, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode modem_poweroff()
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;

	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("modem_poweroff", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_POWEROFF, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode modem_imei(char *buffer, size_t len)
{
	mblue_errcode rc;
	void *payload, *data;
	struct mblue_byte_bundler bundler = {0, NULL};

	data = NULL;
	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("modem_imei", PB_0, NULL);
	if (payload) {
		rc = MBLUE_ERR_UNKNOWN;
		data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_IMEI, payload);
		_ASSERT(PB_PAYLOAD(data));
		if (data) {
			rc = mblue_unserialize_mono_buffer(data, &bundler);
			_ASSERT(rc == MBLUE_OK);
			_ASSERT(bundler.len < len);
			if (rc == MBLUE_OK) {
				if (bundler.len < len) {
					memcpy(buffer, bundler.byte, bundler.len);
				} else {
					rc = MBLUE_ERR_INVALID_PARAMETER;
				}
			}
			mblue_free(data);
		}
	}
        return rc;
}

mblue_errcode modem_iccid(char *buffer, size_t len)
{
	mblue_errcode rc;
	void *payload, *data;
	struct mblue_byte_bundler bundler = {0, NULL};

	data = NULL;
	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("modem_iccid", PB_0, NULL);
	if (payload) {
		rc = MBLUE_ERR_UNKNOWN;
		data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_ICCID, payload);
		_ASSERT(PB_PAYLOAD(data));
		if (data) {
			rc = mblue_unserialize_mono_buffer(data, &bundler);
			_ASSERT(rc == MBLUE_OK);
			_ASSERT(bundler.len < len);
			if (rc == MBLUE_OK) {
				if (bundler.len < len) {
					memcpy(buffer, bundler.byte, bundler.len);
				} else {
					rc = MBLUE_ERR_INVALID_PARAMETER;
				}
			}
			mblue_free(data);
		}
	}
        return rc;
}

mblue_errcode modem_time(char *buffer, size_t len)
{
	mblue_errcode rc;
	void *payload, *data;
	struct mblue_byte_bundler bundler = {0, NULL};

	data = NULL;
	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("modem_time", PB_0, NULL);
	if (payload) {
		rc = MBLUE_ERR_UNKNOWN;
		data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_TIME, payload);
		_ASSERT(PB_PAYLOAD(data));
		if (data) {
			rc = mblue_unserialize_mono_buffer(data, &bundler);
			_ASSERT(rc == MBLUE_OK);
			_ASSERT(bundler.len < len);
			if (rc == MBLUE_OK) {
				if (bundler.len < len) {
					memcpy(buffer, bundler.byte, bundler.len);
				} else {
					rc = MBLUE_ERR_INVALID_PARAMETER;
				}
			}
			mblue_free(data);
		}
	}
        return rc;
}

int16_t modem_rssi()
{
	int rssi;
	void *payload, *data;

	rssi = -1;
	data = NULL;
	payload = mblue_serialize_rpc("modem_rssi", PB_0, NULL);
	if (payload) {
		data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_RSSI, payload);
		_ASSERT(INTEGER_P(data));
		if (data) {
			rssi = (int16_t)GET_INTEGER(data);
		}
	}
        return rssi;

}

mblue_errcode modem_attach()
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;

	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("modem_attach", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_ATTACH, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode modem_ppp_open()
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;

	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("modem_ppp_open", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_PPP_OPEN, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode modem_tcp_connect(int socket, const char *server, uint16_t port)
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;
	struct mblue_byte_bundler bundler = {strlen(server), (uint8_t *)server};
	dl_112 dl = {
		socket, 
		port, {
			{.encode = __mblue_pb_write_bytes}, 
			(void *)&bundler
		}
	};

	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("modem_tcp_connect", PB_113, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_TCP_CONNECT, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

int16_t modem_tcp_send(int socket, const void *data, size_t len)
{
	int bytes;
	void *payload, *return_data;
	return_data = NULL;
	struct mblue_byte_bundler bundler = {len, (uint8_t *)data};
	dl_12 dl = {
		socket, {{.encode = __mblue_pb_write_bytes}, (void *)&bundler}};

	bytes = -1;
	payload = mblue_serialize_rpc("modem_tcp_send", PB_12, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_TCP_SEND, payload);
		bytes = (int16_t)GET_INTEGER(return_data);
	}
        return bytes;
}

int16_t modem_tcp_read(int socket, void *data, size_t len, uint32_t timeout)
{
	int bytes;
	mblue_errcode rc;
	void *payload, *return_data;
	struct mblue_byte_bundler bundler = {0, NULL};
	dl_11 dl = {socket, len};

	bytes = -1;
	return_data = NULL;
	payload = mblue_serialize_rpc("modem_tcp_read", PB_111, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_TCP_RECV, payload);
		_ASSERT(return_data);

		if(PB_PAYLOAD(return_data)) {
			// get and decode data from socket 
			rc = mblue_unserialize_mono_buffer(return_data, &bundler);
			_ASSERT(rc == MBLUE_OK);
			bytes = bundler.len;
			memcpy(data, bundler.byte, bundler.len);
			mblue_free(return_data);
			goto exited;
		}
		bytes = (int16_t)GET_INTEGER(return_data);
	}
exited:
        return bytes;
}

mblue_errcode modem_tcp_close(int socket)
{
	mblue_errcode rc;
	dl_1 dl = {socket};
	void *payload, *return_data;

	rc = MBLUE_ERR_PB_ENCODE;
	return_data = NULL;
	payload = mblue_serialize_rpc("modem_tcp_close", PB_1, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_MODEM_SERVICE, MBLUE_MODEM_TCP_CLOSE, payload);
		_ASSERT(return_data);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode i2c_write(
	uint16_t i2c_index, uint16_t slave, uint8_t reg, const void *buffer, size_t len)
{
	void *payload, *return_data;
	mblue_errcode rc;
	struct mblue_byte_bundler bundler = {len, (uint8_t *)buffer};
	dl_1112 dl = {
		i2c_index, slave, reg, {{.encode = __mblue_pb_write_bytes}, (void *)&bundler}};

	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("i2c_write", PB_1112, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_I2C_SERVICE, MBLUE_I2C_WRITE, payload);
		_ASSERT(return_data);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

int i2c_read(uint16_t i2c_index, uint16_t slave, uint8_t reg, void *buffer, size_t len)
{
	int bytes;
	mblue_errcode rc;
	void *payload, *return_data;
	struct mblue_byte_bundler bundler = {0, NULL};
	dl_1111 dl = {i2c_index, slave, reg, len};

	bytes = -1;
	return_data = NULL;
	payload = mblue_serialize_rpc("i2c_read", PB_1111, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_I2C_SERVICE, MBLUE_I2C_READ, payload);
		_ASSERT(return_data);

		if(PB_PAYLOAD(return_data)) {
			// get and decode data from socket 
			rc = mblue_unserialize_mono_buffer(return_data, &bundler);
			_ASSERT(rc == MBLUE_OK);
			bytes = bundler.len;
			_ASSERT(bundler.len <= len);
			memcpy(buffer, bundler.byte, bundler.len);
			mblue_free(return_data);
			goto exited;
		}
		rc = (mblue_errcode)GET_INTEGER(return_data);
		bytes = rc;
	}
exited:
        return bytes;
}

void *gps_get()
{
	void *payload, *return_data;
	
	return_data = NULL;
	payload = mblue_serialize_rpc("gps_get", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_GPS_SERVICE, MBLUE_GPS_GET, payload);
	}
        return return_data;
}

mblue_errcode gps_service_start()
{
	mblue_errcode rc;
	void *payload, *return_data;
	
	return_data = NULL;
	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("gps_service_start", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_GPS_SERVICE, MBLUE_GPS_SERVICE_START, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode gps_service_stop()
{
	mblue_errcode rc;
	void *payload, *return_data;
	
	return_data = NULL;
	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("gps_service_stop", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_GPS_SERVICE, MBLUE_GPS_SERVICE_STOP, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode iot_send(uint8_t *data, size_t len)
{
	mblue_errcode rc;
	void *payload, *return_data;
	return_data = NULL;
	struct mblue_byte_bundler bundler = {len, (uint8_t *)data};
	dl_array dl = {
		{{.encode = __mblue_pb_write_bytes}, (void *)&bundler}};

	rc = MBLUE_ERR_UNKNOWN;
	payload = mblue_serialize_rpc("iot_send", PB_2, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_IOT_SERVICE, MBLUE_IOT_SERVICE_SEND, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode log_service_set_type(uint32_t type)
{
	mblue_errcode rc;
	void *payload, *return_data;

	return_data = NULL;
	rc = MBLUE_ERR_UNKNOWN;
	dl_1 dl = {type};
	payload = mblue_serialize_rpc("log_service_set_type", PB_1, &dl);
	if (payload) {
		return_data = mblue_remote_call(
		                  MBLUE_LOG_SERVICE, MBLUE_LOG_SERVICE_SET_TYPE, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
	return rc;
}

mblue_errcode log_service_set_level(uint32_t level)
{
	mblue_errcode rc;
	void *payload, *return_data;

	return_data = NULL;
	rc = MBLUE_ERR_UNKNOWN;
	dl_1 dl = {level};
	payload = mblue_serialize_rpc("log_service_set_level", PB_1, &dl);
	if (payload) {
		return_data = mblue_remote_call(
		                  MBLUE_LOG_SERVICE, MBLUE_LOG_SERVICE_SET_LEVEL, payload);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
	return rc;
}

static void log_notify(void *data, void *user_data){

}

static uint8_t *__log_get_file_name_pointer(uint8_t *full_path, uint32_t len)
{
	uint8_t *p, *q, c;
	size_t pos;
	
	p = full_path;
	q = NULL;
	pos = len;
	while (pos--) {
		c = *(p + pos);
		if (c == '\\' || c == '/') {
			q = p + pos + 1;
			break;
		}
	}
	return q;
}

mblue_errcode log_service_write(uint32_t level, uint8_t *source, void *fmt, ...)
{
	char *tmp;
	uint8_t *p;
	va_list args;
	void *payload;
	size_t log_len;
	mblue_errcode rc;
	
	if (level >= LOG_LEVEL_MAX) {
		rc = MBLUE_ERR_LOG_LEVEL;
		goto log_exit;
	}

	tmp = mblue_malloc(MAX_LOG_CACHE_LEN);
	if (!tmp) {
		rc = MBLUE_ERR_NOMEM;
		goto log_exit;
	}

	log_len = 0;
	p = __log_get_file_name_pointer(source, 
			strlen((char const*)source));
	if (p) {
		log_len = snprintf(tmp, 
			MAX_LOG_CACHE_LEN - 1, "%s[%d] ", p, level);
	}
	va_start(args, fmt);
	log_len = vsnprintf(tmp + log_len, 
		MAX_LOG_CACHE_LEN - log_len, fmt, args);
	va_end(args);

	struct mblue_byte_bundler bundler = {strlen(tmp), (uint8_t *)tmp};
	dl_12 dl = {
		level,
		{
			{.encode = __mblue_pb_write_bytes},
			(void *)&bundler
		}
	};

	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("log_service_write", PB_12, &dl);
	mblue_free(tmp);
	if (payload) {
		rc = mblue_remote_call_async(
		                  MBLUE_LOG_SERVICE, MBLUE_LOG_SERVICE_LOG, payload, log_notify, NULL);
	}

log_exit:
	return rc;
}

mblue_errcode log_service_hex(const char *prefix, const char *buffer, size_t len)
{
	size_t i;
	char *p, *q;
        mblue_errcode rc;
	void *payload;
	
	char *data_array = (char *)mblue_malloc(len * 3 + 8);
	_ASSERT(data_array);
	p = data_array;
	
	for(i = 0; i < len; i++) {
		q = p + strlen(p);
		unsigned char c = buffer[i]; 
		if( c < 16) {
			snprintf(q, 3, "0%x", c);
		} else {
			snprintf(q, 3, "%x", c);
		}
	}

	uint8_t *temp = mblue_malloc(strlen(prefix) + strlen(data_array) + 16);
	_ASSERT(temp);
	sprintf((char *)temp, "%s:%s\n", prefix, p);

	mblue_free(data_array);
	data_array = NULL;

	struct mblue_byte_bundler bundler = {strlen((char const*)temp), temp};
	dl_12 dl = {
		0,
		{
			{.encode = __mblue_pb_write_bytes},
			(void *)&bundler
		}
	};

	rc = MBLUE_ERR_PB_ENCODE;
	payload = mblue_serialize_rpc("log_service_write", PB_12, &dl);
	mblue_free(temp);
	temp = NULL;
	if (payload) {
		rc = mblue_remote_call_async(
		                  MBLUE_LOG_SERVICE, MBLUE_LOG_SERVICE_HEX, payload, log_notify, NULL);
	}
	return rc;
}



mblue_errcode led_action(int action)
{
	mblue_errcode rc;
	dl_1 dl = {action};
	void *payload, *return_data;

	rc = MBLUE_ERR_PB_ENCODE;
	return_data = NULL;
	payload = mblue_serialize_rpc("led_action", PB_1, &dl);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_LED_SERVICE, MBLUE_LED_SERVICE_ACTION, payload);
		_ASSERT(return_data);
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode iot_start(void)
{
	mblue_errcode rc;
	void *payload, *return_data;

	rc = MBLUE_ERR_PB_ENCODE;
	return_data = NULL;
	payload = mblue_serialize_rpc("iot_start", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_IOT_SERVICE, MBLUE_IOT_SERVICE_START, payload);
		_ASSERT(return_data);
		_ASSERT(INTEGER_P(return_data));
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}

mblue_errcode iot_stop(void)
{
	mblue_errcode rc;
	void *payload, *return_data;

	rc = MBLUE_ERR_PB_ENCODE;
	return_data = NULL;
	payload = mblue_serialize_rpc("iot_stop", PB_0, NULL);
	if (payload) {
		return_data = mblue_remote_call(
			MBLUE_IOT_SERVICE, MBLUE_IOT_SERVICE_STOP, payload);
		_ASSERT(return_data);
		_ASSERT(INTEGER_P(return_data));
		rc = (mblue_errcode)GET_INTEGER(return_data);
	}
        return rc;
}
