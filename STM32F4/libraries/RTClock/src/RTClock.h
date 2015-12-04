#include <libmaple/rcc.h>
#include <libmaple/bitband.h>
#include <libmaple/pwr.h>
#include <libmaple/bkp.h>
#include <libmaple/nvic.h>
#include <libmaple/exti.h>
#include <time.h>


#ifndef _RTCLOCK_H_
#define _RTCLOCK_H_

//#define RTC_DEBUG

#ifdef __cplusplus
extern "C" {
#endif
#ifdef RTC_DEBUG
        extern void dbg_printf(const char *fmt, ... );
        #define rtc_debug_printf(fmt, ...) dbg_printf(fmt, ##__VA_ARGS__);
#else
        #define rtc_debug_printf(...) 
#endif
#ifdef __cplusplus
}
#endif


#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define DAYS_PER_WEEK (7UL)
#define SECS_PER_WEEK (SECS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_WEEK * 52UL)
#define SECS_YR_2000  (946684800UL) // the time at the start of y2k
#define LEAP_YEAR(Y)     ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )

static  const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0


typedef struct rtc_reg_map {
	__io uint32 TR;			/**< Time register */
	__io uint32 DR;			/**< Date register */
	__io uint32 CR;			/**< Control register */
	__io uint32 ISR;		/**< Init Status register */
	__io uint32 PRER;		/**< Prescaler register */
	__io uint32 WUTR;		/**< Wakeup Timer register */
	__io uint32 CALIBR;		/**< Calibration register */
	__io uint32 ALRMAR;		/**< Alarm A register */
	__io uint32 ALRMBR;		/**< Alarm B register */
	__io uint32 WPR;		/**< Write Protect register */
	__io uint32 SSR;		/**< SubSecond register */
	__io uint32 SHIFTR;		/**< Shift Control register */
	__io uint32 TSTR;		/**< TimeStamp Time register */
	__io uint32 TSDR;		/**< TimeStamp Date register */
	__io uint32 TSSSR;		/**< TimeStamp SubSecond register */
	__io uint32 CALR;		/**< Calibration register */
	__io uint32 TAFCR;		/**< Tamper and Alternate Function Config register */
	__io uint32 ALRMASSR;		/**< Alarm A subSecond register */
	__io uint32 ALRMBSSR;		/**< Alarm B subSecond register */
	__io uint32 BKPxR;		/**< Backup registers */
} rtc_reg_map;
		
/** RTC register map base pointer */
#define RTC_BASE        ((struct rtc_reg_map*)0x40002800)

/** rtc device type */
typedef struct rtc_dev {
	rtc_reg_map *regs;			/**< Register map */
	voidFuncPtr handlers[];     /**< User IRQ handlers */
} rtc_dev;

extern rtc_dev *RTC;

#define NR_RTC_HANDLERS                 4

#define EXTI_RTC_ALARM_BIT		17
#define EXTI_RTC_WAKEUP_BIT		22
#define EXTI_RTC_TAMPSTAMP_BIT		21


/**
 * @brief RTC clock source.
 *
 */
typedef enum rtc_clk_src {
	RTCSEL_DEFAULT	= 0,
	RTCSEL_NONE		= 0x10,
	RTCSEL_LSE		= 0x11,
	RTCSEL_LSI		= 0x12,
	RTCSEL_HSE		= 0x13,
} rtc_clk_src;


/* Control Register */
#define RTC_CR_TSIE_BIT 	15
#define RTC_CR_WUTIE_BIT 	14
#define RTC_CR_ALRBIE_BIT	13
#define RTC_CR_ALRAIE_BIT 	12
#define RTC_CR_TSE_BIT 		11
#define RTC_CR_WUTE_BIT 	10
#define RTC_CR_ALRBE_BIT	9
#define RTC_CR_ALRAE_BIT 	8

/* Initialization and Status Register */
#define RTC_ISR_TSOVF_BIT 	12
#define RTC_ISR_TSF_BIT 	11
#define RTC_ISR_WUTF_BIT 	10
#define RTC_ISR_ALRBF_BIT 	9
#define RTC_ISR_ALRAF_BIT 	8
#define RTC_ISR_INIT_BIT 	7
#define RTC_ISR_INITF_BIT 	6
#define RTC_ISR_RSF_BIT 	5
#define RTC_ISR_INITS_BIT 	4
#define RTC_ISR_SHPF_BIT 	3
#define RTC_ISR_WUTWF_BIT 	2
#define RTC_ISR_ALRBWF_BIT 	1
#define RTC_ISR_ALRAWF_BIT 	0



