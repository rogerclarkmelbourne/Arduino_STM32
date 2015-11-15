#include <utility/rtc_util.h>
#include <libmaple/rcc.h>
#include <libmaple/bitband.h>
#include <time.h>


#ifndef _RTCLOCK_H_
#define _RTCLOCK_H_





class RTClock {
 public:
 	RTClock();
    RTClock(rtc_clk_src src ); 
	RTClock(rtc_clk_src src, uint16 prescaler );
	//~RTClock(); //to implement
	

	void setTime (time_t time_stamp);
	void setTime (struct tm * tm_ptr); 
	
	struct tm* getTime(struct tm* tm_ptr); 
	time_t getTime();
	
	void createAlarm(voidFuncPtr function, time_t alarm_time_t); 
	void createAlarm(voidFuncPtr function, struct tm* alarm_tm);
	
	void attachSecondsInterrupt(voidFuncPtr function); 
	void detachSecondsInterrupt();
	
	void setAlarmTime (tm * tm_ptr);
	void setAlarmTime (time_t alarm_time); 
 //private:

} ;



#endif // _RTCLOCK_H_
                                                                                        