/*
 * =====================================================================================
 *
 *       Filename:  ipc.c
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
#include "ipc.h"
#include "mblue_semaphore.h"
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ipc_init
 *  Description:  inject the system bus task
 *  @param ipc:ipc facade obj
 *  @param task:system bus task obj
 *  @return 0:success
 *		-1:failed
 * =====================================================================================
 */
static mblue_errcode ipc_init(struct mblue_ipc *ipc, struct mblue_task *task)
{
	if (task) {
		ipc->main_task = task;
		return MBLUE_OK;
	}
	return MBLUE_ERR_SYSBUS;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ipc_signal
 *  Description:  send a message to systembus
 *  @param ipc:  ipc facade obj
 *  @param msg:  message obj that would be sent to main task
 *  @return 0:  success
 *		-1:  failed
 * =====================================================================================
 */
static mblue_errcode ipc_invoke(struct mblue_ipc *ipc, struct mblue_message *msg)
{
	if (!ipc->main_task) {
		return MBLUE_ERR_SYSBUS;
	}

	struct mblue_task *system_bus, *src;
	system_bus = ipc->main_task;
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

static struct mblue_ipc *ipc = NULL;
static struct mblue_ipc ipc_facade = {
	.init = ipc_init,
	.invoke = ipc_invoke,
};

struct mblue_ipc *ipc_facade_create_instance(struct mblue_task *task)
{
	if (!ipc) {
		ipc = &ipc_facade;
		ipc->init(ipc, task);
		return ipc;
	} else {
		return NULL;
	}
}

struct mblue_ipc *ipc_get_instance(void)
{
	return ipc;
}
