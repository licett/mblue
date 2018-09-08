/*
 * =====================================================================================
 *
 *       Filename:  rpc.h
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
#ifndef  rpc_INC
#define  rpc_INC
#include "mblue_errcode.h"
struct mblue_rpc {
	struct mblue_task *main_task;

	mblue_errcode (*init)(struct mblue_rpc *rpc, struct mblue_task *task);
	mblue_errcode (*invoke)(struct mblue_rpc *rpc, struct mblue_message *msg);
};

struct mblue_rpc *rpc_facade_create_instance(struct mblue_task *task);
struct mblue_rpc *rpc_get_instance(void);
#endif   /* ----- #ifndef rpc_INC  ----- */
