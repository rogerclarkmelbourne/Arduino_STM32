/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file   wirish/boards/48F3/include/board/board.h
 * @author F3-port: Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief  F303xx board header (F303CB, F303CC, F303RB, F303RC, F303VB, F303VC).
 *
 * See wirish/boards/maple/include/board/board.h for more information
 * on these definitions.
 */

#ifndef _BOARD_F303xx_H_
#define _BOARD_F303xx_H_

#include <libmaple/stm32.h>

#define CYCLES_PER_MICROSECOND	(F_CPU / 1000000U)
#define SYSTICK_RELOAD_VAL     (F_CPU/1000) - 1 /* takes a cycle to reload */

enum {
		PC13, PC14, PC15,
		PF0, PF1,
		PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
		PB0, PB1, PB2,
		PB10, PB11, PB12, PB13, PB14, PB15,
		PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
		PB3, PB4, PB5, PB6, PB7, PB8, PB9,
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
		PC0, PC1, PC2, PC3,
		PF4,
		PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12,
		PD2,
#	if defined(STM32_XL_DENSITY)
		PE2, PE3, PE4, PE5, PE6,
		PF9, PF10,
		PF2,
		PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
		PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
		PF6,
		PD0, PD1,
		PD3, PD4, PD5, PD6, PD7,
		PE0, PE1,
#	endif
#endif
};

#define BOARD_USART1_TX_PIN       PA9		/* also PB6 */
#define BOARD_USART1_RX_PIN       PA10	/* also PB7 */

#define BOARD_USART2_TX_PIN       PA2		/* also PA14, PB3 */
#define BOARD_USART2_RX_PIN       PA3		/* also PA15, PB4 */

#define BOARD_USART3_TX_PIN       PB10
#define BOARD_USART3_RX_PIN       PB11

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
# define BOARD_UART4_TX_PIN				PC10
# define BOARD_UART4_RX_PIN				PC11

# define BOARD_UART5_TX_PIN				PC12
# define BOARD_UART5_RX_PIN				PD2
#endif

#define BOARD_NR_SPI              3

#define BOARD_SPI1_NSS_PIN        PA4
#define BOARD_SPI1_SCK_PIN        PA5
#define BOARD_SPI1_MISO_PIN       PA6
#define BOARD_SPI1_MOSI_PIN       PA7

#define BOARD_SPI2_NSS_PIN        PB12
#define BOARD_SPI2_SCK_PIN        PB13
#define BOARD_SPI2_MISO_PIN       PB14
#define BOARD_SPI2_MOSI_PIN       PB15

#define BOARD_SPI3_NSS_PIN        PA15
#define BOARD_SPI3_SCK_PIN        PB3
#define BOARD_SPI3_MISO_PIN       PB4
#define BOARD_SPI3_MOSI_PIN       PB5

#define BOARD_JTMS_SWDIO_PIN      PA13
#define BOARD_JTCK_SWCLK_PIN      PA14
#define BOARD_JTDI_PIN            PA15
#define BOARD_JTDO_PIN            PB3
#define BOARD_NJTRST_PIN          PB4

#if defined(STM32_MEDIUM_DENSITY)
# define BOARD_NR_USARTS           3
#	define BOARD_NR_GPIO_PINS        37
#	define BOARD_NR_PWM_PINS         28
#	define BOARD_NR_ADC_PINS         15
#	define BOARD_NR_USED_PINS         4
#elif defined(STM32_HIGH_DENSITY)
# define BOARD_NR_USARTS           5
#	define BOARD_NR_GPIO_PINS        52
#	define BOARD_NR_PWM_PINS         32
#	define BOARD_NR_ADC_PINS         22
#	define BOARD_NR_USED_PINS         4
#elif defined(STM32_XL_DENSITY)
# define BOARD_NR_USARTS           5
#	define BOARD_NR_GPIO_PINS        87
#	define BOARD_NR_PWM_PINS         53
#	define BOARD_NR_ADC_PINS         39
#	define BOARD_NR_USED_PINS         7
#endif

/* redefine the following ones to match your hardware design */
#define BOARD_BUTTON_PIN          PA0
#define BOARD_LED_PIN             PE10

#define BOARD_USB_DISC_DEV        GPIOA
#define BOARD_USB_DISC_BIT        13

#endif
