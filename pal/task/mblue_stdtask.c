/*
 * =====================================================================================
 *
 *       Filename:  task_template.c
 *
 *    Description:  mblue task template 
 *
 *        Version:  1.0
 *        Created:  2017/4/15 10:33:30
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
#include "message_queue.h"
#include "mblue_stdtask.h"
#include "mblue_semaphore.h"
#include "mblue_heap.h"
#include "mblue_segment.h"
#include "system_bus.h"

struct mblue_message_container {
	struct mblue_segment *dst;
	struct mblue_message *msg;
};

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  segment_bind
 *  Description:  bind current segment to task
 *			1.bind current segment to task
 *			2.register current segment to task's local segment list
 *			3.launch the segment
 * =====================================================================================
 */
void segment_bind(struct mblue_task *task, struct mblue_segment *ms)
{
	struct system_bus *sb;

	sb = get_system_bus_instance();
	ms->set_context(ms, task);
	list_add_tail(&ms->task, &task->segments);
	sb->regist(sb, ms);

	ms->on_launch(ms);
}

struct mblue_task *get_current_context()
{
	uint32_t i;
	struct mblue_task *t;
	struct task_manager *tm;

	tm = get_task_manager_instance();
	for (i = 0; i < MBLUE_MAX_TASKS_IN_SYSTEM; i++) {
		t = tm->task[i];
		if (t && custom_current_task_equal(t->task_obj)) {
			return t;
		}
	}
	return NULL;
}

void task_sleep(uint32_t ms)
{
	custom_task_sleep(ms);
}

static mblue_errcode template_pend(struct mblue_task *task)
{
	struct mblue_semaphore *sem;

	sem = &task->call_sem;//&TASK_CALL_SEMAPHORE;
	return sem->pend(sem, SEMAPHORE_PEND_FOREVER);
}

static mblue_errcode template_post(struct mblue_task *task)
{
	struct mblue_semaphore *sem;

	sem = &task->call_sem;//&TASK_CALL_SEMAPHORE;
	return sem->post(sem);
}

static void template_proc(void *a0)
{
	bool ret;
	uint8_t type;
	struct mblue_segment *ms;
	struct mblue_task *task;
	struct mblue_message *msg;
	struct mblue_semaphore *msg_sem;
	struct message_queue *mq;

	task = (struct mblue_task *)a0;		
	_ASSERT(task && task->mq);
	msg_sem = &task->msg_sem;//&TASK_MSG_SEMAPHORE;
	mq = task->mq;

	if (task->on_create) {
		(task->on_create)(task);
	}
	while(1) {
		ret = msg_sem->pend(msg_sem, SEMAPHORE_PEND_FOREVER);			
		if (!ret) {
			msg = (struct mblue_message *)mq->remove(mq);
			ms = msg->dst;
			_ASSERT(msg && ms);
			_ASSERT(ms->magic == MBLUE_SEGMENT_MAGIC);

			type = GET_TYPE(msg);
			(ms->get_handler(ms, type))(ms, msg);
		}
	}
}

static mblue_errcode template_msg_post(struct mblue_task *task, struct mblue_message *msg)
{
	struct message_queue *mq;
	struct mblue_semaphore *sem;

	mq = task->mq;
	sem = &task->msg_sem;//&TASK_MSG_SEMAPHORE;

	if(!(mq->add(mq, msg))) {
		sem->post(sem);
	}
	return MBLUE_OK;
}

static struct mblue_task *task_start(
	struct mblue_task *task_instance,
	int priority,		char *name,	
	uint8_t *task_stack,	uint32_t stack_size, 
	uint32_t elements_in_queue,
	task_proc	tproc, 
	mblue_errcode (*nproc)(struct mblue_task *task, struct mblue_message *msg),
	mblue_errcode (*on_create)(struct mblue_task *task),
	mblue_errcode (*tpend)(struct mblue_task *task),
	mblue_errcode (*tpost)(struct mblue_task *task))
{
	struct mblue_task *task;
	struct mblue_task *t;
	struct task_manager *tm;
	bool task_in_heap;

	_ASSERT(name);
	_ASSERT((stack_size > 0) && (stack_size <= 8192));
	_ASSERT((elements_in_queue > 0) && (elements_in_queue <= 1024));

	task_in_heap = false;
	if (task_instance == NULL) {
		task_instance	= (struct mblue_task *)
					mblue_malloc(sizeof(struct mblue_task));
		if(!(task_instance)) {
			goto fail_task;
		}
		task_in_heap = true;
	}
	task			= task_instance;
	task->task_stack	= task_stack;
	task->priority		= priority;
	task->task_id		= priority;
	task->name		= name;
	task->stack_size	= stack_size;
	task->on_create		= on_create;
	task->tproc		= tproc;
	task->nproc		= nproc;
	task->tpend		= tpend;
	task->tpost		= tpost;
	task->mq		= mblue_mq_alloc(elements_in_queue);
	if(!(task->mq)) {
		goto fail_mq;
	}
	mblue_semaphore_construct(&task->call_sem);
	mblue_semaphore_construct(&task->msg_sem);
	INIT_LIST_HEAD(&task->segments);

	tm = get_task_manager_instance();
	t = tm->create_task(tm, task);
	if (t) {
		return t;
	}
	
	_ASSERT(FALSE);
	mblue_mq_release(task->mq);
fail_mq:
	if (task_in_heap) {
		mblue_free(task);
	}
fail_task:
	return NULL;
}

struct mblue_task *create_mblue_task(
	struct mblue_task *task_instance,
	int priority,		char *name,	
	uint8_t *task_stack,	uint32_t stack_size, 
	uint32_t elements_in_queue,
	mblue_errcode (*customized_create)(struct mblue_task *task))
{
	return task_start(
		task_instance,
		priority, name, task_stack, stack_size, elements_in_queue,
		template_proc, 
		template_msg_post, 
		customized_create, 
		template_pend, 
		template_post);
}
