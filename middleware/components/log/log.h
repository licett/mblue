/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2017/3/12 19:46:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  LOG_INC
#define  LOG_INC
#include "mblue_stddefine.h"

#define	MAX_LOG_CACHE_LEN		128			/*  */
#define LOG_STREAM			STREAM_STD 

enum log_stream {
	STREAM_SWO,
	STREAM_RTT,
	STREAM_STD,
};

enum log_type {
	LOG_TYPE_PRINT = 0x01,
	LOG_TYPE_STORAGE = 0x02,
	LOG_TYPE_SERVER = 0x04,
	LOG_TYPE_MAX = 0x07
};

enum log_level {
	LOG_DISABLE,
	LOG_ERROR,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
	LOG_LEVEL_MAX
};

struct log_manager
{
	int (*stream_write)(char *buffer);

	int (* init)(struct log_manager *lm);
	// int (* write)(struct log_manager *lm, uint32_t level, char *fmt, ...);
	int (* write)(struct log_manager *lm, uint32_t level, char *buffer, uint32_t len);
	int (* uninit)(struct  log_manager *lm);
	int (* set_stream)(struct  log_manager *lm, int (*flush)(char *buffer));
	void (* set_level)(struct log_manager *lm, uint32_t level);
	void (* set_type)(struct log_manager *lm, uint32_t type);

	
	char cache[MAX_LOG_CACHE_LEN];
	uint32_t log_level;
	uint32_t log_type;
};
struct log_manager * logger_get_instance();
#endif   /* ----- #ifndef  LOG_INC----- */
