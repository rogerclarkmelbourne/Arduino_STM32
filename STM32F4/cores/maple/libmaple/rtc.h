
#ifndef _LIBMAPLE_RTC_H_
#define _LIBMAPLE_RTC_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <sys/types.h> // for __time_t_defined
#include <libmaple/libmaple_types.h>
#include <libmaple/exti.h>
#include <ext_interrupts.h>

//#define RTC_DEBUG 1


#if !defined(__time_t_defined) // avoid conflict with newlib or other posix libc
  #warning "Using private time_t definintion"
  typedef uint32_t time_t;
#endif


typedef struct rtc_reg_map {
	__IO uint32 TR;			/**< Time register */
	__IO uint32 DR;			/**< Date register */
	__IO uint32 CR;			/**< Control register */
	__IO uint32 ISR;		/**< Init Status register */
	__IO uint32 PRER;		/**< Prescaler register */
	__IO uint32 WUTR;		/**< Wakeup Timer register */
	__IO uint32 CALIBR;		/**< Calibration register */
	__IO uint32 ALRMAR;		/**< Alarm A register */
	__IO uint32 ALRMBR;		/**< Alarm B register */
	__IO uint32 WPR;		/**< Write Protect register */
	__IO uint32 SSR;		/**< SubSecond register */
	__IO uint32 SHIFTR;		/**< Shift Control register */
	__IO uint32 TSTR;		/**< TimeStamp Time register */
	__IO uint32 TSDR;		/**< TimeStamp Date register */
	__IO uint32 TSSSR;		/**< TimeStamp SubSecond register */
	__IO uint32 CALR;		/**< Calibration register */
	__IO uint32 TAFCR;		/**< Tamper and Alternate Function Config register */
	__IO uint32 ALRMASSR;		/**< Alarm A subSecond register */
	__IO uint32 ALRMBSSR;		/**< Alarm B subSecond register */
	__IO uint32 BKPxR;		/**< Backup registers */
} rtc_reg_map;

/** RTC register map base pointer */
#define RTC        ((struct rtc_reg_map*)0x40002800)

//#define NR_RTC_HANDLERS         4
//voidFuncPtr rtc_handlers[NR_RTC_HANDLERS];     /**< User IRQ handlers */


/**
 * @brief RTC clock source.
 *
 */
typedef enum rtc_clk_src {
	RTCSEL_NONE		= 0,
	RTCSEL_LSE		= 1,
	RTCSEL_LSI		= 2,
	RTCSEL_HSE		= 3,
} rtc_clk_src;

// Time register
#define RTC_TR_PM_BIT     22
#define RTC_TR_HOUR_BIT   16
#define RTC_TR_MINUTE_BIT  8
#define RTC_TR_SECOND_BIT  0

#define RTC_TR_PM_MASK     (0x01)//<<RTC_TR_PM_BIT)
#define RTC_TR_HOUR_MASK   (0x3F)//<<RTC_TR_HOUR_BIT)
#define RTC_TR_MINUTE_MASK (0x7F)//<<RTC_TR_MINUTE_BIT)
#define RTC_TR_SECOND_MASK (0x7F)//<<RTC_TR_SECOND_BIT)

// Date register
#define RTC_DR_YEAR_BIT    16
#define RTC_DR_WEEKDAY_BIT 13
#define RTC_DR_MONTH_BIT    8
#define RTC_DR_DAY_BIT      0

#define RTC_DR_YEAR_MASK    (0xFF)//<<RTC_TR_YEAR_BIT)
#define RTC_DR_WEEKDAY_MASK (0x07)//<<RTC_TR_WEEKDAY_BIT)
#define RTC_DR_MONTH_MASK   (0x1F)//<<RTC_TR_MONTH_BIT)
#define RTC_DR_DAY_MASK     (0x3F)//<<RTC_TR_DAY_BIT)


/* Control Register */
#define RTC_CR_BYPSHAD_BIT      5

#define RTC_CR_TSIE 	    (1<<15)
#define RTC_CR_WUTIE 	    (1<<14)
#define RTC_CR_ALRBIE	    (1<<13)
#define RTC_CR_ALRAIE 	    (1<<12)
#define RTC_CR_TSE 		    (1<<11)
#define RTC_CR_WUTE 	    (1<<10)
#define RTC_CR_ALRBE	    (1<<9)
#define RTC_CR_ALRAE 	    (1<<8)
#define RTC_CR_FMT          (1<<6)
#define RTC_CR_BYPSHAD      (1<<5)
#define RTC_CR_REFCKON      (1<<4)
#define RTC_CR_TSEDGE       (1<<3)
#define RTC_CR_WUCKSEL_MASK (0x07)

