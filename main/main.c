/* mblue includes. */
#include <mblue_stdtask.h>
#include "mblue_stdtask.h"
#include "mblue_logger.h"

static int system_preinit(void)
{
	// preinit such as clock/hardware init 
}

int main(void)
{
	system_preinit();
	task_prepare();

	task_run();
}
