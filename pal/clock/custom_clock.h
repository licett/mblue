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

#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"

#define	GAP_SECONDS_FROM_1970_TO_2000		(946656000U)			/*  */

#define RTC_CLOCK_SOURCE_LSE          /* LSE used as RTC source clock */

/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	} 
}


/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static void rtc_init(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	__IO uint32_t uwAsynchPrediv = 0;
	__IO uint32_t uwSynchPrediv = 0;

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);

	/* Enable the LSE OSC */
	RCC_LSEConfig(RCC_LSE_ON);
	/* Wait till LSE is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	}
	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
	uwSynchPrediv = 0xFF;
	uwAsynchPrediv = 0x7F;

	/* Enable the RTC Clock */
	RCC_RTCCLKCmd(ENABLE);
	/* Wait for RTC APB registers synchronisation */
	RTC_WaitForSynchro();

	/* Configure the RTC data register and RTC prescaler */
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
	RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);

	/* 2048 tick per second */
	RTC_WakeUpCmd(DISABLE);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
	/*RTC_SetWakeUpCounter(0x1FFF);*/
	/*RTC_SetWakeUpCounter(0xFF);*/

	/* Enable the Wakeup Interrupt */
	RTC_ClearITPendingBit(RTC_IT_WUT);
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	/* Connect EXTI_Line22 to the RTC Wakeup event */
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable the RTC Wakeup Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xF;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void rtc_config(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{

	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0xA0A0) {

		// setting date info
		RTC_DateTypeDef RTC_DateStructure;
		RTC_DateStructure.RTC_Year = year - 2000;
		RTC_DateStructure.RTC_Month = month;
		RTC_DateStructure.RTC_Date = day;
		RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);

		// setting time info
		RTC_TimeTypeDef  RTC_TimeStructure;
		RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
		RTC_TimeStructure.RTC_Hours   = hour;
		RTC_TimeStructure.RTC_Minutes = minute;
		RTC_TimeStructure.RTC_Seconds = second;

		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);

		/* Indicator for the RTC configuration */
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0xA0A0);
	}
}


#define YEAR_TO_DAYS(y) ((y)*365 + (y)/4 - (y)/100 + (y)/400)

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

	RTC_TimeTypeDef rtc_time;
	RTC_GetTime(RTC_Format_BIN, &rtc_time);
	RTC_DateTypeDef rtc_date;
	RTC_GetDate(RTC_Format_BIN, &rtc_date);

	*year = rtc_date.RTC_Year + 2000;
	*month = rtc_date.RTC_Month;
	*day = rtc_date.RTC_Date;
	*hour = rtc_time.RTC_Hours;
	*minute = rtc_time.RTC_Minutes;
	*second = rtc_time.RTC_Seconds;

	return 0;
}

static int set_system_calendar(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	rtc_config(year,
	           month,
	           day,
	           hour,
	           minute,
	           second);
	return 0;
}

static uint32_t passedDaysPerMonth[13] = { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
static uint8_t leap_year (uint32_t y) {
	if (((!(y % 4)) && (y % 100) ) || (!(y % 400)))
		return 1;
	else
		return 0;
}

static uint64_t _get_unix_time()
{
	RTC_TimeTypeDef rtc_time;
	RTC_GetTime(RTC_Format_BIN, &rtc_time);
	RTC_DateTypeDef rtc_date;
	RTC_GetDate(RTC_Format_BIN, &rtc_date);

	uint64_t unix_time;
	uint32_t year = rtc_date.RTC_Year + 2000 - 1900;

	unix_time = rtc_time.RTC_Seconds;
	unix_time += rtc_time.RTC_Minutes * 60;
	unix_time += rtc_time.RTC_Hours * 3600;
	unix_time += passedDaysPerMonth[rtc_date.RTC_Month] * 86400;
	unix_time += (rtc_date.RTC_Date - 1) * 86400;
	unix_time += (year - 70) * 31536000;
	unix_time += ((year - 69) / 4) * 86400;
	unix_time -= ((year - 1) / 100) * 86400;
	unix_time += ((year + 299) / 400) * 86400;
	if (leap_year(year) && rtc_date.RTC_Month >= 3)
		unix_time += 86400;

	unix_time -= 8 * 3600; // rtc设置的时间是（北京时区，即：+8时区）

	return unix_time;
}

static uint64_t get_system_utc_tick(void)
{
	return _get_unix_time();
}

static int set_system_utc_tick(uint64_t seconds)
{
	uint16_t year;
	uint8_t month, day, hour, minute, second, time_zone;
	if (!convert_utc_to_calendar(seconds,
	                             &year,
	                             &month,
	                             &day,
	                             &hour,
	                             &minute,
	                             &second,
	                             &time_zone)) {
		return set_system_calendar(year, month, day, hour, minute, second);
	}
	return -1;
}
#endif   /* ----- #ifndef custom_clock_INC  ----- */
