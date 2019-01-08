/*
 * =====================================================================================
 *
 *       Filename:  ipc.h
 *
 *    Description:  external facade of remote call
 *
 *        Version:  1.0
 *        Created:  2017/4/18 16:04:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  IPC_INC
#define  IPC_INC
#include "mblue_errcode.h"
struct mblue_ipc {
	struct mblue_task *main_task;

	mblue_errcode (*init)(struct mblue_ipc *ipc, struct mblue_task *task);
	mblue_errcode (*invoke)(struct mblue_ipc *ipc, struct mblue_message *msg);
};

struct mblue_ipc *ipc_facade_create_instance(struct mblue_task *task);
struct mblue_ipc *ipc_get_instance(void);
#endif   /* ----- #ifndef ipc_INC  ----- */
