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

#include <RTClock.h>



#ifdef RTC_DEBUG
  #include <stdio.h>
  #include <usb_serial.h>
  char dbg_s[200];
  #define PRINTF(...) { sprintf(dbg_s, __VA_ARGS__); Serial.print(dbg_s); }
#else
  #define PRINTF(...) 
#endif
  #define PRINTF1(...) { sprintf(dbg_s, __VA_ARGS__); Serial.print(dbg_s); }


typedef struct {
uint16_t s_presc;
uint16_t as_presc;
} prescaler_t;

const prescaler_t prescalers[4] = {
	{   0,   0}, // RTCSEL_NONE	
	{ 255, 127}, // RTCSEL_LSE
	{ 249, 127}, // RTCSEL_LSI
	{7999, 124}, // RTCSEL_HSE
};

//-----------------------------------------------------------------------------
void RTClock::begin(rtc_clk_src src, uint16 sync_presc, uint16 async_presc)
{
	clk_src = src;
	sync_prescaler = sync_presc;
	async_prescaler = async_presc;

    PRINTF("> RTClock::begin\n");
    //PRINTF("PWR->CR(1) = %08X\n", PWR->CR);
    bkp_init();		// turn on peripheral clocks to PWR and BKP and reset the backup domain via RCC registers.

    PRINTF("bkp_enable_writes\n");
    bkp_enable_writes();	// enable writes to the backup registers and the RTC registers via the DBP bit in the PWR control register
    PRINTF("PWR->CR(2) = %08X\n", PWR->CR);

	rcc_set_prescaler(RCC_PRESCALER_RTC, RCC_RTCCLK_DIV(CRYSTAL_FREQ)); // Set the RTCPRE to 8.
	PRINTF("RCC->CFGR = %08X\n", RCC->CFGR);

	PRINTF("RTC clock source: %s\n", (clk_src==RTCSEL_LSE)?"LSE":((clk_src==RTCSEL_LSI)?"LSI":((clk_src==RTCSEL_HSE)?"HSE":"NONE")));
    switch (clk_src)
	{
	case RTCSEL_LSE:
	{
		PRINTF("Preparing RTC for LSE mode, RCC->BDCR = %08X\n", RCC->BDCR);
	    if ((RCC->BDCR & RCC_BDCR_RTCSEL_MASK) == RCC_BDCR_RTCSEL_LSE)
			break;
		RCC->BDCR = RCC_BDCR_BDRST; // Reset the entire Backup domain
		PRINTF("BCKP domain reset\n");

		RCC->BDCR = (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSE | RCC_BDCR_LSEON);
		PRINTF("RCC->BDCR = %08X\n", RCC->BDCR);
		uint32 t = 0;
		while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {
			if (++t > 10000000) {
				PRINTF("RCC LSERDY Timeout ! BDCR = %08X\n", RCC->BDCR);
				break;
			}
		}
	}	break;

	case RTCSEL_LSI:
	{
	    PRINTF("Preparing RTC for LSI mode\n");
	    if ((RCC->BDCR & RCC_BDCR_RTCSEL_MASK) == RCC_BDCR_RTCSEL_LSI)
			break;
		RCC->BDCR = RCC_BDCR_BDRST; // Reset the entire Backup domain
		PRINTF("BCKP domain reset\n");
		RCC->BDCR = (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_LSI | RCC_BDCR_LSEBYP);
		PRINTF("RCC->BDCR = %08X\r\n", RCC->BDCR);

		RCC->CSR = RCC_CSR_LSION;
		uint32 t = 0;
		while (!(RCC->CSR & RCC_CSR_LSIRDY)) {
			if (++t > 10000000) {
				PRINTF("RCC LSIRDY Timeout ! CSR = %08X\n", RCC->CSR);
				goto end0;
			}
		}
		PRINTF("RCC->CSR = %08X\n", RCC->CSR);
	}   break;
	
	case RTCSEL_HSE :
	    PRINTF("Preparing RTC for HSE mode, RCC->BDCR = %08X\n", RCC->BDCR);
	    if ((RCC->BDCR & RCC_BDCR_RTCSEL_MASK) == RCC_BDCR_RTCSEL_HSE)
			break;
		RCC->BDCR = RCC_BDCR_BDRST; // Reset the entire Backup domain
		PRINTF("BCKP domain reset\n");
		RCC->BDCR = (RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_HSE | RCC_BDCR_LSEBYP);
		PRINTF("RCC->BDCR = %08X\n", RCC->BDCR);
	    break;

	case RTCSEL_NONE:
	    PRINTF("Preparing RTC for NONE mode\n");
	    if ((RCC->BDCR & RCC_BDCR_RTCSEL_MASK) != RCC_BDCR_RTCSEL_NONE)
            RCC->BDCR = RCC_BDCR_BDRST; // Reset the entire Backup domain
	    //do nothing. Have a look at the clocks to see the diff between NONE and DEFAULT
	    goto end0;
		break;
    }
	if ( (sync_prescaler + async_prescaler) == 0) {
		sync_prescaler = prescalers[clk_src].s_presc;
		async_prescaler = prescalers[clk_src].as_presc;
	}
	PRINTF("sync_prescaler = %d, async_prescaler = %d\n", sync_prescaler, async_prescaler);
	rtc_enter_config_mode();
	RTC->PRER = (uint32)(async_prescaler << 16) + sync_prescaler;
	RTC->DR = 0x00002101; // reset value
	RTC->TR = 0x00000000; // reset value
    //RCC->CR |= RTC_CR_BYPSHAD;
	*bb_perip(&RTC->CR, RTC_CR_BYPSHAD_BIT) = 1; // bypass shadow regs
	PRINTF("RTC PRER: %08X, CR: %08X\n", RTC->PRER, RTC->CR);
    rtc_exit_config_mode();

end0:
    PRINTF("< RTClock::begin\n");
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmATime (bool hours_match, bool mins_match, bool secs_match, bool date_match)
{
    rtc_enter_config_mode();
    unsigned int bits = (bin2bcd(_tm.day)<<24) + (bin2bcd(_tm.hour)<<16) + 
			(bin2bcd(_tm.minute)<<8) + bin2bcd(_tm.second);
    if (!date_match) bits |= (1 << 31);
    if (!hours_match) bits |= (1 << 23);
    if (!mins_match) bits |= (1 << 15);
    if (!secs_match) bits |= (1 << 7);
    RTC->CR &= ~(RTC_CR_ALRAE);
    uint32 t = 0;
    while (!(RTC->ISR & RTC_ISR_ALRAWF)) {
       if (++t > 1000000) {
           PRINTF("RTC ALRAWF Timeout ! ISR = %08X\n", RTC->ISR);
           return;
       }
    }
    RTC->ALRMAR = bits;
    RTC->CR |= (RTC_CR_ALRAE  |RTC_CR_ALRAIE); // turn on ALRAIE
    rtc_exit_config_mode();
    nvic_irq_enable(NVIC_RTCALARM);
    nvic_irq_enable(NVIC_RTC);
    rtc_enable_alarm_event();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmATime (time_t alarm_time, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{	
    breakTime(alarm_time, &_tm);
    setAlarmATime(hours_match, mins_match, secs_match, date_match);
}

//-----------------------------------------------------------------------------
void RTClock::turnOffAlarmA(void)
{
    rtc_enter_config_mode();
    RTC->CR &= ~(RTC_CR_ALRAIE); // turn off ALRAIE
    rtc_exit_config_mode();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmBTime (bool hours_match, bool mins_match, bool secs_match, bool date_match)
{
    rtc_enter_config_mode();
    unsigned int bits = (bin2bcd(_tm.day) << 24) + (bin2bcd(_tm.hour) << 16) + 
			(bin2bcd(_tm.minute) << 8) + bin2bcd(_tm.second);
    if (!date_match) bits |= (1 << 31);
    if (!hours_match) bits |= (1 << 23);
    if (!mins_match) bits |= (1 << 15);
    if (!secs_match) bits |= (1 << 7);
    RTC->CR &= ~(RTC_CR_ALRBE);
    uint32 t = 0;
    while (!(RTC->ISR & RTC_ISR_ALRBWF)) {
       if (++t > 1000000) {
           PRINTF("RTC ALRBWF Timeout ! ISR = %08X\n", RTC->ISR);
           return;
       }
    }
    RTC->ALRMBR = bits;
    RTC->CR |= (RTC_CR_ALRBE | RTC_CR_ALRBIE); // turn on ALRBIE
    rtc_exit_config_mode();
    nvic_irq_enable(NVIC_RTCALARM);
    nvic_irq_enable(NVIC_RTC);
    rtc_enable_alarm_event();
}

//-----------------------------------------------------------------------------
void RTClock::setAlarmBTime (time_t alarm_time, bool hours_match, bool mins_match, bool secs_match, bool date_match)
{	
    breakTime(alarm_time, &_tm);
    setAlarmBTime(hours_match, mins_match, secs_match, date_match);
}

//-----------------------------------------------------------------------------
void RTClock::turnOffAlarmB() {
    rtc_enter_config_mode();
    RTC->CR &= ~(RTC_CR_ALRBIE); // turn off ALRBIE
    rtc_exit_config_mode();
}

//-----------------------------------------------------------------------------
void RTClock::setPeriodicWakeup(uint16 period)
{
    PRINTF("< setPeriodicWakeup\n");
    rtc_enter_config_mode();
    RTC->CR &= ~(RTC_CR_WUTE);
    uint32 t = 0;
    while (!(RTC->ISR & RTC_ISR_WUTWF)) {
       if (++t > 1000000) {
           PRINTF("RTC WUTWF Timeout ! ISR = %08X\n", RTC->ISR);
           return;
       }
    }
    PRINTF("before setting RTC->WUTR\r\n");    
    RTC->WUTR = period; // set the period
    PRINTF("RTC->WUTR = %08X\r\n", RTC->WUTR);
    PRINTF("before setting RTC->CR.WUCKSEL\r\n");    
    RTC->CR &= ~(RTC_CR_WUCKSEL_MASK);
	RTC->CR |= 4; // Set the WUCKSEL to 1Hz (0x00000004)
	*bb_perip(&RTC->ISR, RTC_ISR_WUTF_BIT) = 0;
    RTC->CR |= RTC_CR_WUTE;
    if (period == 0)
        RTC->CR &= ~(RTC_CR_WUTIE); // if period is 0, turn off periodic wakeup interrupt.
    else {
        PRINTF("before turn ON RTC->CR.WUTIE\r\n");    
        RTC->CR |= (RTC_CR_WUTIE); // turn on WUTIE
    }
    PRINTF("RCC->CR = %08X\r\n", RCC->CR);
    rtc_exit_config_mode();
    rtc_enable_wakeup_event();
    nvic_irq_enable(NVIC_RTC);
    PRINTF("setPeriodicWakeup >\n");
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

