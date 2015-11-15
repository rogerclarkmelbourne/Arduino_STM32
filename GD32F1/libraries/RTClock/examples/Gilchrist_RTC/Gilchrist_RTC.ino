/*
 *  test-rtc.c
 *  
 *  Example program that sets up the Real Time Clock and then blinks the
 *  LED in patterns for seconds and alarm interrupts.
 *
 *  Created by  Rod Gilchrist on 11-12-24.
 Ray Burnette: 20150521:
   Edited: \Documents\Arduino\hardware\STM32\STM32F1\system\libmaple\stm32f1\include\series\rcc.h to include:
      #include <libmaple/bitband.h>
      #define RCC_BDCR_RTCSEL_LSI             (0x2 << 8)
      static inline void rcc_start_lsi(void)
      static inline void rcc_start_lse(void)
      static inline void rcc_start_hse(void)

   Arduino GUI 1.7.3 from Arduino.org
    Sketch uses 20,268 bytes (18%) of program storage space. Maximum is 110,592 bytes.
    Global variables use 4,552 bytes of dynamic memory.
   */

#include "RTClock.h"

#define BOARD_LED_PIN PB1

int globAlmCnt = 0;
int globOvCnt = 0;
int globSecCnt = 0;
int specAlmCnt = 0;
int lastGlobAlmCnt = -1;
int lastSpecAlmCnt = -1;

void rtc_sec_intr() { if (rtc_is_second()) globSecCnt++; }
void rtc_ovf_intr() { if (rtc_is_overflow()) globOvCnt++; }
void rtc_glob_alm_intr() { if (rtc_is_alarm()) globAlmCnt++; }
void rtc_spec_alm_intr() { if (rtc_is_alarm()) specAlmCnt++; }

void setup() {
   // http://forums.leaflabs.com/topic.php?id=1437
   // slow! div speed. NOTE! 512 is stop/hang when USB not connected!
   // rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_256);
   // Normal speed:
   // rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_1);

    pinMode(BOARD_LED_PIN, OUTPUT);
	delay(5000);
	Serial.println("begin RTC blink");
	delay(1000);	

	rtc_init(RTCSEL_LSI);
	rtc_set_prescaler_load(0x7fff);
	rtc_set_count(0);
	
	rtc_attach_interrupt(RTC_SECONDS_INTERRUPT, rtc_sec_intr);
	rtc_attach_interrupt(RTC_OVERFLOW_INTERRUPT, rtc_ovf_intr); // expected every 128000 seconds
	rtc_attach_interrupt(RTC_ALARM_GLOBAL_INTERRUPT, rtc_glob_alm_intr);
	rtc_attach_interrupt(RTC_ALARM_SPECIFIC_INTERRUPT, rtc_spec_alm_intr);
}

void loop() {
	int i,n;
	
	Serial.print("Time + interrupt counts: ");
	Serial.print(rtc_get_count());
	Serial.print(".");
	Serial.print(rtc_get_divider());
	Serial.print(" (");
	Serial.print(globSecCnt);
	Serial.print(", ");
	Serial.print(globOvCnt);
	Serial.print(", ");
	Serial.print(globAlmCnt);
	Serial.print(", ");
	Serial.print(specAlmCnt);
	Serial.println(")");

	delay(1000);

	digitalWrite(BOARD_LED_PIN, 1);
	if ((lastSpecAlmCnt != specAlmCnt) || (lastGlobAlmCnt != globAlmCnt)){
		lastGlobAlmCnt = globAlmCnt;
		lastSpecAlmCnt = specAlmCnt;

		Serial.println(" -- alarm -- ");
		for (i=0;i<3;i++) { digitalWrite(BOARD_LED_PIN, 0); delay(100); digitalWrite(BOARD_LED_PIN, 1); delay(100);}
		n = rtc_get_count() + 5;
		rtc_set_alarm(n);
	}
	
	delay(1000);
	digitalWrite(BOARD_LED_PIN, 0);
}


