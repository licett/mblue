/*
 * =====================================================================================
 *
 *       Filename:  custom_clock.h
 *
 *    Description:  custom definition of clock struct
 *
 *        Version:  1.0
 *        Created:  2017/5/16 19:09:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_timer_INC
#define  custom_timer_INC

#include <time.h>
#include <signal.h>
#include "mblue_timer.h"

/*-----------------------------------------------------------------------------
 * platform related type define 
 *-----------------------------------------------------------------------------*/
struct posix_timer {
	timer_t id;
	struct itimerspec spec;
	void (*notify)(void *);
	void *param;
};

static int platform_timer_stop(struct mblue_timer *timer)
{
	int rc;
	struct posix_timer *pt;

	rc = -1;
	pt = (struct posix_timer *)timer->_obj;
	if (pt) {
		timer->_obj = NULL;
		memset(&pt->spec, 0, sizeof(struct itimerspec));
		rc =  timer_settime(pt->id, 0, &pt->spec, NULL);
		mblue_free(pt);
	}

	return rc;
}

static int platform_timer_start(struct mblue_timer *timer)
{
	int rc;
	struct posix_timer *pt;

	rc = -1;
	pt = (struct posix_timer *)timer->_obj;
	if (pt) {
		rc = timer_settime(pt->id, 0, &pt->spec, NULL);
	}
	return rc;
}

static int platform_timer_alive(struct mblue_timer *timer)
{
	int alive, rc;
	struct posix_timer *pt;
	struct itimerspec current;

	alive = 0;
	pt = (struct posix_timer *)timer->_obj;
	if (!pt) {
		goto alive_exit;
	}
	
	memset(&current, 0, sizeof(current));
	rc = timer_gettime(pt->id, &current);	
	if (rc) {
		goto alive_exit;
	}
	if (current.it_value.tv_sec || current.it_value.tv_nsec) {
		alive = 1;
	}

alive_exit:
	return alive;
}

static int platform_timer_init(struct mblue_timer *timer, uint32_t period, 
	bool auto_reload, void (*notify)(void *), void *data)
{
	int rc;
	struct sigevent sigev;
	timer_t timerid;
	struct posix_timer *pt;
	struct itimerspec itval, oitval;

	rc = -1;
	timerid = 0;
	memset(&sigev, 0, sizeof(sigev));
	sigev.sigev_notify          = SIGEV_SIGNAL;
	sigev.sigev_signo           = SIGUSR1;
	sigev.sigev_value.sival_ptr = timer;

	rc = timer_create(CLOCK_MONOTONIC, &sigev, &timerid);
	if (rc) {
		goto init_exit;
	}
	pt = (struct posix_timer *)mblue_malloc(
				sizeof(struct posix_timer));
	pt->id = timerid;
	pt->spec.it_value.tv_sec = period / 1000;
	pt->spec.it_value.tv_nsec = (period % 1000) * 1000000;
	if (auto_reload) {
		pt->spec.it_interval.tv_sec = pt->spec.it_value.tv_sec ;
		pt->spec.it_interval.tv_nsec = pt->spec.it_value.tv_nsec;
	}
	pt->notify = notify;
	pt->param = data;
	timer->_obj = (void *)pt;

init_exit:
	return rc;
}

static int platform_timer_uninit(struct mblue_timer *timer)
{
	return platform_timer_stop(timer);
}

static void __timer_notify(int sig, siginfo_t *si, void *uc)
{
	struct mblue_timer *timer;
	struct posix_timer *pt;

	timer = si->si_value.sival_ptr;
	if (timer && timer->_obj) {
		pt = timer->_obj;
		(pt->notify)(pt->param);
	}
}

static int platform_timer_preinit(void)
{
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = __timer_notify;
	sigemptyset(&sa.sa_mask);

	return sigaction(SIGUSR1, &sa, NULL);
}
#endif   /* ----- #ifndef custom_timer_INC  ----- */
