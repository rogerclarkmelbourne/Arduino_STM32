# stm32duino RTC drift adjustment

This Arduino sketch is built based on Stm32duino libmaple core.
It provides an example of how one could adjust for RTC (real time clock) drifts if one is using a 32k crystal that drift significantly.

For drifts that is less than (304 secs ~ 5 minutes) per month. STM has published an appnote for a different means of slowing down the RTC clock
[AN2604 STM32F101xx and STM32F103xx RTC calibration](https://www.st.com/content/ccc/resource/technical/document/application_note/6c/a3/24/49/a5/d4/4a/db/CD00164185.pdf/files/CD00164185.pdf/jcr:content/translations/en.CD00164185.pdf). This is possibly simpler and more accurate.

## Building and running the sketch

Build and run the sketch based on STM32duino libmaple core using the Arduino IDE.
The files relevant to the sketch are:

* RTCAdjust.ino - this is the sketch itself
* rtadjust.h    - utility functions for the adjustments
* rtadjust.cpp  - utility functions for the adjustments

The sketch is a demo that runs on the serial console, the commands on the serial console are

* t - display current time
* sYYYY-MM-DD HH:MM:SS - sync/set time
* cYYYY-MM-DD HH:MM:SS - calibrate rtc  
  where YYYY-MM-DD HH:MM_SS - is the current accurate time
* xnnnn - set the drift duration where nnnn is the drift duration  
        number of seconds for the stm32 rtc to drift 1 secs (faster)

t command displays the time and some additional info related to the adjustments e.g.  
```
RTC timestamp: 2018-12-10, 00:00:01  
last adj:RTC timestamp: 2018-12-10, 00:00:00  
drift duration, number of seconds for the stm32 rtc to drift 1 secs (faster): 3350
```
## How do the adjustments work

In your wiring/arduino setup() function, make it call the function *adjtime();*, e.g.
```
void setup() {
  /* adjust rtc */
  adjtime();
}
```
This function would make RTC adjustments based a calibrated drift duration or offset which i named as the drift duration. The drift duration is number of seconds for the stm32 rtc to drift 1 seconds (faster)


## How to do the calibrations using the demo sketch / app

This app/sketch itself has the commands to do the calibration. To do the calibration:

1. run  sYYYY-MM-DD HH:MM:SS - sync/set time  
   where YYYY-MM-DD HH:MM_SS - is the current accurate date/time
   This would set the RTC and the last adjustment date/time  to the date / time you supplied.  
   The last adjustment date/time is saved in 2 backup registers 8 and 9.
   
2. after a day or so, run cYYYY-MM-DD HH:MM:SS - calibrate rtc
   where YYYY-MM-DD HH:MM_SS - is the current accurate date/time at the time you do the calibration.
   This would compute the drift duration and save it in backup register 7.
   
3. reset the device and check the time after the calibration. if setup() calls adjtime(), the RTC would show the drift adjusted/compensated date/time.

## How to do the adjustments and calibrations work in actual code?

1. in setup() run adjtime() 
```
void setup() {
  /* adjust rtc */
  adjtime();
}
```
This would make the adjustments every time the sketch is started.
Note that adjtime() does the adjustment no more than once in 24 hours. This design is to prevent frequent adjustments which could result in cumulative errors as the granuality of adjustments is 1 sec. if you prefer for the adjustments to be more frequent you could comment the statement in the function.

2. The function to set the RTC and last adjusted date time is:  
```
/*  synctime() set the RTC clock and saves the time in backup register 8 and 9
 *  as the time of the last adjustment
 *
 *  call this function with the current accurate clock time to set the rtc
 *
 *  @param time the time_t value of the current accurate clock time
 */
void synctime(time_t time_now);
```

3. The function to perform the RTC calibration to compute the drift duration is
```
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
```

4. Dependencies, in the main sketch:
```
/* note that the adjustment functions needs this RTClock
 * if you rename variable rt, update rtadjust.h so that the
 * extern refers to this RTClock*/
RTClock rt(RTCSEL_LSE); // initialise
```