class RTClock {
 public:
 	RTClock();
        RTClock(rtc_clk_src src ); 
	RTClock(rtc_clk_src src, uint16 sync_prescaler, uint16 async_prescaler);
	//~RTClock(); //to implement

	void setTime (time_t time_stamp);
	void setTime (struct tm * tm_ptr); 
	
	struct tm* getTime(struct tm* tm_ptr); 
	time_t getTime();
	
	void setAlarmATime (tm * tm_ptr, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false);
	void setAlarmATime (time_t alarm_time, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false); 
	void turnOffAlarmA();
	void setAlarmBTime (tm * tm_ptr, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false);
	void setAlarmBTime (time_t alarm_time, bool hours_match = true, bool mins_match = true, bool secs_match = true, bool date_match = false); 
	void turnOffAlarmB();
	
	void setPeriodicWakeup(uint16 period);

	void attachPeriodicWakeupInterrupt(voidFuncPtr function); 
	void detachPeriodicWakeupInterrupt();

	void attachAlarmAInterrupt(voidFuncPtr function); 
	void detachAlarmAInterrupt();
	void attachAlarmBInterrupt(voidFuncPtr function); 
	void detachAlarmBInterrupt();

 //private:

} ;



/**
 * @brief Clear the register synchronized flag. The flag is then set by hardware after a write to PRL/DIV or CNT.
 */
static inline void rtc_clear_sync() {
	*bb_perip(&(RTC->regs)->ISR, RTC_ISR_RSF_BIT) = 0;
}

/**
 * @brief Check (wait if necessary) to see RTC registers are synchronized.
 */
static inline void rtc_wait_sync() {
        uint32 t = 0;
	rtc_debug_printf("entering rtc_wait_sync\r\n");
	while (*bb_perip(&(RTC->regs)->ISR, RTC_ISR_RSF_BIT) == 0) {
	    if (++t > 1000000) {
                rtc_debug_printf("RTC_BASE->ISR.RSF Timeout !\r\n");
                rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
                return;
            }
	}
	rtc_debug_printf("finished rtc_wait_sync\r\n");
}

/**
 * @brief Enter configuration mode.
 */
static inline void rtc_enter_config_mode() {
        uint32 t = 0;
	// Unlock Write Protect
        RTC_BASE->WPR = 0xCA;
        RTC_BASE->WPR = 0x53;
	*bb_perip(&(RTC->regs)->ISR, RTC_ISR_INIT_BIT) = 1;
	rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
	while (!(RTC_BASE->ISR & 0x00000040)) {
	    if (++t > 1000000) {
                rtc_debug_printf("RTC_BASE->ISR.INITF Timeout !\r\n");
                rtc_debug_printf("RTC_BASE->ISR = %08X\r\n", RTC_BASE->ISR);
                return;
            }
	}
	rtc_debug_printf("rtc_enter_config_mode done !\r\n");
}

/**
 * @brief Exit configuration mode.
 */
static inline void rtc_exit_config_mode() {
	*bb_perip(&(RTC->regs)->ISR, RTC_ISR_INIT_BIT) = 0;
	rtc_debug_printf("rtc_exit_config_mode done !\r\n");
}


/**
 * @brief Enable an RTC alarm event. Enabling this event allows waking up from deep sleep via WFE.
 * @see rtc_interrupt_id
 */
static inline void rtc_enable_alarm_event() {
        *bb_perip(&EXTI_BASE->IMR, EXTI_RTC_ALARM_BIT) = 1;
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_ALARM_BIT) = 1;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 1;
}

/**
 * @brief Disable the RTC alarm event.
 * @see rtc_interrupt_id
 */
static inline void rtc_disable_alarm_event() {
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_ALARM_BIT) = 0;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 0;
}


/**
 * @brief Enable an RTC Wakeup event. 
 * @see rtc_interrupt_id
 */
static inline void rtc_enable_wakeup_event() {
        *bb_perip(&EXTI_BASE->IMR, EXTI_RTC_WAKEUP_BIT) = 1;
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_WAKEUP_BIT) = 1;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_WAKEUP_BIT) = 1;
}

/**
 * @brief Disable the RTC alarm event.
 * @see rtc_interrupt_id
 */
static inline void rtc_disable_wakeup_event() {
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_WAKEUP_BIT) = 0;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_WAKEUP_BIT) = 0;
}



#endif // _RTCLOCK_H_
                                                                                        