/*
 * =====================================================================================
 *
 *       Filename:  device_gps.h
 *
 *    Description:  operation of gps td1030
 *
 *        Version:  1.0
 *        Created:  12/28/2017 12:53:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  device_gps_INC
#define  device_gps_INC
#include "mblue_stddefine.h"

struct gps;
struct gps_ops {
	mblue_errcode (*poweron)(struct gps *);
	mblue_errcode (*init)(struct gps *);
	void *(*pull)(struct gps *);
	mblue_errcode (*sleep)(struct gps *);
	mblue_errcode (*wake_up)(struct gps *);
	mblue_errcode (*poweroff)(struct gps *);
};

struct gps {
	uint16_t i2c_index;
	uint16_t address;
	struct gps_ops *ops;
};

struct gps *gps_get_instance(void);
#endif   /* ----- #ifndef gps_td1030_INC  ----- */
