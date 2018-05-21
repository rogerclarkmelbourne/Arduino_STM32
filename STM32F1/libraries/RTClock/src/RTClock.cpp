/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**

 */

#include "RTClock.h"

	RTClock::RTClock() {
		RTClock(RTCSEL_HSE, 0xf423);
	}

    RTClock::RTClock(rtc_clk_src src) {
		RTClock(src, 0);
	
	}//end RTC

	RTClock::RTClock(rtc_clk_src src, uint32 prescaler ) {
		switch (src) {
	
		case RTCSEL_LSE : {
			rtc_init(RTCSEL_LSE);//LSE should be 32768 Hz.
			if (prescaler != 0) rtc_set_prescaler_load(prescaler); //according to sheet clock/(prescaler + 1) = Hz
			else rtc_set_prescaler_load(0x7fff);
			break;
			}
		case RTCSEL_LSI : {
			rtc_init(RTCSEL_LSI);//LSI is around 40000 Hz (between 30000 and 60000).
			if (prescaler != 0) rtc_set_prescaler_load(prescaler); //according to sheet clock/(prescaler + 1) = Hz 39999Hz = 0x9C3F
			else rtc_set_prescaler_load(0x9C3F);
			break;
			}
		case RTCSEL_HSE : {
			rtc_init(RTCSEL_HSE);//HSE = 8/128MHz = 62500 Hz
			if (prescaler != 0) rtc_set_prescaler_load(prescaler); //according to sheet clock/(prescaler + 1) = Hz 0xF423 = 62499
			else rtc_set_prescaler_load(0xF423);
			break;
			}
		case RTCSEL_DEFAULT: {
			//do nothing. Have a look at the clocks to see the diff between NONE and DEFAULT
			break;
			}
		case RTCSEL_NONE: {
			//do nothing. Have a look at the clocks to see the diff between NONE and DEFAULT
			break;
			}
		
		}//end switch
	

	}

/*
	RTClock::~RTClock() {
	//to implement
	}
*/	
	
	void RTClock::setTime (tm_t & tmm) {
		time_t mktm = makeTime(tmm); // time will be make to mktm
    		setTime(mktm);
		//rtc_set_count(time_stamp);
	}
	
	void RTClock::setTime (time_t time_stamp) {
		rtc_set_count(time_stamp);
	}
	
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
	
	//-----------------------------------------------------------------------------
void RTClock::breakTime(time_t timeInput, tm_t & tmm)
{
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	uint32_t days;

	time = (uint32_t)timeInput;
	tmm.second = time % 60;
	time /= 60; // now it is minutes
	tmm.minute = time % 60;
	time /= 60; // now it is hours
	tmm.hour = time % 24;
	time /= 24; // now it is days
	tmm.weekday = ((time + 3) % 7); // Monday is day 1

	year = 0;
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	tmm.year = year; // year is offset from 1970 

	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0

	days = 0;
	month = 0;
	monthLength = 0;
	for (month=0; month<12; month++) {
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
	tmm.month = month + 1;  // jan is month 1  
	tmm.day = time + 1;     // day of month
}

//-----------------------------------------------------------------------------
time_t RTClock::makeTime(tm_t & tmm)
{
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
	int i;
	uint32_t seconds;

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds = tmm.year*(SECS_PER_DAY * 365);
	for (i = 0; i < tmm.year; i++) {
		if (LEAP_YEAR(i)) {
			seconds +=  SECS_PER_DAY;   // add extra days for leap years
		}
	}

	// add days for this year, months start from 1
	for (i = 1; i < tmm.month; i++) {
		if ( (i == 2) && LEAP_YEAR(tmm.year)) { 
			seconds += SECS_PER_DAY * 29;
		} else {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
		}
	}
	seconds+= (tmm.day-1) * SECS_PER_DAY;
	seconds+= tmm.hour * SECS_PER_HOUR;
	seconds+= tmm.minute * SECS_PER_MIN;
	seconds+= tmm.second;
	return (time_t)seconds; 
}

	time_t RTClock::getTime() {
		return rtc_get_count();
	}

    void RTClock::getTime(tm_t & tm_ptr) {
		time_t res = rtc_get_count();
		breakTime(res, tm_ptr);
	}
	
	void RTClock::createAlarm(voidFuncPtr function, time_t alarm_time_t) {
		rtc_set_alarm(alarm_time_t); //must be int... for standardization sake. 
		rtc_attach_interrupt(RTC_ALARM_SPECIFIC_INTERRUPT, function);
	}
	
	void RTClock::removeAlarm() {
		rtc_detach_interrupt(RTC_ALARM_SPECIFIC_INTERRUPT);
	}
	
	void RTClock::attachSecondsInterrupt(voidFuncPtr function) {
		rtc_attach_interrupt(RTC_SECONDS_INTERRUPT, function);
	}
	void RTClock::detachSecondsInterrupt() {
		rtc_detach_interrupt(RTC_SECONDS_INTERRUPT);
	}

        void RTClock::attachAlarmInterrupt(voidFuncPtr function, time_t alarm_time) {  // Don't need run RTClock::setAlarmTime(time_t alarm_time)
		rtc_set_alarm(alarm_time);
		rtc_attach_interrupt(RTC_ALARM_GLOBAL_INTERRUPT, function);
	}
	
	void RTClock::attachAlarmInterrupt(voidFuncPtr function) {       // Must run RTClock::setAlarmTime (time_t alarm_time or tm_t & alarm_tm) first
		rtc_attach_interrupt(RTC_ALARM_GLOBAL_INTERRUPT, function);
	}
	
	void RTClock::detachAlarmInterrupt() {
		rtc_detach_interrupt(RTC_ALARM_GLOBAL_INTERRUPT);
	}

	void RTClock::createAlarm(voidFuncPtr function, tm_t & alarm_tm) {
		time_t alarm = makeTime(alarm_tm);//convert to time_t
		createAlarm(function, alarm);	
	}

//change alarm time	

	void RTClock::setAlarmTime (tm_t  & tm_ptr) {
		time_t time = makeTime(tm_ptr);//convert to time_t
		rtc_set_alarm(time); //must be int... for standardization sake. 
	}

	void RTClock::setAlarmTime (time_t alarm_time) {
	
		rtc_set_alarm(alarm_time);
	
	}

	
