
#include "rtc.h"


uint32_t rtc_tr, rtc_dr;

//-----------------------------------------------------------------------------
uint8_t _year(uint32_t dr)    { return bcd2bin( (dr>>RTC_DR_YEAR_BIT) & RTC_DR_YEAR_MASK ); }
uint8_t _month(uint32_t dr)   { return bcd2bin( (dr>>RTC_DR_MONTH_BIT) & RTC_DR_MONTH_MASK ); }
uint8_t _day(uint32_t dr)     { return bcd2bin( (dr>>RTC_DR_DAY_BIT) & RTC_DR_DAY_MASK ); }
uint8_t _weekday(uint32_t dr) { return bcd2bin( (dr>>RTC_DR_WEEKDAY_BIT) & RTC_DR_WEEKDAY_MASK ); }
uint8_t _pm(uint32_t tr)      { return ( (tr>>RTC_TR_PM_BIT) & RTC_TR_PM_MASK ); }
uint8_t _hour(uint32_t tr)    { return bcd2bin( (tr>>RTC_TR_HOUR_BIT) & RTC_TR_HOUR_MASK ); }
uint8_t _minute(uint32_t tr)  { return bcd2bin( (tr>>RTC_TR_MINUTE_BIT) & RTC_TR_MINUTE_MASK ); }
uint8_t _second(uint32_t tr)  { return bcd2bin( (tr>>RTC_TR_SECOND_BIT) & RTC_TR_SECOND_MASK ); }

//-----------------------------------------------------------------------------
void getTimeStamp(void)
{
	rtc_tr = RTC->TR;
	(void)RTC->DR;
	rtc_dr = RTC->DR;
}

//-----------------------------------------------------------------------------
void _getTime(tm_t *tm)
{
	uint32 tr;
	do { // read multiple time till both readings are equal
		getTimeStamp();
		tr = rtc_tr;
		getTimeStamp();
	} while ( tr!=rtc_tr );
	tm->year    = _year(rtc_dr);
    tm->month   = _month(rtc_dr);
    tm->day     = _day(rtc_dr);
    tm->weekday = _weekday(rtc_dr);
    tm->pm      = _pm(rtc_tr);
    tm->hour    = _hour(rtc_tr);
    tm->minute  = _minute(rtc_tr);
    tm->second  = _second(rtc_tr);
}

//-----------------------------------------------------------------------------
void _setTime(tm_t *tm)
{
    if (tm->year > 99)
        tm->year = tm->year % 100;
    rtc_dr = BUILD_DATE_REGISTER(tm->year, tm->month, tm->day, tm->weekday);
    rtc_tr = BUILD_TIME_REGISTER(tm->hour, tm->minute, tm->second);
    rtc_enter_config_mode();
    RTC->TR = rtc_tr;
    RTC->DR = rtc_dr;
    rtc_exit_config_mode();
	//getTimeStamp(); // fix wrong first read
}

//-----------------------------------------------------------------------------
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k

#define LEAP_YEAR(Y)  ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0
//-----------------------------------------------------------------------------
time_t _makeTime(tm_t * tm)
{
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
	// seconds from 1970 till 1 jan 00:00:00 of the given year
	uint32_t seconds = tm->year*(SECS_PER_DAY * 365);
	for (uint16_t i = 0; i < tm->year; i++) {
		if (LEAP_YEAR(i)) {
			seconds +=  SECS_PER_DAY;   // add extra days for leap years
		}
	}

	// add days for this year, months start from 1
	for (uint16_t i = 1; i < tm->month; i++) {
		if ( (i == 2) && LEAP_YEAR(tm->year)) { 
			seconds += SECS_PER_DAY * 29;
		} else {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
		}
	}
	seconds+= (tm->day-1) * SECS_PER_DAY;
	seconds+= tm->hour * SECS_PER_HOUR;
	seconds+= tm->minute * SECS_PER_MIN;
	seconds+= tm->second;
	return (time_t)seconds; 
}
//-----------------------------------------------------------------------------
void _breakTime(time_t epoch_time, tm_t * tm)
{
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

	uint32_t time = (uint32_t)epoch_time;
	tm->second = time % 60;
	time /= 60; // now it is minutes
	tm->minute = time % 60;
	time /= 60; // now it is hours
	tm->hour = time % 24;
	time /= 24; // now it is days
	tm->weekday = ((time + 4) % 7); // Monday is day 1 // (time + 4): Sunday is day 1 

	uint8_t year = 0;
	uint32_t days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	tm->year = year; // year is offset from 1970 

	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0

	uint8_t month = 0;
	uint8_t monthLength = 0;
	for (month=0; month<12; month++)
	{
		if (month==1) { // february
			if (LEAP_YEAR(year)) {
				monthLength=29;
			} else {
				monthLength=28;
			}
		} else {
			monthLength = monthDays[month];
		}

		if (time >= monthLength) {
			time -= monthLength;
		} else {
			break;
		}
	}
	tm->month = month + 1;  // jan is month 1  
	tm->day = time + 1;     // day of month
}

