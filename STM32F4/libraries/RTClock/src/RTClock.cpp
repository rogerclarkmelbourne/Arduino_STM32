/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 Inspired of the F1xx version adapted for the F4xx, not much F1xx left.
 author : Martin Ayotte, 2015.
 */

#include "RTClock.h"

static rtc_dev rtc = {
    .regs         = RTC_BASE,
//    .handlers     = { [NR_RTC_HANDLERS - 1] = 0 },
};

rtc_dev *RTC = &rtc;

voidFuncPtr handlerAlarmA = NULL;
voidFuncPtr handlerAlarmB = NULL;
voidFuncPtr handlerPeriodicWakeup = NULL;

//-----------------------------------------------------------------------------
RTClock::RTClock(rtc_clk_src src, uint16 sync_prescaler, uint16 async_prescaler) {
    uint32 t = 0;
    RCC_BASE->APB1ENR |= RCC_APB1RSTR_PWRRST;
    rtc_debug_printf("RCC_BASE->APB1ENR = %08X\r\n", RCC_BASE->APB1ENR);
    rtc_debug_printf("before bkp_init\r\n");
    bkp_init();		// turn on peripheral clocks to PWR and BKP and reset the backup domain via RCC registers.
                        // (we reset the backup domain here because we must in order to change the rtc clock source).
    rtc_debug_printf("before bkp_disable_writes\r\n");
    bkp_disable_writes();
    rtc_debug_printf("before bkp_enable_writes\r\n");
    bkp_enable_writes();	// enable writes to the backup registers and the RTC registers via the DBP bit in the PWR control register
    rtc_debug_printf("RCC_BASE->CFGR = %08X\r\n", RCC_BASE->CFGR);
    RCC_BASE->CFGR |= (0x08 << 16); // Set the RTCPRE to HSE / 8.
    rtc_debug_printf("RCC_BASE->CFGR = %08X\r\n", RCC_BASE->CFGR);

    switch (src) {	
        case RTCSEL_LSE :
	    rtc_debug_printf("Preparing RTC for LSE mode\r\n");
	    if ((RCC_BASE->BDCR & 0x00000300) != 0x0100)
                RCC_BASE->BDCR = 0x00010000; // Reset the entire Backup domain
            RCC_BASE->BDCR = 0x00008101;
            rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
            while (!(RCC_BASE->BDCR & 0x00000002)) {
                if (++t > 1000000) {
                    rtc_debug_printf("RCC_BASE->BDCR.LSERDY Timeout !\r\n");
                    rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
                    return;
                }
            }
            rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
            rtc_enter_config_mode();
            if (sync_prescaler == 0 && async_prescaler == 0)
                RTC_BASE->PRER = 255 | (127 << 16);
            else
                RTC_BASE->PRER = sync_prescaler | (async_prescaler << 16);
	    break;
	case RTCSEL_LSI :
	    rtc_debug_printf("Preparing RTC for LSI mode\r\n");
	    if ((RCC_BASE->BDCR & 0x00000300) != 0x0200)
                RCC_BASE->BDCR = 0x00010000; // Reset the entire Backup domain
            RCC_BASE->BDCR = 0x00008204;
            RCC_BASE->CSR |= 0x00000001;
            rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
            while (!(RCC_BASE->CSR & 0x00000002)) {
                if (++t > 1000000) {
                    rtc_debug_printf("RCC_BASE->CSR.LSIRDY Timeout !\r\n");
                    rtc_debug_printf("RCC_BASE->CSR = %08X\r\n", RCC_BASE->CSR);
                    return;
                }
            }
            rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
       	    rtc_enter_config_mode();
            if (sync_prescaler == 0 && async_prescaler == 0)
               	RTC_BASE->PRER = 249 | (127 << 16);
            else
               	RTC_BASE->PRER = sync_prescaler | (async_prescaler << 16);
	    break;
	case RTCSEL_DEFAULT: 
	case RTCSEL_HSE : 
	    rtc_debug_printf("Preparing RTC for HSE mode\r\n");
	    if ((RCC_BASE->BDCR & 0x00000300) != 0x0300)
                RCC_BASE->BDCR = 0x00010000; // Reset the entire Backup domain
            RCC_BASE->BDCR = 0x00008304;
            rtc_debug_printf("RCC_BASE->BDCR = %08X\r\n", RCC_BASE->BDCR);
            rtc_enter_config_mode();
            if (sync_prescaler == 0 && async_prescaler == 0)
                RTC_BASE->PRER = 7999 | (124 << 16);
            else
                RTC_BASE->PRER = sync_prescaler | (async_prescaler << 16);
	    break;
	case RTCSEL_NONE:
	    rtc_debug_printf("Preparing RTC for NONE mode\r\n");
	    if ((RCC_BASE->BDCR & 0x00000300) != 0x0000)
                RCC_BASE->BDCR = 0x00010000; // Reset the entire Backup domain
	    RCC_BASE->BDCR = RCC_BDCR_RTCSEL_NONE;
	    //do nothing. Have a look at the clocks to see the diff between NONE and DEFAULT
	    break;
    }
    RCC_BASE->CR |= 0x00000040; // Turn to 24hrs mode
//    rtc_debug_printf("before rtc_clear_sync\r\n");
//    rtc_clear_sync();
//    rtc_debug_printf("before rtc_wait_sync\r\n");
//    rtc_wait_sync();
    rtc_exit_config_mode();
    rtc_debug_printf("end of rtc_init\r\n");
}

