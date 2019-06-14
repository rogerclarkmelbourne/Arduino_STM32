/*
 * rtadjust.h
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
#ifndef RTADJUST_H_
#define RTADJUST_H_
#include <RTClock.h>

/* adjust RTC time
 * call this from setup() so that the sketch updates the rtc when starting up
 */
void adjtime();

/*  synctime() set the RTC clock and saves the time in backup register 8 and 9
 *  as the time of the last adjustment
 *
 *  call this function with the current accurate clock time to set the rtc
 *
 *  @param time the time_t value of the current accurate clock time
 */
void synctime(time_t time_now);

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
void calibratertc(time_t time_now);


/* set the time of last adjustment in backup register 8 and 9
 * @param time this is the time_t value to be saved
 */
void setbkptime(time_t time);


/* get the time of last adjustment from backup register 8 and 9
 * @return the time_t value of the time saved
 */
time_t getbkptime();


/* save the drift duration in backup register 7
 * number of seconds for the stm32 rtc to drift 1 secs (faster)
 * from an accurate time source
 *
 * @param adj 	number of seconds for the stm32 rtc to drift 1 secs (faster)
 * 				from an accurate time source
 *
 */
void setbkpdrift(int16_t drift_dur);

/* get the drift duration from backup register 7
 * @return 	number of seconds for the stm32 rtc to drift 1 secs (faster)
 * 			from an accurate time source
 *
 */
int16_t getdrift();


/* update BKP_RTCCR register calibration value
 * see AN2604 STM32F101xx and STM32F103xx RTC calibration
 * https://www.st.com/content/ccc/resource/technical/document/application_note/ff/c1/4f/86/4e/29/42/d1/CD00167326.pdf/files/CD00167326.pdf/jcr:content/translations/en.CD00167326.pdf
 * as well as RM0008 reference manual for stm32f1x section 6 Backup registers (BKP) page 80
 * section 6.4.2 RTC clock calibration register (BKP_RTCCR) page 82
 *
 * @param cal the calibration value according to AN2604 STM32F101xx and STM32F103xx RTC calibration
 *
 */
void setrtccr(uint8_t cal);

/* retrieve the rtccr calibration value
 *
 * @return rtccr calibration value
 */
uint8_t getrtccr();

/* utility function to parse entered timestamp
 * format yyyy-mm-dd hh:mm:ss
 * */
int8_t parsetimestamp(char *buf, tm_t &tm);

/* note that the adjustment functions needs this RTClock
 * if you rename variable rt, update this so that the
 * extern refers to valid RTClock*/
extern RTClock rt;

#endif /* RTADJUST_H_ */
