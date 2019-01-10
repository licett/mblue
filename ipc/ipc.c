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
#include "mblue_list.h"
#include "mblue_assert.h"
#include "mblue_segment.h"
#include "task_manager.h"
#include "ipc.h"
#include "mblue_semaphore.h"
#include "mblue_logger.h"

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
	mblue_errcode rc;
	struct mblue_segment *ms, *tmp;
	struct list_head *node, seg;
	struct mblue_task *dst, *src;

	rc = MBLUE_OK;

	ms = msg->dst;
	if (!ms) {
		dst = ipc->main_task;

		list_for_each(node, &(dst->segments)) {
			tmp = list_entry(node, struct mblue_segment, task);
			if (!tmp 
#ifdef DEBUG
				|| (tmp->magic != MBLUE_SEGMENT_MAGIC)
#endif
				) {
				LOGGER(LOG_ERROR," found err in segment list\n");
				_ASSERT(FALSE);
				rc = MBLUE_ERR_CORRUPT_MEM;
				goto ipc_exit;

			} else if (tmp->major == MBLUE_BUS) {
				msg->dst = tmp;
				break;
			}
		}
		_ASSERT(msg->dst);
	} else {
		dst = ms->context;
	}

	if ((dst->nproc)(dst, msg)) {
		rc =  MBLUE_ERR_SYSBUS;
		goto ipc_exit;
	}

	if (GET_TYPE(msg) == SYNC_CALL) {
		src = (struct mblue_task *)msg->src;
		if (!src) {
			rc = MBLUE_ERR_SYSBUS;
			goto ipc_exit;
		}
                rc = (src->tpend)(src);
	}

ipc_exit:
	return rc;
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