/*
RTClock::~RTClock() {
    //to implement
}
*/	
	
//-----------------------------------------------------------------------------
void RTClock::setTime (time_t time_stamp)
{
	breakTime(time_stamp, tm); // time will be broken to tm
    setTime(tm);
}

//-----------------------------------------------------------------------------
void RTClock::setTime (tm_t & tm)
{
    if (tm.year > 99)
        tm.year = tm.year % 100;
    rtc_enter_config_mode();
    RTC_BASE->TR = BUILD_TIME_REGISTER(tm.hour, tm.minute, tm.second);
    RTC_BASE->DR = BUILD_DATE_REGISTER(tm.year, tm.month, tm.day, tm.weekday);
    rtc_exit_config_mode();		                
}

/*============================================================================*/	
/* functions to convert to and from system time */
/* These are for interfacing with time serivces and are not normally needed in a sketch */

// leap year calulator expects year argument as years offset from 1970
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

//static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

//-----------------------------------------------------------------------------
void RTClock::breakTime(time_t timeInput, tm_t & tm)
{
// break the given time_t into time components
// this is a more compact version of the C library localtime function
// note that year is offset from 1970 !!!

	uint8_t year;
	uint8_t month, monthLength;
	uint32_t time;
	uint32_t days;

	time = (uint32_t)timeInput;
	tm.second = time % 60;
	time /= 60; // now it is minutes
	tm.minute = time % 60;
	time /= 60; // now it is hours
	tm.hour = time % 24;
	time /= 24; // now it is days
	tm.weekday = ((time + 4) % 7); // Monday is day 1 // + 1;  // Sunday is day 1 

	year = 0;
	days = 0;
	while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
		year++;
	}
	tm.year = year; // year is offset from 1970 

	days -= LEAP_YEAR(year) ? 366 : 365;
	time -= days; // now it is days in this year, starting at 0

	days = 0;
	month = 0;
	monthLength = 0;
	for (month=0; month<12; month++) {
		if (month==1) { // february
			if (LEAP_YEAR(year)) {
				monthLength=29;
			} else {
				monthLength=28;
			}
		} else {
			monthLength = monthDays[month];
		}

		if (time >= monthLength) {
			time -= monthLength;
		} else {
			break;
		}
	}
	tm.month = month + 1;  // jan is month 1  
	tm.day = time + 1;     // day of month
}

//-----------------------------------------------------------------------------
time_t RTClock::makeTime(tm_t & tm)
{
// assemble time elements into time_t 
// note year argument is offset from 1970 (see macros in time.h to convert to other formats)
// previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
  
	int i;
	uint32_t seconds;

	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds = tm.year*(SECS_PER_DAY * 365);
	for (i = 0; i < tm.year; i++) {
		if (LEAP_YEAR(i)) {
			seconds +=  SECS_PER_DAY;   // add extra days for leap years
		}
	}

	// add days for this year, months start from 1
	for (i = 1; i < tm.month; i++) {
		if ( (i == 2) && LEAP_YEAR(tm.year)) { 
			seconds += SECS_PER_DAY * 29;
		} else {
			seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
		}
	}
	seconds+= (tm.day-1) * SECS_PER_DAY;
	seconds+= tm.hour * SECS_PER_HOUR;
	seconds+= tm.minute * SECS_PER_MIN;
	seconds+= tm.second;
	return (time_t)seconds; 
}

