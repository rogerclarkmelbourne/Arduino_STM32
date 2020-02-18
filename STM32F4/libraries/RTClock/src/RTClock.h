//
// RTClock.h
//

#ifndef _RTCLOCK_H_
#define _RTCLOCK_H_


#include <libmaple/rtc.h>
#include <libmaple/bkp.h>

//#define RTC_DEBUG 1


class RTClock {
  public:
 	RTClock() {}
	void begin(rtc_clk_src _src, uint16 _sync_prescaler, uint16 _async_prescaler);
	void begin(rtc_clk_src _src) { begin(_src, 0, 0); }
	void begin() { begin(RTCSEL_LSE, 0, 0); }
	//~RTClock(); //to implement
	
	//void breakTime(time_t epoch_time, tm_t &_tm);
	#define breakTime _breakTime
	time_t makeTime(tm_t & tmm) { return _makeTime(&_tm); }
	void setTime(time_t time_stamp) { breakTime(time_stamp, &_tm); _setTime(&_tm); }
	void setTime(tm_t & tmm) { _setTime(&tmm); } 

	time_t getTimeNow() { _getTime(&_tm); return makeTime(_tm); }
	time_t getTime() { return getTimeNow(); }
	time_t now() { return getTimeNow(); }

	void getTime(tm_t & tmm) { _getTime(&tmm); }
	void now(tm_t & tmm ) { _getTime(&tmm); }  // non-standard use of now() function, added for compatibility with previous versions of the library

	uint8_t year(void)    { _getTime(&_tm); return _tm.year; }
	uint8_t month(void)   { _getTime(&_tm); return _tm.month; }
	uint8_t day(void)     { _getTime(&_tm); return _tm.day; }
	uint8_t weekday(void) { _getTime(&_tm); return _tm.weekday; }
	uint8_t hour(void)    { _getTime(&_tm); return _tm.hour; }
	uint8_t minute(void)  { _getTime(&_tm); return _tm.minute; }
	uint8_t second(void)  { _getTime(&_tm); return _tm.second; }
	//uint8_t pm(void)      { return _pm(RTC->TR); }
	uint8_t isPM(void)    { return ( hour()>=12 ); }
	
	uint8_t year(time_t t)    { breakTime(t, &_tm); return _tm.year; }
	uint8_t month(time_t t)   { breakTime(t, &_tm); return _tm.month; }
	uint8_t day(time_t t)     { breakTime(t, &_tm); return _tm.day; }
	uint8_t weekday(time_t t) { breakTime(t, &_tm); return _tm.weekday; }
	uint8_t hour(time_t t)    { breakTime(t, &_tm); return _tm.hour; }
	uint8_t minute(time_t t)  { breakTime(t, &_tm); return _tm.minute; }
	uint8_t second(time_t t)  { breakTime(t, &_tm); return _tm.second; }
	uint8_t isPM(time_t t)    { return (hour(t)>=12); }
	
	void setAlarmATime (bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false);
	void setAlarmATime (time_t alarm_time, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false); 
	void turnOffAlarmA();
	void setAlarmBTime (bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false);
	void setAlarmBTime (time_t alarm_time, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false); 
	void turnOffAlarmB();
	
	void setPeriodicWakeup(uint16 period);

	void attachPeriodicWakeupInterrupt(voidFuncPtr function); 
	void detachPeriodicWakeupInterrupt();
	inline void attachSecondsInterrupt(voidFuncPtr function) { attachPeriodicWakeupInterrupt(function); }
	inline void detachSecondsInterrupt() { detachPeriodicWakeupInterrupt(); }

	void attachAlarmAInterrupt(voidFuncPtr function); 
	void detachAlarmAInterrupt();
	void attachAlarmBInterrupt(voidFuncPtr function); 
	void detachAlarmBInterrupt();

	tm_t _tm;
	uint16_t sync_prescaler, async_prescaler;
	rtc_clk_src clk_src;
};


#endif // _RTCLOCK_H_
