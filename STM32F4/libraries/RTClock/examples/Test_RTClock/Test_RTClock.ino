/*
	This is an example of how to use the RTclock of STM32F4 device
	
	This example can also be used to set the RTC to the current epoch time:
	- goto: http://www.unixtimestamp.com/
	- enter the current date and time to the right field "Timestamp converter"
	- press the "Convert" button
	- 
*/

#include <RTClock.h>

#include <Streaming.h>


//RTClock rt(RTCSEL_LSE); // initialise
RTClock rtc;

time_t tt;
tm_t tm;

const uint32_t DEFAULT_TIME = 1498944019; // 2017.07.01, 21:20:19 used as reference epoch time

#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#define LED_PIN BOARD_LED_PIN

//-----------------------------------------------------------------------------
void blink () 
{
	digitalWrite(LED_PIN, digitalRead(LED_PIN)?LOW:HIGH);
}

uint8_t s[20]; // for serial parsing

//-----------------------------------------------------------------------------
char * read_line()
{
	while ( Serial.available() ) Serial.read(); // empty Rx buffer
	while ( Serial.available()<=0 ) ; // wait for new characters
	uint8_t c, i = 0;
	s[0] = 0;
	while ( Serial.available() && (i<20) ) {
		c = Serial.read();
		if ( c=='\n' || c== '\r') {
			s[i] = 0;
			break;
		}
		s[i++] = c;
	}
	while ( Serial.available() ) Serial.read(); // flush Rx
	return (char*)&s[0];
}

//-----------------------------------------------------------------------------
void processSyncMessage(void)
{
	if ( Serial.available() ) {
		if( *read_line()==(TIME_HEADER) ) {
			uint32_t pctime = atoi((const char*)&s[1]);
			Serial << ("Epoch time received: ") << pctime << endl;
			if ( pctime >= DEFAULT_TIME) { // check the integer is a valid epoch time
				rtc.setTime(pctime); // Set RTC to the time received via the serial port
			}
		}
		Serial << endl;
	}
}

//-----------------------------------------------------------------------------
void Change_DateTime(void)
{
	// check and correct the weekday if necessary
	rtc.getTime(tm);	
	Serial << "Current weekday is " << (tm.weekday);
	// get time elements
	tt = rtc.makeTime(tm);
	uint16_t tmp = rtc.weekday(tt);
	if ( tmp!=tm.weekday ) {// correct weekday
		rtc.setTime(tt);
		Serial << " instead of " << tmp << ". Now is corrected.\n";
	} else {
		Serial << " - seems to be fine, no need to change it.\n";
	}

	uint8_t chg = 0;
	// get time elements
	rtc.getTime(tm);	
	Serial << "\nCurrent RTC date: " << (1970+tm.year) << "." << (tm.month) << (".") << (tm.day) << (", weekday: ") << (tm.weekday) << endl;
	Serial << "Do you want to change it? (y/n)\n";
	if ( *read_line()=='y' ) {
		// change here the date
change_year:
		Serial << "Current year: " << (1970+tm.year) << ". Enter new year in \"YYYY\" format (numbers only) or press enter to skip.\n";
		if (*read_line()==0) goto change_month;
		tmp = atoi((const char*)s);
		if ( tmp<1970 ) { Serial << "Please enter a valid number greater than 1970\n"; goto change_year; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_year;
		tm.year = tmp-1970;
		chg = 1;
change_month:
		Serial << "Current month: " << tm.month << ". Enter new month in \"MM\" format [1..12] or press enter to skip.\n";
		if (*read_line()==0) goto change_day;
		tmp = atoi((const char*)s);
		if ( tmp<1 || tmp>12 ) { Serial << "Please enter a valid number [1..12]\n"; goto change_month; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_month;
		tm.month = tmp;
		chg = 1;
change_day:
		Serial << "Current day: " << tm.day << ". Enter new day in \"DD\" format [1..31] or press enter to skip.\n";
		if (*read_line()==0) goto change_weekday;
		tmp = atoi((const char*)s);
		if ( tmp<1 || tmp>31 ) { Serial << "Please enter a valid number [1..31]\n"; goto change_day; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_day;
		tm.day = tmp;
		chg = 1;
change_weekday:
		Serial << "Current weekday: " << tm.weekday << ". Enter new weekday [1(=Monday)..7(=Sunday)] or press enter to skip.\n";
		if (*read_line()==0) goto change_time;
		tmp = atoi((const char*)s);
		if ( tmp<1 || tmp>7 ) { Serial << "Please enter a valid number [1..7]\n"; goto change_weekday; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_weekday;
		tm.weekday = tmp;
		chg = 1;
change_time:
		Serial << "Current RTC time: " << _TIME(tm.hour, tm.minute, tm.second) << endl;
		Serial << "Do you want to change it? (y/n)\n";
		if ( *read_line()=='n' ) goto change_end;
change_hour:
		Serial << "Current hour: " << tm.hour << ". Enter new hour [0..23] or press enter to skip.\n";
		if (*read_line()==0) goto change_minute;
		tmp = atoi((const char*)s);
		if ( tmp>23 ) { Serial << "Please enter a valid number [0..23]\n"; goto change_hour; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_hour;
		tm.hour = tmp;
		chg = 1;
change_minute:
		Serial << "Current minute: " << tm.minute << ". Enter new minute [0..59] or press enter to skip.\n";
		if (*read_line()==0) goto change_second;
		tmp = atoi((const char*)s);
		if ( tmp>59 ) { Serial << "Please enter a valid number [0..59]\n"; goto change_minute; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_minute;
		tm.minute = tmp;
		chg = 1;
change_second:
		Serial << "Current second: " << tm.second << ". Enter new second [0..59] or press enter to skip.\n";
		if (*read_line()==0) goto change_end;
		tmp = atoi((const char*)s);
		if ( tmp>59 ) { Serial << "Please enter a valid number [0..59]\n"; goto change_second; }
		Serial << "You entered: " << tmp << ". Accept value? (y/n)\n";
		if ( *read_line()=='n' ) goto change_second;
		tm.second = tmp;
		chg = 1;
	} else {
		goto change_time;
	}

change_end:
	if ( chg ) {
		// set here the RTC time.
		Serial << "Changed date & time: " << (1970+tm.year) << "." << (tm.month) << (".") << (tm.day) << (", weekday: ") << (tm.weekday) << ", " << _TIME(tm.hour, tm.minute, tm.second) << endl;
		Serial << "Write now to RTC? (y/n)\n";
		read_line();
		if ( s[0]=='y' ) {
			rtc.setTime(tm);
			Serial << "Data written to RTC.\n\n";
		}
	} else
		Serial << "RTC was not changed.\n\n";
} 
//-----------------------------------------------------------------------------
void setup() 
{
	Serial.begin();
	delay(3000);

	pinMode(LED_PIN, OUTPUT);
	
	Serial << "This is an example of how to use the STM32F4 RTC library.\n\n";

	Change_DateTime();
}

//-----------------------------------------------------------------------------
void loop() 
{
	if ( Serial.available() ) {
		// adjust time according to received epoch time from PC
		processSyncMessage();
	}

	if (tt!=rtc.now()) {
		// get epoch time
		tt = rtc.now();
		Serial << ("- RTC epoch timestamp = ") << (tt);
		// get time elements
		rtc.getTime(tm);
		//rtc.breakTime(tt, tm);
		Serial << (" == ") << (1970+tm.year) << "." << (tm.month) << (".") << (tm.day) << (", weekday ") << (tm.weekday) << (", ");
		Serial << _TIME(tm.hour, tm.minute, tm.second) << endl;
	}
}