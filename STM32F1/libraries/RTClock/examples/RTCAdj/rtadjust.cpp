/*
 * RTCAdjust.cpp
 *
 *  Created on: Dec 10, 2018
 *
 *  @license MIT use at your own risk
 *
 *  Copyright 2018 andrew goh
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 */

#include <Arduino.h>
#include <RTClock.h>
#include "rtadjust.h"

/* adjust RTC time
 * call this from setup() so that the sketch updates the rtc when starting up
 */
void adjtime() {
	int adj;
	time_t now = rt.getTime();
	int last = getbkptime();
	int dur = now - last;

	/* adjust time no more than once in 24 hours
	 * multiple adjustments could result in cumulative errors
	 * comment this if you prefer to adjust every time
	 */
	if(now - last < 24 * 60 * 60) return;

	int driftdur = getdrift();
	if(driftdur != 0)
		adj = dur / driftdur ; // number of sec to subtract
	else
		adj = 0;
	now -= adj; // subtracting as the clock runs fast, change this if you are 'speeding up' the rtc
	rt.setTime(now);
	setbkptime(now);
}

/*  synctime() set the RTC clock and saves the time in backup register 8 and 9
 *  as the time of the last adjustment
 *
 *  call this function with the current accurate clock time to set the rtc
 *
 *  @param time the time_t value of the current accurate clock time
 */
void synctime(time_t time_now) {
	rt.setTime(time_now);
	setbkptime(time_now);
}


/*  this function calibrate the rtc by computing the drift_duration
 *
 *  call this function with the current accurate clock time to calibrate the rtc
 *
 *  if the cumulative delay between current time and the last time when synctime()
 *  is called is lower than 100, a warning would be displayed that the drift
 *  granulity is low and may result in inaccuracy of the rtc adjustments
 *
 *  @param time the time_t value of the current accurate clock time
 */
void calibratertc(time_t time_now) {
	int last = getbkptime();
	int now = rt.getTime();
	int elapsed = time_now - last;
	Serial.print(F("elapsed (s):"));
	Serial.println(elapsed);
	int drift = now - time_now ;
	Serial.print(F("drift (s):"));
	Serial.println(drift);
	if (drift < 100) {
		Serial.print(F("warn: drift granuity is low, 1 sec difference results in "));
		float d = 100.0 / (drift + 1.0);
		Serial.print(d);
		Serial.println(F(" % error"));
	}
	int16_t drift_dur = elapsed / drift;
    Serial.print(F("drift duration, number of seconds for the stm32 rtc to drift 1 secs (faster):"));
    Serial.println(drift_dur);
	setbkpdrift(drift_dur);
}

/* set the time of last adjustment in backup register 8 and 9
 * @param time this is the time_t value to be saved
 */
void setbkptime(time_t time) {
	bkp_enable_writes();
	bkp_write(8, time & 0xffff);
	bkp_write(9, time >> 16);
	bkp_disable_writes();
}

/* get the time of last adjustment from backup register 8 and 9
 * @return the time_t value of the time saved
 */
time_t getbkptime() {
	time_t time;
	time = bkp_read(8);
	time |= bkp_read(9) << 16;
	return time;
}

/* save the drift duration in backup register 7
 * number of seconds for the stm32 rtc to drift 1 secs (faster)
 * from an accurate time source
 *
 * @param adj 	number of seconds for the stm32 rtc to drift 1 secs (faster)
 * 				from an accurate time source
 *
 */
void setbkpdrift(int16_t drift_dur) {
	bkp_enable_writes();
	bkp_write(7, drift_dur);
	bkp_disable_writes();
}


/* get the drift duration from backup register 7
 * @return 	number of seconds for the stm32 rtc to drift 1 secs (faster)
 * 			from an accurate time source
 *
 */
int16_t getdrift() {
	return bkp_read(7);
}


const char * delim = " -:";
/* utility function to parse entered timestamp
 * format yyyy-mm-dd hh:mm:ss
 * */
int8_t parsetimestamp(char *buf, tm_t &tm) {
	int8_t i = 0;
    char *token = strtok(buf, delim); // get first token
    // walk through tokens
    while( token != NULL ) {
    	i++;
    	int num = atoi(token);
    	//Serial.print(i);
    	//Serial.print(' ');
    	//Serial.println(num);
    	switch(i) {
    	case 1:
    		tm.year = num - 1970;
    		break;
    	case 2:
    		tm.month = num;
    		break;
    	case 3:
    		tm.day = num;
    		break;
    	case 4:
    		tm.hour = num;
    		break;
    	case 5:
    		tm.minute = num;
    		break;
    	case 6:
    		tm.second = num;
    		break;
    	default:
    		break;
    	}
		token = strtok(NULL, delim); // get next token
    }
    if (i==6) //if we have 6 tokens assume it is correct ;p
    	return 0;
    else
    	return -1;
}




