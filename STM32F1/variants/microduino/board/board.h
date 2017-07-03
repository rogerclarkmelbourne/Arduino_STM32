/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 * Modifications Copyright (c) 2014 Ian Harvey
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
 * @file   variants/microduino/board/board.h
 * @author Ian Harvey
 * @brief  Private include file for Microduino STM32
 *
 * See wirish/boards/maple/include/board/board.h for more information
 * on these definitions.
 */

#ifndef _BOARD_MICRODUINO_STM32_H_
#define _BOARD_MICRODUINO_STM32_H_

#define CYCLES_PER_MICROSECOND	(F_CPU / 1000000U)
#define SYSTICK_RELOAD_VAL     (F_CPU/1000) - 1 /* takes a cycle to reload */

/* Roger Clark. 

Removed the definitions for led pin 4 and button pin 24
*/

#define BOARD_NR_USARTS           3
#define BOARD_USART1_TX_PIN       1
#define BOARD_USART1_RX_PIN       0
#define BOARD_USART2_TX_PIN       16
#define BOARD_USART2_RX_PIN       17
#define BOARD_USART3_TX_PIN       3
#define BOARD_USART3_RX_PIN       2

#define BOARD_NR_SPI              1
#define BOARD_SPI1_NSS_PIN        10
#define BOARD_SPI1_MOSI_PIN       11
#define BOARD_SPI1_MISO_PIN       12
#define BOARD_SPI1_SCK_PIN        13

#define BOARD_NR_GPIO_PINS        25
#define BOARD_NR_PWM_PINS         14
#define BOARD_NR_ADC_PINS         12
#define BOARD_NR_USED_PINS         4

#define BOARD_JTMS_SWDIO_PIN      5
#define BOARD_JTCK_SWCLK_PIN      6
#define BOARD_JTDI_PIN            7
#define BOARD_JTDO_PIN            8
#define BOARD_NJTRST_PIN          9

#define BOARD_USB_DISC_DEV        GPIOB // FIXME
#define BOARD_USB_DISC_BIT        9  // FIXME

enum {
    PA10, PA9, PB11, PB10, PA8, PA13, PA14, PA15,
    PB3, PB4, PA4, PA7, PA6, PA5,
    
    PA0, PA1, PA2, PA3, PB7, PB6, PB0,
    PA12, PA11 //FIXME - button pin
};

#endif
