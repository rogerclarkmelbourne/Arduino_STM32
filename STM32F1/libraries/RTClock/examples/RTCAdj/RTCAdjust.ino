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
 *
 */
#include <Arduino.h>
#include "rtadjust.h"

/* note that the adjustment functions needs this RTClock
 * if you rename variable rt, update rtadjust.h so that the
 * extern refers to this RTClock*/
RTClock rt(RTCSEL_LSE); // initialise

#define BUFLEN 100
uint8_t ind = 0;
char cmd = 0;
char buf[BUFLEN];
tm_t tm;

enum LineState {
	KEY,
	LINE
} state;

void processkey();
void showtime();
void synctime(char *buf, int len);
void settime(char *buf, int len);
void calibrate(char *buf, int len);
void setdriftdur(char *buf, int len);
void help();
void clearbuf();

void setup() {
	state = LineState::KEY;

	Serial.begin(115200);

	/* initialise access to backup registers,
	 * this is necessary due to the use of backup registers */
	bkp_init();

	/* adjust rtc */
	adjtime();

	//turn on the led basic check
	//pinMode(LED_BUILTIN, OUTPUT);
	//digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
	if(Serial.available())
		processkey();
	else
		delay(1);
}

void processkey() {
	char c = Serial.read();
	//echo the char;
	Serial.print(c);
	if( state == LineState::LINE ) {
		if (ind < BUFLEN && c != '\r') {
			if(c == 8 || c == 127 ) // backspace
				ind--;
			else
				buf[ind++] = c; // copy the chars into buf
		} else {
			switch(cmd) {
			case 's':
				synctime(buf,ind);
				break;
			case 'T':
				settime(buf,ind);
				break;
			case 'c':
				calibrate(buf,ind);
				break;
			case 'x':
				setdriftdur(buf,ind);
				break;
			default:
				break;
			}
			state = LineState::KEY;
			cmd = 0;
			clearbuf();
		}
	} else {
		switch(c) {
		case 't':
			showtime();
			break;
		case 's':
			state = LineState::LINE;
			clearbuf();
			cmd = 's';
			break;
		case 'T':
			state = LineState::LINE;
			clearbuf();
			cmd = 'T';
			break;
		case 'c':
			state = LineState::LINE;
			clearbuf();
			cmd = 'c';
			break;
		case 'x':
			state = LineState::LINE;
			clearbuf();
			cmd = 'x';
			break;
		case 'h':
		case 'H':
		case '?':
			help();
			break;
		default:
			break;
		}
	}
}


void showtime() {
    // get and print actual RTC timestamp
    rt.breakTime(rt.now(), tm);
    memset(buf,0,BUFLEN);
    sprintf(buf, "RTC timestamp: %u-%u-%u, %02u:%02u:%02u",
    		tm.year+1970, tm.month, tm.day, tm.hour, tm.minute, tm.second);
    Serial.println(buf);
    clearbuf();

    Serial.print("last adj:");
    rt.breakTime(getbkptime(), tm);
    memset(buf,0,BUFLEN);
    sprintf(buf, "RTC timestamp: %u-%u-%u, %02u:%02u:%02u",
    		tm.year+1970, tm.month, tm.day, tm.hour, tm.minute, tm.second);
    Serial.println(buf);
    clearbuf();

    Serial.print(F("drift duration, number of seconds for the stm32 rtc to drift 1 secs (faster):"));
    Serial.println(getdrift());

    Serial.print(F("BKP_RTCCR:"));
    Serial.println(getrtccr());
}

void synctime(char *buf, int len) {
	if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
	if(parsetimestamp(buf, tm) <0) {
		Serial.println(F("invalid date/time"));
		return;
	}

	time_t time = rt.makeTime(tm);
	/* this call the actual function to set the RTC and update
	 * the last adjusted time simultaneously
	 */
	synctime(time);
}



void calibrate(char *buf, int len) {
	if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
	if(parsetimestamp(buf, tm) <0) {
		Serial.println(F("invalid date/time"));
		return;
	}

	time_t time = rt.makeTime(tm);

	/* this call the calibratertc() function to calculate
	 * the drift duration
	 */
	calibratertc(time);
}

/*
 * this function sets the rtc directly by-passing all the adjustments
 *
 * note that this function is used during tests to simulate drifts etc
 * hence it is not features in help();
 *
 * in a normal context use synctime() to set the RTC time so that
 * the last adjustment date/time is updated as well
 */
void settime(char *buf, int len) {
	if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
	if(parsetimestamp(buf, tm) <0) {
		Serial.println(F("invalid date/time"));
		return;
	}

	rt.setTime(tm);
}

void setdriftdur(char *buf, int len) {
	if (len == BUFLEN) buf[BUFLEN-1] = 0; //terminate the string for safety
	int16_t drift_dur = atoi(buf);
	/* this funciton updates the drift duration directly */
	setbkpdrift(drift_dur);
}

void help() {
	Serial.println(F("h, H, ? - display help, this message"));
	Serial.println(F("t - current time"));
	Serial.println(F("sYYYY-MM-DD HH:MM:SS - sync/set time"));
	Serial.println(F("cYYYY-MM-DD HH:MM:SS - calibrate rtc"));
	Serial.println(F("where YYYY-MM-DD HH:MM_SS - is the current accurate time"));
	Serial.println(F("xnnnn - set the drift duration where nnnn is the drift duration"));
	Serial.println(F("        number of seconds for the stm32 rtc to drift 1 secs (faster)"));
	Serial.println(F("the s, c, x commands needs to end with a carriage return at the end of line"));
	Serial.println();
}

void clearbuf() {
	ind = 0;
	memset(buf,0,BUFLEN);
}

