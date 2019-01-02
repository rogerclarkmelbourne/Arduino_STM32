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
	if(driftdur != 0) {
		adj = dur / driftdur ; // number of sec to subtract
		now -= adj; // subtracting as the clock runs fast, change this if you are 'speeding up' the rtc
		rt.setTime(now);
		setbkptime(now);
	}
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
 *  note that this function can only be run once to compute the drift_duration
 *  this is because the time of last adjustment would have been updated
 *  by adjtime() after calibration and is no longer relevant for purpose of
 *  computing drift duration
 *
 *  to run it again
 *  1) first zero out drift duration using setbkpdrift(0)
 *     or disconnect VBAT and power to clear backup memory
 *  next repeat the calibration cycle
 *  2) run synctime(time_now) with the accurate clock time
 *  3) after a day or more (longer period cumulates more drift and increase accuracy)
 *     run calibrate(time_now) with the accurate clock time
 *
 *  @param time the time_t value of the current accurate clock time
 */
void calibratertc(time_t time_now) {

	int cur_drift = getdrift();
	if(cur_drift != 0) {
		Serial.print(F("err: rtc has been calibrated prior, "));
		Serial.println(F("zero out drift duration before calibrating again"));
		return;
	}
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
	int drift_dur = elapsed / drift;
    Serial.print(F("drift duration, number of seconds for the stm32 rtc to drift 1 secs (faster):"));
    Serial.println(drift_dur);
    int rtccr_max = 1048576 / 127;
    if(drift_dur > rtccr_max) { //about less than 314 secs ~ 5 mins drift per month
 /* if the drift is better than 1048576 / 127 ~ 1:8256 the rtc hardware can do the
  * adjustment, in this case just setup BKP_RTCCR register calibration value and let the
  * RTC hardware take care of the drift adjustment. this only works if the RTC
  * runs faster than accurate time
  *
  * see AN2604 STM32F101xx and STM32F103xx RTC calibration
  * https://www.st.com/content/ccc/resource/technical/document/application_note/ff/c1/4f/86/4e/29/42/d1/CD00167326.pdf/files/CD00167326.pdf/jcr:content/translations/en.CD00167326.pdf
  */
    	uint8_t cal = drift_dur * 127 / 1048576;
    	setrtccr(cal);
    	//set the drift duration as zero - no software adjustments needed
    	setbkpdrift(0);
    } else if ( drift_dur > -32768 && drift_dur <= rtccr_max ) {
    	setbkpdrift(drift_dur & 0xffff); //bkp register is only 16 bits
    } else
    	//drift duration < -32767 ! (about slower by less than 10 secs per month)
    	//no adjustment can't store in 16 bit bkp register
    	setbkpdrift(0);

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

/* update BKP_RTCCR register calibration value
 * see AN2604 STM32F101xx and STM32F103xx RTC calibration
 * https://www.st.com/content/ccc/resource/technical/document/application_note/ff/c1/4f/86/4e/29/42/d1/CD00167326.pdf/files/CD00167326.pdf/jcr:content/translations/en.CD00167326.pdf
 * as well as RM0008 reference manual for stm32f1x section 6 Backup registers (BKP) page 80
 * section 6.4.2 RTC clock calibration register (BKP_RTCCR) page 82
 *
 * @param cal the calibration value according to AN2604 STM32F101xx and STM32F103xx RTC calibration
 *
 */
void setrtccr(uint8_t cal) {
	bkp_enable_writes();
	uint16_t val = 0;
	//calibration value is the 1st 7 bits of RTCCR
	//we zero out the other bits as it is not needed
	BKP->regs->RTCCR = cal & 0x7f;
	bkp_disable_writes();
}

/* retrieve the rtccr calibration value
 *
 * @return rtccr calibration value
 */
uint8_t getrtccr() {
	return (BKP->regs->RTCCR) & 0x7f;
}

/* utility function to parse entered timestamp
 * format yyyy-mm-dd hh:mm:ss
 * */
int8_t parsetimestamp(char *buf, tm_t &tm) {
	int8_t i = 0;
    char *token = strtok(buf, " -:"); // get first token
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
		token = strtok(NULL, " -:"); // get next token
    }
    if (i==6) //if we have 6 tokens assume it is correct ;p
    	return 0;
    else
    	return -1;
}




