/* mblue includes. */
#include <mblue_stdtask.h>
#include "task_manager.h"
#include "mblue_task_cfg.h"
#include "mblue_stdtask.h"
#include "mblue_logger.h"

int main(void)
{
	size_t i;
	struct mblue_task *task;

	// launch preload tasks
	for (i = 0; i < sizeof(mblue_preload_tasks) / sizeof(mblue_preload_tasks[0]); i++) {
		task = &mblue_preload_tasks[i];
		create_mblue_task(
			task, 
			task->priority, 
			task->name, 
			task->task_stack, 
			task->stack_size, 
			MBLUE_TASK_MAX_MSG_IN_QUEUE, 
			task->on_create
			);
	}

	pthread_exit(NULL);
	
}
