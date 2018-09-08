/*
 * =====================================================================================
 *
 *       Filename:  rpc.c
 *
 *    Description:  external facade of remote call
 *
 *        Version:  1.0
 *        Created:  2017/4/18 16:03:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "mblue_stddefine.h"
#include "task_manager.h"
#include "rpc.h"
#include "mblue_semaphore.h"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  rpc_init
 *  Description:  inject the system bus task
 *  @param rpc:rpc facade obj
 *  @param task:system bus task obj
 *  @return 0:success
 *		-1:failed
 * =====================================================================================
 */
static mblue_errcode rpc_init(struct mblue_rpc *rpc, struct mblue_task *task)
{
	if (task) {
		rpc->main_task = task;
		return MBLUE_OK;
	}
	return MBLUE_ERR_SYSBUS;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  rpc_signal
 *  Description:  send a message to systembus
 *  @param rpc:  rpc facade obj
 *  @param msg:  message obj that would be sent to main task
 *  @return 0:  success
 *		-1:  failed
 * =====================================================================================
 */
static mblue_errcode rpc_invoke(struct mblue_rpc *rpc, struct mblue_message *msg)
{
	if (!rpc->main_task) {
		return MBLUE_ERR_SYSBUS;
	}

	struct mblue_task *system_bus, *src;
	system_bus = rpc->main_task;
	if ((system_bus->nproc)(system_bus, msg)) {
		return MBLUE_ERR_SYSBUS;
	}
	if (GET_TYPE(msg) == SYNC_CALL) {
		src = (struct mblue_task *)msg->src;
		if (!src) {
			return MBLUE_ERR_SYSBUS;
		}
                return (src->tpend)(src);
	}
	return MBLUE_OK;
}

static struct mblue_rpc *rpc = NULL;
static struct mblue_rpc rpc_facade = {
	.init = rpc_init,
	.invoke = rpc_invoke,
};

struct mblue_rpc *rpc_facade_create_instance(struct mblue_task *task)
{
	if (!rpc) {
		rpc = &rpc_facade;
		rpc->init(rpc, task);
		return rpc;
	} else {
		return NULL;
	}
}

struct mblue_rpc *rpc_get_instance(void)
{
	return rpc;
}
