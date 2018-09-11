/*
 * =====================================================================================
 *
 *       Filename:  locator.h
 *
 *    Description:  application entry of locator
 *
 *        Version:  1.0
 *        Created:  12/29/2017 07:11:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  locator_INC
#define  locator_INC
#include "mblue_macro.h"
#include "mblue_segment.h"

struct	SIMPLE_SCENARIO {
	struct mblue_segment ms_entity;
};

DECLARE_SCENARIO_INCLUDE(SIMPLE_SCENARIO);
#endif   /* ----- #ifndef locator_INC  ----- */
