/*
 * =====================================================================================
 *
 *       Filename:  mblue_state_machine.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/23/2018 04:11:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  MBLUE_STATE_MACHINE_INC
#define  MBLUE_STATE_MACHINE_INC
#include "state_machine.h"

#define	DUMMY_STATE_ELEMENT					/*  */
struct mblue_state_maching;
typedef mblue_errcode (*state_err_process_t)(void *state_data, mblue_errcode input);
typedef mblue_errcode (*state_action_process_t)(struct event *e);

struct state_error_info {
	uint8_t error_count;
	uint8_t sleep_count;
	uint8_t reserved_error;
	uint8_t pad;
	void *data;
};
struct state_info {
	uint32_t state_value;
	struct state_error_info state_error;
};
struct state_actions {
	state_err_process_t err;
	state_action_process_t act;
};
//TODO add char data[0] to struct state
struct state_element {
	struct state state;
	struct state_info info;
	struct state_actions action;
};

struct mblue_state_maching {
	struct state_machine sm; 
	struct state_element *elements; 

	mblue_errcode (*init)(struct mblue_state_maching *, 
		struct state *start, struct state *end);
	void (*run_step)(struct mblue_state_maching *, struct event *);
};
struct mblue_state_maching *mblue_state_maching_alloc(
	struct state *start, struct state *end);
void mblue_state_maching_release(struct mblue_state_maching *msm);
#endif   /* ----- #ifndef mblue_state_machine_INC  ----- */