//-----------------------------------------------------------------------------
void RTClock::getTime(tm_t & tm)
{
    uint32_t dr_reg, tr_reg;
	do { // read multiple time till both readings are equal
		dr_reg = getDReg();
		tr_reg = getTReg();
	} while ( (dr_reg!=getDReg()) || (tr_reg!=getTReg()) );
	tm.year    = _year(dr_reg);
    tm.month   = _month(dr_reg);
    tm.day     = _day(dr_reg);
    tm.weekday = _weekday(dr_reg);
    tm.pm      = _pm(tr_reg);
    tm.hour    = _hour(tr_reg);
    tm.minute  = _minute(tr_reg);
    tm.second  = _second(tr_reg);
}

//-----------------------------------------------------------------------------
time_t RTClock::getTime()
{
	getTime(tm);
	return makeTime(tm);
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmATime (tm_t * tm_ptr, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{
    uint32 t = 0;
    rtc_enter_config_mode();
    unsigned int bits = ((tm_ptr->day / 10) << 28) | ((tm_ptr->day % 10) << 24) |
		        ((tm_ptr->hour / 10) << 20) | ((tm_ptr->hour % 10) << 16) | 
			((tm_ptr->minute / 10) << 12) | ((tm_ptr->minute % 10) << 8) | 
			((tm_ptr->second / 10) << 4) | (tm_ptr->second % 10);
    if (!date_match) bits |= (1 << 31);
    if (!hours_match) bits |= (1 << 23);
    if (!mins_match) bits |= (1 << 15);
    if (!secs_match) bits |= (1 << 7);
    RTC_BASE->CR &= ~(1 << RTC_CR_ALRAE_BIT);
    while (!(RTC_BASE->ISR & (1 << RTC_ISR_ALRAWF_BIT))) {
       if (++t > 1000000) {
           rtc_debug_printf("RTC_BASE->ISR.ALRAWF Timeout !\r\n");
           rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
           return;
       }
    }
    RTC_BASE->ALRMAR = bits;
    RTC_BASE->CR |= (1 << RTC_CR_ALRAE_BIT);
    RTC_BASE->CR |= (1 << RTC_CR_ALRAIE_BIT); // turn on ALRAIE
    rtc_exit_config_mode();
    nvic_irq_enable(NVIC_RTCALARM);
    nvic_irq_enable(NVIC_RTC);
    rtc_enable_alarm_event();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmATime (time_t alarm_time, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{	
    breakTime(alarm_time, tm);
    setAlarmATime(&tm, hours_match, mins_match, secs_match, date_match);
}

//-----------------------------------------------------------------------------
void RTClock::turnOffAlarmA(void)
{
    rtc_enter_config_mode();
    RTC_BASE->CR &= ~(1 << RTC_CR_ALRAIE_BIT); // turn off ALRAIE
    rtc_exit_config_mode();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmBTime (tm_t * tm_ptr, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{
    uint32 t = 0;
    rtc_enter_config_mode();
    unsigned int bits = ((tm_ptr->day / 10) << 28) | ((tm_ptr->day % 10) << 24) |
		        ((tm_ptr->hour / 10) << 20) | ((tm_ptr->hour % 10) << 16) | 
			((tm_ptr->minute / 10) << 12) | ((tm_ptr->minute % 10) << 8) | 
			((tm_ptr->second / 10) << 4) | (tm_ptr->second % 10);
    if (!date_match) bits |= (1 << 31);
    if (!hours_match) bits |= (1 << 23);
    if (!mins_match) bits |= (1 << 15);
    if (!secs_match) bits |= (1 << 7);
    RTC_BASE->CR &= ~(1 << RTC_CR_ALRBE_BIT);
    while (!(RTC_BASE->ISR & (1 << RTC_ISR_ALRBWF_BIT))) {
       if (++t > 1000000) {
           rtc_debug_printf("RTC_BASE->ISR.ALRBWF Timeout !\r\n");
           rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
           return;
       }
    }
    RTC_BASE->ALRMBR = bits;
    RTC_BASE->CR |= (1 << RTC_CR_ALRBE_BIT);
    RTC_BASE->CR |= (1 << RTC_CR_ALRBIE_BIT); // turn on ALRBIE
    rtc_exit_config_mode();
    nvic_irq_enable(NVIC_RTCALARM);
    nvic_irq_enable(NVIC_RTC);
    rtc_enable_alarm_event();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmBTime (time_t alarm_time, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{	
    breakTime(alarm_time, tm);
    setAlarmBTime(&tm, hours_match, mins_match, secs_match, date_match);
}

//-----------------------------------------------------------------------------
void RTClock::turnOffAlarmB() {
    rtc_enter_config_mode();
    RTC_BASE->CR &= ~(1 << RTC_CR_ALRBIE_BIT); // turn off ALRBIE
    rtc_exit_config_mode();
}

//-----------------------------------------------------------------------------
void RTClock::setPeriodicWakeup(uint16 period)
{
    uint32 t = 0;
    rtc_enter_config_mode();
    RTC_BASE->CR &= ~(1 << RTC_CR_WUTE_BIT);
    while (!(RTC_BASE->ISR & (1 << RTC_ISR_WUTWF_BIT))) {
       if (++t > 1000000) {
           rtc_debug_printf("RTC_BASE->ISR.WUTWF Timeout !\r\n");
           rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
           return;
       }
    }
    rtc_debug_printf("before setting RTC_BASE->WUTR\r\n");    
    RTC_BASE->WUTR = period; // set the period
    rtc_debug_printf("RTC_BASE->WUTR = %08X\r\n", RTC_BASE->WUTR);
    rtc_debug_printf("before setting RTC_BASE->CR.WUCKSEL\r\n");    
    RTC_BASE->CR &= ~(3); RTC_BASE->CR |= 4; // Set the WUCKSEL to 1Hz (0x00000004)
    RTC_BASE->ISR &= ~(1 << RTC_ISR_WUTF_BIT);
    RTC_BASE->CR |= (1 << RTC_CR_WUTE_BIT);
    if (period == 0)
        RTC_BASE->CR &= ~(1 << RTC_CR_WUTIE_BIT); // if period is 0, turn off periodic wakeup interrupt.
    else {
        rtc_debug_printf("before turn ON RTC_BASE->CR.WUTIE\r\n");    
        RTC_BASE->CR |= (1 << RTC_CR_WUTIE_BIT); // turn on WUTIE
    }
    rtc_debug_printf("RCC_BASE->CR = %08X\r\n", RCC_BASE->CR);
    rtc_exit_config_mode();
    rtc_enable_wakeup_event();
    nvic_irq_enable(NVIC_RTC);
    rtc_debug_printf("setPeriodicWakeup() done !\r\n");
}


void RTClock::attachAlarmAInterrupt(voidFuncPtr function) {
    handlerAlarmA = function;
}

void RTClock::detachAlarmAInterrupt() {
    handlerAlarmA = NULL;
}

void RTClock::attachAlarmBInterrupt(voidFuncPtr function) {
    handlerAlarmB = function;
}

void RTClock::detachAlarmBInterrupt() {
    handlerAlarmB = NULL;
}

void RTClock::attachPeriodicWakeupInterrupt(voidFuncPtr function) {
    handlerPeriodicWakeup = function;
}

void RTClock::detachPeriodicWakeupInterrupt() {
    handlerPeriodicWakeup = NULL;
}



extern "C" {
    void __irq_rtc(void) {
        rtc_debug_printf("__irq_rtc() called !\r\n");
        rtc_enter_config_mode();
        RTC_BASE->ISR &= ~(1 << RTC_ISR_WUTF_BIT);
        rtc_exit_config_mode();
        *bb_perip(&EXTI_BASE->PR, EXTI_RTC_WAKEUP_BIT) = 1;
        if (handlerPeriodicWakeup != NULL) {
            handlerPeriodicWakeup();
        }
    }

    void __irq_rtcalarm(void) {
        bool isAlarmA = false;
        bool isAlarmB = false;
        rtc_debug_printf("__irq_rtcalarm() called !\r\n");
        rtc_enter_config_mode();
        if (RTC_BASE->ISR & (1 << RTC_ISR_ALRAF_BIT)) {
            isAlarmA = true;
            rtc_debug_printf("AlarmA !\r\n");
            RTC_BASE->ISR &= ~(1 << RTC_ISR_ALRAF_BIT);
        }
        if (RTC_BASE->ISR & (1 << RTC_ISR_ALRBF_BIT)) {
            isAlarmB = true;
            rtc_debug_printf("AlarmB !\r\n");
            RTC_BASE->ISR &= ~(1 << RTC_ISR_ALRBF_BIT);
        }
        rtc_exit_config_mode();
        *bb_perip(&EXTI_BASE->PR, EXTI_RTC_ALARM_BIT) = 1;
        if (isAlarmA && handlerAlarmA != NULL) {
            handlerAlarmA();
        }
        if (isAlarmB && handlerAlarmB != NULL) {
            handlerAlarmB();
        }
    }
}