/* Initialization and Status Register */
#define RTC_ISR_TAMP2F_BIT     14
#define RTC_ISR_TAMP1F_BIT     13
#define RTC_ISR_TSOVF_BIT      12
#define RTC_ISR_TSF_BIT        11
#define RTC_ISR_WUTF_BIT       10
#define RTC_ISR_ALRBF_BIT       9
#define RTC_ISR_ALRAF_BIT       8
#define RTC_ISR_INIT_BIT        7
#define RTC_ISR_INITF       BIT(6)
#define RTC_ISR_RSF_BIT         5
#define RTC_ISR_INITS       BIT(4)
#define RTC_ISR_SHPF        BIT(3)
#define RTC_ISR_WUTWF       BIT(2)
#define RTC_ISR_ALRBWF      BIT(1)
#define RTC_ISR_ALRAWF      BIT(0)


static inline uint8_t bcd2bin(uint8_t b) { return ( (10*(b>>4)) + (b&0x0F) ); }
static inline uint8_t bin2bcd(uint8_t b) { return ( ((b/10)<<4) + (b%10) ); }

#define BUILD_TIME_REGISTER(h, m, s) ( ( bin2bcd((h&RTC_TR_HOUR_MASK)) << RTC_TR_HOUR_BIT ) | \
                                       ( bin2bcd((m&RTC_TR_MINUTE_MASK)) << RTC_TR_MINUTE_BIT ) | \
								       ( bin2bcd((s&RTC_TR_SECOND_MASK)) << RTC_TR_SECOND_BIT) )

#define BUILD_DATE_REGISTER(y, m, d, wd) ( ( bin2bcd((y&RTC_DR_YEAR_MASK)) << RTC_DR_YEAR_BIT ) | \
                                           ( bin2bcd((m&RTC_DR_MONTH_MASK)) << RTC_DR_MONTH_BIT) | \
									       ( bin2bcd((d&RTC_DR_DAY_MASK)) << RTC_DR_DAY_BIT ) | \
									       ( (wd&RTC_DR_WEEKDAY_MASK) << RTC_DR_WEEKDAY_BIT ) )

typedef struct tm_t {
	uint8_t  year;    // years since 1970
	uint8_t  month;   // month of a year - [ 1 to 12 ]
	uint8_t  day;     // day of a month - [ 1 to 31 ]
	uint8_t  weekday; // day of a week (first day is Monday) - [ 1 to 7 ]
	uint8_t  pm;      // AM: 0, PM: 1
	uint8_t  hour;    // hour of a day - [ 0 to 23 ]
	uint8_t  minute;  // minute of an hour - [ 0 to 59 ]
	uint8_t  second;  // second of a minute - [ 0 to 59 ]
} tm_t;


void _getTime(tm_t *tm);
void _setTime(tm_t *tm);
void _breakTime(time_t epoch_time, tm_t *tm);
time_t _makeTime(tm_t * tm);

//-----------------------------------------------------------------------------
uint8_t _year(uint32_t dr)   ;
uint8_t _month(uint32_t dr)  ;
uint8_t _day(uint32_t dr)    ;
uint8_t _weekday(uint32_t dr);
uint8_t _pm(uint32_t tr)     ;
uint8_t _hour(uint32_t tr)   ;
uint8_t _minute(uint32_t tr) ;
uint8_t _second(uint32_t tr) ;

//-----------------------------------------------------------------------------
// Clear the register synchronized flag. The flag is then set by hardware after a write to PRL/DIV or CNT.
//-----------------------------------------------------------------------------
static inline void rtc_clear_sync() {
	*bb_perip(&RTC->ISR, RTC_ISR_RSF_BIT) = 0;
}

#if 0
//-----------------------------------------------------------------------------
// Check (wait if necessary) to see RTC registers are synchronized.
//-----------------------------------------------------------------------------
void rtc_wait_sync()
{
	PRINTF("> rtc_wait_sync\n");
	uint32 t = millis();
	while ( !(RTC->ISR & BIT(RTC_ISR_RSF_BIT)) ) 
	{
	    if ( (millis()-t)>1500) {
			PRINTF("Sync Timeout ! ISR = %08X\n", RTC->ISR);
			break;
		}
	}
	PRINTF("< rtc_wait_sync\n");
}
#endif

void rtc_enter_config_mode();
//-----------------------------------------------------------------------------
// Exit configuration mode.
//-----------------------------------------------------------------------------
static inline void rtc_exit_config_mode()
{
	*bb_perip(&RTC->ISR, RTC_ISR_INIT_BIT) = 0;
	interrupts();
	//delayMicroseconds(100);
}

extern voidFuncPtr handlerAlarmA;
extern voidFuncPtr handlerAlarmB;
extern voidFuncPtr handlerPeriodicWakeup;

void rtc_enable_alarm_event();
void rtc_enable_wakeup_event();
void rtc_disable_wakeup_event();



#ifdef __cplusplus
}
#endif

#endif // _LIBMAPLE_RTC_H_
