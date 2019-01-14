#include <utility/rtc_util.h>
#include <libmaple/rcc.h>
#include <libmaple/bitband.h>
#include <time.h>


#ifndef _RTCLOCK_H_
#define _RTCLOCK_H_

#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k
#define LEAP_YEAR(Y)  ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

#if !defined(__time_t_defined) // avoid conflict with newlib or other posix libc
  #warning "Using private time_t definintion"
  typedef uint32_t time_t;
#endif

static  const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

typedef struct tm_t {
	uint8_t  year;    // years since 1970
	uint8_t  month;   // month of a year - [ 1 to 12 ]
	uint8_t  day;     // day of a month - [ 1 to 31 ]
	uint8_t  weekday; // day of a week (first day is Monday) - [ 0 to 6 ]
	uint8_t  pm;      // AM: 0, PM: 1
	uint8_t  hour;    // hour of a day - [ 0 to 23 ]
	uint8_t  minute;  // minute of an hour - [ 0 to 59 ]
	uint8_t  second;  // second of a minute - [ 0 to 59 ]
} tm_t;

static inline uint8_t bcd2bin(uint8_t b) { return ( (10*(b>>4)) + (b&0x0F) ); }
static inline uint8_t bin2bcd(uint8_t b) { return ( ((b/10)<<4) + (b%10) ); }

class RTClock {
 public:
 	RTClock();
    RTClock(rtc_clk_src src ); 
	RTClock(rtc_clk_src src, uint32 prescaler );
	//~RTClock(); //to implement
	
	void breakTime(time_t epoch_time, tm_t & tmm);
	time_t makeTime(tm_t & tmm);
	
	void setTime (time_t time_stamp);
	void setTime (tm_t & tmm); 
	
	time_t getTime();
	void getTime(tm_t & tmm );

	time_t now() { return getTime(); }
	void now(tm_t & tmm ) { getTime(tmm); }  // non-standard use of now() function, added for compatibility with previous versions of the library

	uint8_t year(void)    { getTime(tmm); return tmm.year; }
	uint8_t month(void)   { getTime(tmm); return tmm.month; }
	uint8_t day(void)     { getTime(tmm); return tmm.day; }
	uint8_t weekday(void) { getTime(tmm); return tmm.weekday; }
	uint8_t hour(void)    { getTime(tmm); return tmm.hour; }
	uint8_t minute(void)  { getTime(tmm); return tmm.minute; }
	uint8_t second(void)  { getTime(tmm); return tmm.second; }
	uint8_t isPM(void)    { return ( hour()>=12 ); }
	
	uint8_t year(time_t t)    { breakTime(t, tmm); return tmm.year; }
	uint8_t month(time_t t)   { breakTime(t, tmm); return tmm.month; }
	uint8_t day(time_t t)     { breakTime(t, tmm); return tmm.day; }
	uint8_t weekday(time_t t) { breakTime(t, tmm); return tmm.weekday; }
	uint8_t hour(time_t t)    { breakTime(t, tmm); return tmm.hour; }
	uint8_t minute(time_t t)  { breakTime(t, tmm); return tmm.minute; }
	uint8_t second(time_t t)  { breakTime(t, tmm); return tmm.second; }
	uint8_t isPM(time_t t)    { return (hour(t)>=12); }

	// Usage: localtime = TimeZone(UnixTime, 8); 
	time_t TimeZone(time_t t, int TZ) { return ( t + (TZ * SECS_PER_HOUR)); } 
	
	// Usage:  1.  localtime = TimeZone(UnixTime, 9, 45)  ->   UTC +09:45 TimeZone; 
	time_t TimeZone(time_t t, int HTZ, int MTZ)  { return ( t + (HTZ * SECS_PER_HOUR) + (MTZ * 60)); }    // HTZ = Hour offset, MTZ = Minute offset
	
	void createAlarm(voidFuncPtr function, time_t alarm_time_t); 
	void createAlarm(voidFuncPtr function, struct tm_t & alarm_tm);
	void removeAlarm();
	
	void attachSecondsInterrupt(voidFuncPtr function); 
	void detachSecondsInterrupt();
	
    void attachAlarmInterrupt(voidFuncPtr function); 
	void attachAlarmInterrupt(voidFuncPtr function, time_t alarm_time);
	void detachAlarmInterrupt();
	
	void setAlarmTime (tm_t  & tm_ptr);
	void setAlarmTime (time_t alarm_time); 
	
 
	tm_t tmm;
};




#endif // _RTCLOCK_H_
                                                                                        
