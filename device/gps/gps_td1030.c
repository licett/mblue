/*
 * =====================================================================================
 *
 *       Filename:  gps_td1030.c
 *
 *    Description:  operation of gps td1030
 *
 *        Version:  1.0
 *        Created:  12/28/2017 12:52:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include "device_gps.h"
#include "mblue_message.h"
#include "mblue_assert.h"
#include "mblue_logger.h"
#include "i2c_service.h"
#include "board_cdma_locator.h"
#include "mblue_global_rpc_method.h"

#define	MAX_GPS_DATA_CACHE_LEN			512			/*  */
#define	DATA_LENGTH_REG				0x80			/*  */
#define	DATA_REG				0x82			/*  */

const uint16_t err_token = I2C_NULL_TOKEN;
static uint8_t gps_data_cache[MAX_GPS_DATA_CACHE_LEN];
static const uint8_t gga_config[] = { 
	0x23, 0x3E, 0x03, 0x51, 0x04, 0x00, 0x04, 0x04, 0x01, 0x01, 0x62, 0x86
}; 
/*static const uint8_t rmc_config[] = { 
	0x23, 0x3E, 0x03, 0x51, 0x04, 0x00, 0x03, 0x04, 0x01, 0x01, 0x61, 0x82
}; */

static void __td1030_reset(struct gps *g)
{
	// for reset use
	i2c_read(g->i2c_index, g->address, DATA_REG, gps_data_cache, sizeof(gps_data_cache));
}

static mblue_errcode td1030_poweron(struct gps *g) 
{
	gps_poweron();
	__td1030_reset(g);
	return MBLUE_OK;
}

static mblue_errcode td1030_init(struct gps *g)
{
	return i2c_write(g->i2c_index, g->address, 0, gga_config, sizeof(gga_config));
}

static void *td1030_pull(struct gps *g)
{
	void *p;
	int num;
	int16_t ilen, len;
	
	ilen = 0;
	len = 0;
	p = NULL;
	num = i2c_read(g->i2c_index, g->address, DATA_LENGTH_REG, &ilen, 2);
	len = (ilen << 8) | (ilen >> 8);
	if (num == 2 && len > 0) { 
		// len > 0 is a condition that good enough to filter invalid marker such as 0xFFFF, 0xE1E1 and 0xFFE1
		if (len < MAX_GPS_DATA_CACHE_LEN) {
			memset(gps_data_cache, 0, sizeof(gps_data_cache));
			num = i2c_read(g->i2c_index, g->address, DATA_REG, gps_data_cache, len);
			_ASSERT(memcmp(gps_data_cache, &err_token, 2));
			if (num) {
				p =  gps_data_cache;
			}
		}
	}

	return p;
}

static mblue_errcode td1030_poweroff(struct gps *g)
{
	gps_poweroff();
	return MBLUE_OK;	
}

static mblue_errcode td1030_sleep(struct gps *g)
{
	return td1030_poweroff(g);	
}

static mblue_errcode td1030_wakeup(struct gps *g)
{
	return td1030_poweron(g);	
}

static struct gps_ops op = {
	.poweron = td1030_poweron,
	.init = td1030_init,
	.pull = td1030_pull,
	.sleep = td1030_sleep,
	.wake_up = td1030_wakeup,
	.poweroff = td1030_poweroff,
};

static struct gps _td1030 = {
	.i2c_index = 0,
	.address = 0x60,
	.ops = &op,
};
static struct gps *td1030 = NULL;

struct gps *gps_get_instance(void) {
	if (td1030 == NULL) {
		td1030 = &_td1030;
		td1030->ops->poweron(td1030);
	}
	return td1030;
}
