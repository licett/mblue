/*
 * =====================================================================================
 *
 *       Filename:  custom_clock.h
 *
 *    Description:  custom port of clock module
 *
 *        Version:  1.0
 *        Created:  2017/8/8 11:15:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  gzzhu (), gzzhu@leqibike.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef  custom_clock_INC
#define  custom_clock_INC

#include <time.h>
#define	GAP_SECONDS_FROM_1970_TO_2000		(946656000U)			/*  */
#define YEAR_TO_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)

static uint32_t passedDaysPerMonth[13] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static uint8_t leap_year (uint32_t y) {
	if (((!(y % 4)) && (y % 100) ) || (!(y % 400)))
		return 1;
	else
		return 0;
}

static void rtc_init(void)
{
}


static int convert_utc_to_calendar(uint64_t seconds,
                                   uint16_t *cal_year, uint8_t *cal_month, uint8_t *cal_day,
                                   uint8_t *cal_hour, uint8_t *cal_minute, uint8_t *cal_second,
                                   uint8_t *cal_time_zone)
{
	uint32_t year, month, day, hour, minute, second;

	second = seconds % 60;
	seconds /= 60;

	minute = seconds % 60;
	seconds /= 60;

	hour = seconds % 24;
	seconds /= 24;

	/* unixtime is now days since 01/01/1970 UTC
	 * Rebaseline to the Common Era */
	seconds += 719499;

	/* Roll forward looking for the year.  This could be done more efficiently
	 * but this will do.  We have to start at 1969 because the year we calculate here
	 * runs from March - so January and February 1970 will come out as 1969 here.
	 */
	for (year = 1969; seconds > YEAR_TO_DAYS(year + 1) + 30; year++);

	/* OK we have our "year", so subtract off the days accounted for by full years. */
	seconds -= YEAR_TO_DAYS(year);

	/* unixtime is now number of days we are into the year (remembering that March 1
	 * is the first day of the "year" still). */

	/* Roll forward looking for the month.  1 = March through to 12 = February. */
	for (month = 1; month < 12 && seconds > 367 * (month + 1) / 12; month++);

	/* Subtract off the days accounted for by full months */
	seconds -= 367 * month / 12;

	/* unixtime is now number of days we are into the month */
	/* Adjust the month/year so that 1 = January, and years start where we
	 * usually expect them to. */
	month += 2;
	if (month > 12) {
		month -= 12;
		year++;
	}
	day = seconds;

	*cal_year = year;
	*cal_month = month;
	*cal_day = day;
	*cal_hour = hour;
	*cal_minute = minute;
	*cal_second = second;
	return 0;
}

static int get_system_calendar(uint16_t *year, uint8_t *month, uint8_t *day,
                               uint8_t *hour, uint8_t *minute, uint8_t *second,
                               uint8_t *time_zone)
{
	int rc;
	time_t timep;   
	struct tm *p; 

	rc = -1;
	time(&timep);   
	p = localtime(&timep);
	if (p) {
		printf("%d-%d-%d %d:%d:%d\n", (1900 + p->tm_year), ( 1 + p->tm_mon), p->tm_mday,
			(p->tm_hour + 12), p->tm_min, p->tm_sec); 
		*year = 1900 + p->tm_year;
		*month = 1 + p->tm_mon;
		*day = p->tm_mday;
		*hour = p->tm_hour + 12;
		*minute = p->tm_min;
		*second = p->tm_sec;
		rc = 0;
	}
	return rc;
}

static int set_system_calendar(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	return -1;
}

static uint64_t get_system_utc_tick(void)
{
	time_t seconds;

	seconds = time((time_t *)NULL);
	return (uint64_t)seconds;
}

static int set_system_utc_tick(uint64_t seconds)
{
	return -1;
}
#endif   /* ----- #ifndef custom_clock_INC  ----- */
