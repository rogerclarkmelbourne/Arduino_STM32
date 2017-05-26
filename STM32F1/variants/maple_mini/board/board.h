/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
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
 * @file   wirish/boards/maple_mini/include/board/board.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Maple Mini board header.
 *
 * See wirish/boards/maple/include/board/board.h for more information
 * on these definitions.
 */

#ifndef _BOARD_MAPLE_MINI_H_
#define _BOARD_MAPLE_MINI_H_

#define CYCLES_PER_MICROSECOND	(F_CPU / 1000000U)
#define SYSTICK_RELOAD_VAL     (F_CPU/1000) - 1 /* takes a cycle to reload */

#define BOARD_NR_USARTS           3
#define BOARD_USART1_TX_PIN       26
#define BOARD_USART1_RX_PIN       25
#define BOARD_USART2_TX_PIN       9
#define BOARD_USART2_RX_PIN       8
#define BOARD_USART3_TX_PIN       1
#define BOARD_USART3_RX_PIN       0

#define BOARD_NR_SPI              2
#define BOARD_SPI1_NSS_PIN        7
#define BOARD_SPI1_MOSI_PIN       4
#define BOARD_SPI1_MISO_PIN       5
#define BOARD_SPI1_SCK_PIN        6
#define BOARD_SPI2_NSS_PIN        31
#define BOARD_SPI2_MOSI_PIN       28
#define BOARD_SPI2_MISO_PIN       29
#define BOARD_SPI2_SCK_PIN        30

#define BOARD_NR_GPIO_PINS        34
#define BOARD_NR_PWM_PINS         12
#define BOARD_NR_ADC_PINS          9
#define BOARD_NR_USED_PINS         4

#define BOARD_JTMS_SWDIO_PIN      22
#define BOARD_JTCK_SWCLK_PIN      21
#define BOARD_JTDI_PIN            20
#define BOARD_JTDO_PIN            19
#define BOARD_NJTRST_PIN          18

#define BOARD_USB_DISC_DEV        GPIOB
#define BOARD_USB_DISC_BIT        9

#define LED_BUILTIN PB1

enum {
    PB11, PB10, PB2, PB0, PA7, PA6, PA5, PA4, PA3, PA2, PA1, PA0, PC15, PC14,
    PC13, PB7, PB6, PB5, PB4, PB3, PA15, PA14, PA13, PA12, PA11, PA10, PA9,
    PA8, PB15, PB14, PB13, PB12, PB8, PB1
};

#endif