#if 0
//-----------------------------------------------------------------------------
// Check (wait if necessary) to see RTC registers are synchronized.
//-----------------------------------------------------------------------------
void rtc_wait_sync()
{
	PRINTF("> rtc_wait_sync\n");
	uint32 t = millis();
	while ( !(RTC->ISR & BIT(RTC_ISR_RSF_BIT)) ) 
	{
	    if ( (millis()-t)>1500) {
			PRINTF("Sync Timeout ! ISR = %08X\n", RTC->ISR);
			break;
		}
	}
	PRINTF("< rtc_wait_sync\n");
}
#endif

//-----------------------------------------------------------------------------
// Enable an RTC alarm event. Enabling this event allows waking up from deep sleep via WFE.
//-----------------------------------------------------------------------------
void rtc_enable_alarm_event()
{
    EXTI_BASE->IMR  |= EXTI_RTC_ALARM;
	EXTI_BASE->EMR  |= EXTI_RTC_ALARM;
	EXTI_BASE->RTSR |= EXTI_RTC_ALARM;
}

//-----------------------------------------------------------------------------
// Disable the RTC alarm event.
//-----------------------------------------------------------------------------
void rtc_disable_alarm_event()
{
	EXTI_BASE->EMR  &= ~(EXTI_RTC_ALARM);
	EXTI_BASE->RTSR &= ~(EXTI_RTC_ALARM);
}

//-----------------------------------------------------------------------------
// @brief Enable an RTC Wakeup event. 
//-----------------------------------------------------------------------------
void rtc_enable_wakeup_event()
{
    EXTI_BASE->IMR  |= EXTI_RTC_WAKEUP;
	EXTI_BASE->EMR  |= EXTI_RTC_WAKEUP;
	EXTI_BASE->RTSR |= EXTI_RTC_WAKEUP;
}

//-----------------------------------------------------------------------------
// Enter configuration mode.
//-----------------------------------------------------------------------------
void rtc_enter_config_mode()
{
	//PRINTF("> rtc_enter_config_mode\n");
	noInterrupts();
	// Unlock Write Protect
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	//PRINTF("RTC->ISR(1) = %08X\n", RTC->ISR);
	//*bb_perip(&RTC->ISR, RTC_ISR_INIT_BIT) = 1;
	RTC->ISR = 0x1FFFF;
	//PRINTF("RTC->ISR(2) = %08X\n", RTC->ISR);
	uint32 t = 0;
	while (!(RTC->ISR & RTC_ISR_INITF))
	{
	    if (++t > 10000000) {
			//PRINTF("RTC->ISR.INITF Timeout ! ISR = %08X\n", RTC->ISR);
			break;;
		}
	}
	//PRINTF("< rtc_enter_config_mode\n");
}


voidFuncPtr handlerAlarmA;
voidFuncPtr handlerAlarmB;
voidFuncPtr handlerPeriodicWakeup;

//-----------------------------------------------------------------------------
void __irq_rtc(void)
{
	rtc_enter_config_mode();
	*bb_perip(&RTC->ISR, RTC_ISR_WUTF_BIT) = 0;
	rtc_exit_config_mode();
	EXTI_BASE->PR = EXTI_RTC_WAKEUP;
	if (handlerPeriodicWakeup != NULL) {
		handlerPeriodicWakeup();
	}
}

//-----------------------------------------------------------------------------
void __irq_rtcalarm(void)
{
	bool isAlarmA = false;
	bool isAlarmB = false;
	rtc_enter_config_mode();
	if (RTC->ISR & BIT(RTC_ISR_ALRAF_BIT)) {
		isAlarmA = true;
		*bb_perip(&RTC->ISR, RTC_ISR_ALRAF_BIT) = 0;
	}
	if (RTC->ISR & BIT(RTC_ISR_ALRBF_BIT)) {
		isAlarmB = true;
		*bb_perip(&RTC->ISR, RTC_ISR_ALRBF_BIT) = 0;
	}
	rtc_exit_config_mode();
	EXTI_BASE->PR = EXTI_RTC_ALARM;
	if (isAlarmA && handlerAlarmA != NULL) {
		handlerAlarmA();
	}
	if (isAlarmB && handlerAlarmB != NULL) {
		handlerAlarmB();
	}
}

