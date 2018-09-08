/*
 * =====================================================================================
 *
 *       Filename:  mblue_state_machine.c
 *
 *    Description:  mblue state machine
 *
 *        Version:  1.0
 *        Created:  01/23/2018 03:59:35 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (embedded), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mblue_stddefine.h"
#include "mblue_assert.h"
#include "mblue_heap.h"
#include "mblue_state_machine.h"

static mblue_errcode sm_init(struct mblue_state_maching *msm,
		struct state *start, struct state *end)
{
	stateM_init(&msm->sm, start, end);
	return MBLUE_OK;
}

static void sm_run(struct mblue_state_maching *msm, struct event *input)
{
	int rc;
	rc = stateM_handleEvent(&msm->sm, input);
	_ASSERT(rc != stateM_noStateChange);
}

struct mblue_state_maching *mblue_state_maching_alloc(
	struct state *start, struct state *end)
{
	struct mblue_state_maching *msm;

	_ASSERT(start);
	_ASSERT(end);
	msm = mblue_malloc(sizeof(struct mblue_state_maching));
	if (msm) {
		msm->init = sm_init;
		msm->run_step = sm_run;
		(msm->init)(msm, start, end);
	}
	return msm;
}

void mblue_state_maching_release(struct mblue_state_maching *msm)
{
	mblue_free(msm);
}
