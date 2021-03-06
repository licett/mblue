/*
 * =====================================================================================
 *
 *       Filename:  mblue_stdtask.h
 *
 *    Description:  mblue task template 
 *
 *        Version:  1.0
 *        Created:  2017/10/26 16:49:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef  TASK_TEMPLATE_INC
#define  TASK_TEMPLATE_INC
#include "mblue_segment.h"
#include "task_manager.h"
void task_prepare();
void task_run();
void segment_bind(struct mblue_task *task, struct mblue_segment *ms);
struct mblue_task *get_current_context();
struct mblue_task *create_mblue_task(
	struct mblue_task *task_instance,
	int priority,		char *name,	
	uint8_t *task_stack,	uint32_t stack_size, 
	uint32_t elements_in_queue,
	mblue_errcode (*customized_create)(struct mblue_task *task));
#endif   /* ----- #ifndef TASK_TEMPLATE_INC  ----- */
