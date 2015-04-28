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
 * @file   wirish/boards/maple/include/board/board.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Maple board header.
 */

#ifndef _BOARD_MAPLE_H_
#define _BOARD_MAPLE_H_

#define CYCLES_PER_MICROSECOND  72
#define SYSTICK_RELOAD_VAL     71999 /* takes a cycle to reload */

/* Roger clark. Removed defines for LED pin and Button pin as they are not Arduino API defines */
// Pin number for the built-in button
//BOARD _ BUTTON _ PIN        27

// Pin number for the built-in LED.
//BOARD _ LED _ PIN           13

/* Number of USARTs/UARTs whose pins are broken out to headers. */
#define BOARD_NR_USARTS         3

#define BOARD_USART1_TX_PIN     PA9
#define BOARD_USART1_RX_PIN     PA10

#define BOARD_USART2_TX_PIN     PA2
#define BOARD_USART2_RX_PIN     PA3

#define BOARD_USART3_TX_PIN     PC10
#define BOARD_USART3_RX_PIN     PC11

/* Number of SPI ports broken out to headers. */
#define BOARD_NR_SPI            2

/* SPI pin numbers. */
#define BOARD_SPI1_NSS_PIN      10
#define BOARD_SPI1_MOSI_PIN     11
#define BOARD_SPI1_MISO_PIN     12
#define BOARD_SPI1_SCK_PIN      13
#define BOARD_SPI2_NSS_PIN      31
#define BOARD_SPI2_MOSI_PIN     34
#define BOARD_SPI2_MISO_PIN     33
#define BOARD_SPI2_SCK_PIN      32

/* Total number of GPIO pins that are broken out to headers and
 * intended for use. This includes pins like the LED, button, and
 * debug port (JTAG/SWD) pins. */
#define BOARD_NR_GPIO_PINS      46

/* Number of pins capable of PWM output. */
#define BOARD_NR_PWM_PINS       14

/* Number of pins capable of ADC conversion. */
#define BOARD_NR_ADC_PINS       15

/* Number of pins already connected to external hardware.  For Maple,
 * these are just the debug port
 * pins (see below). */
#define BOARD_NR_USED_PINS       2

/**
 * Note: there is no USB in this board.
 */

/* Pin aliases: these give the GPIO port/bit for each pin as an
 * enum. These are optional, but recommended. They make it easier to
 * write code using low-level GPIO functionality. */
enum {
    PA3, PA2, PA10, PB3, PB5, PB4, PB10, PA8,
    PA9, PC7, PB6, PA7, PA6, PA5, PB9,PB8,
    PA0, PA1, PA4, PB0, PC1, PC0,
    PC10,PC12,PB7,PC13,PC14,PC15,PC2,PC3,PC11,PD2,PC9,PC8,PC6,PC5,PA12,PA11,PB12,PB11,
    PB2,PB1,PB15,PB14,PB13
};

#endif
