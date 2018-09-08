/*
 * =====================================================================================
 *
 *       Filename:  debug_callback.c
 *
 *    Description:  syscallback
 *
 *        Version:  1.0
 *        Created:  2017/4/25 16:44:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include "_assert.h"

//const unsigned ITM_BASE_ADDRESS = 0xE0000000;
volatile uintptr_t *excPC = 0;
volatile uintptr_t *excCaller = 0;
static void _delay(unsigned num_loops)
{
	unsigned i;
	for (i=0; i<num_loops; i++)
	{
		asm ("NOP");
	}
}

static void _port_wait(volatile unsigned *port)
{
	_delay(10);
	/* Wait while fifo ready */
	while (*port == 0);
}

static void _itm_put_08(volatile unsigned *port, unsigned char data) 
{ 
	volatile unsigned char* myport = (volatile unsigned char*)port; 
	_port_wait(port); 
	*myport = data; 
}

void debug_syscallback_putch(const char data)
{
	_itm_put_08((volatile unsigned *)0xE0000000, data);
}

void debug_syscallback_abort(const char *str)
{
	_ASSERT(0);
}

void debug_syscallback_exit(int code)
{
	_ASSERT(0);
}

void debug_system_abort(void)
{
	_ASSERT(0);
}

void debug_exchook(Hwi_ExcContext *ctx)
{
	excPC = ctx->pc;     // Program counter where exception occurred
	excCaller = ctx->lr; // Link Register when exception occurred

	_ASSERT(0);
}
