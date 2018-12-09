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

/* utility function to parse entered timestamp
 * format yyyy-mm-dd hh:mm:ss
 * */
int8_t parsetimestamp(char *buf, tm_t &tm);

/* note that the adjustment functions needs this RTClock
 * if you rename variable rt, update this so that the
 * extern refers to valid RTClock*/
extern RTClock rt;

#endif /* RTADJUST_H_ */

