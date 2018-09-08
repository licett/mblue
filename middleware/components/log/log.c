/*
 * =====================================================================================
 *
 *       Filename:  log.c
 *
 *    Description:  log interface
 *
 *        Version:  1.0
 *        Created:  2017/3/12 19:46:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdarg.h>
#include "stm32f4xx.h"
#include "log.h"
#include "SEGGER_RTT.h"
#include "mb_persistence.h"
#include "mblue_global_rpc_method.h"
#include "mblue_clock.h"
#include "mblue_heap.h"
#include "mblue_assert.h"

#ifndef LOG
#define LOG 1 // 0:swo 1:rtt
#endif

struct mb_logger *lg = NULL;

static int __itm_print(char *buffer)
{
	char *p, count;

	p = buffer;
	count = 0;
	while(*p) {
		ITM_SendChar(*p++);
		count++;
	}
	return count == 0 ? -1 : count;
}
static int __rtt_print(char *buffer)
{
	SEGGER_RTT_WriteString(0, buffer);
	return strlen(buffer);
}

static int (*mblue_log_print[])(char *buffer) = {
	__itm_print,
	__rtt_print,
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  log_init
 *  Description:  init log manager
 * =====================================================================================
 */
static int log_init(struct  log_manager *lm)
{
	lm->log_level = LOG_DEBUG;
        lm->log_type = LOG_TYPE_SWO;
	lm->set_stream(lm, mblue_log_print[LOG]);

	lg = get_log_instance();
	lg->init(lg);
	return 0;
}		/* -----  end of function log_init  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  log_write
 *  Description:  write log to storage
 * =====================================================================================
 */
// static int log_write(struct log_manager *lm, uint32_t level, char *fmt, ...)
// {
// 	va_list args;
// 	int i;

// 	i = 0;
// 	if (level <= lm->log_level) {
// 		va_start(args, fmt);
// 		i = vsnprintf((char *)lm->cache, MAX_LOG_CACHE_LEN, fmt, args);
// 		//_ASSERT(i <= MAX_LOG_CACHE_LEN);
// 		va_end(args);

// 		if (lm->stream_write) {
// 			(lm->stream_write)(lm->cache);
// 		}
// 	}
// 	return i;
// }		/* -----  end of function summary_write  ----- */


static int _write_to_flash(uint8_t *data, uint32_t len)
{
	return lg->write(lg, data, len);
}

static void _write_to_service(uint8_t *data, uint32_t len)
{
	modem_tcp_send(1, data, len);
}

/**
 *  format current time
 * @param buffer [description]
 */
static void _prinf_timer(char *buffer)
{
    struct mblue_clock *_clock = mblue_clock_get_instance();
    if (_clock) {

        uint32_t time_tamp = _clock->get_utc(_clock);

        struct mblue_clock_entity *_cur_time = _clock->get_calendar(_clock);
        _ASSERT(_cur_time);
        sprintf(buffer, "%d-%d-%d %d:%d:%d", _cur_time->year,
                _cur_time->month,
                _cur_time->day,
                _cur_time->hour,
                _cur_time->minute,
                _cur_time->second);
    }
}

static void _write_to_swo(struct log_manager *lm, uint8_t *data, uint32_t len){
	if (lm->stream_write) {
		char timer[32] = {0};
		_prinf_timer(timer);
		char *temp = mblue_malloc(strlen(timer) + len + 8);
		sprintf(temp, "%s  %s", timer, data);
		(lm->stream_write)(temp);
        mblue_free(temp);
        temp = NULL;
	}
}

static int log_write(struct log_manager *lm, uint32_t level, char *buffer, uint32_t len)
{
	if(level <= lm->log_level){

			if (lm->log_type & LOG_TYPE_SWO) {
				_write_to_swo(lm, (uint8_t *)buffer, len);
			}
			
			if (lm->log_type & LOG_TYPE_FLASH) {
				_write_to_flash((uint8_t *)buffer, len);
			}
			
			if (lm->log_type & LOG_TYPE_SERVICE) {
				_write_to_service((uint8_t *)buffer, len);
			}
	}

	return 0;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  log_set_stream
 *  Description:  set stream handler
 * =====================================================================================
 */
int log_set_stream(struct log_manager *lm, int (*flush)(char *buffer))
{
	if (flush) {
		lm->stream_write = flush;
		return 0;
	}
	return -1;
}		/* -----  end of function log_set_stream  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  log_uninit
 *  Description:  uninit log manager
 * =====================================================================================
 */
static int log_uninit(struct log_manager *lm)
{
	return 0;
}		/* -----  end of function log_uninit  ----- */

static void log_set_level(struct log_manager *lm, uint32_t level)
{
	if (level < LOG_LEVEL_MAX) {
		lm->log_level = level;
	}
}

static void log_set_type(struct log_manager *lm, uint32_t type){
	if(type < LOG_TYPE_MAX){
		lm->log_type = type;
	}
}

static struct log_manager log_manage =
{
	.init = log_init,
	.write = log_write,
	.uninit = log_uninit,
	.set_stream = log_set_stream,
	.set_level = log_set_level,
	.set_type = log_set_type
};

static struct log_manager *lm = NULL;
struct log_manager *logger_get_instance()
{
	if (!lm) {
		lm = &log_manage;
		lm->init(lm);
	}
	return lm;
}
