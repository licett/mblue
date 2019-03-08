/*
 * =====================================================================================
 *
 *       Filename:  global_ipc_method.c
 *
 *    Description:  declare of all global ipc method
 *    TODO design a template to register global ipc
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
#include "mblue_global_ipc_method.h"
#include "mblue_heap.h"
#include "mblue.pb.h"
#include "mblue_logger.h"
#include "ipc_helper.h"
#include "pb_schema.h"

mblue_errcode log_service_set_type(uint32_t type)
{
	mblue_errcode rc;
	void *payload, *return_data;

	return_data = NULL;
	rc = MBLUE_ERR_UNKNOWN;
	dl_1 dl = {type};
	payload = mblue_serialize_ipc("log_service_set_type", PB_1, &dl);
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
	payload = mblue_serialize_ipc("log_service_set_level", PB_1, &dl);
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

mblue_errcode log_service_write(uint32_t level, char *source, void *fmt, ...)
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
	payload = mblue_serialize_ipc("log_service_write", PB_12, &dl);
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
	payload = mblue_serialize_ipc("log_service_write", PB_12, &dl);
	mblue_free(temp);
	temp = NULL;
	if (payload) {
		rc = mblue_remote_call_async(
		                  MBLUE_LOG_SERVICE, MBLUE_LOG_SERVICE_HEX, payload, log_notify, NULL);
	}
	return rc;
}



