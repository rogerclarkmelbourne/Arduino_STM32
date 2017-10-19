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

/* 72 MHz -> 72 cycles per microsecond. */
#define CYCLES_PER_MICROSECOND	(F_CPU / 1000000U)

/* Roger clark

Removed led pin and button pin defines as these are not Arduino API defines
On the Maple the led is on pin 13 and the button is on pin 38
*/


/* Number of USARTs/UARTs whose pins are broken out to headers. */
#define BOARD_NR_USARTS         3

/* USART pin numbers. */
#define BOARD_USART1_TX_PIN     7
#define BOARD_USART1_RX_PIN     8
#define BOARD_USART2_TX_PIN     1
#define BOARD_USART2_RX_PIN     0
#define BOARD_USART3_TX_PIN     29
#define BOARD_USART3_RX_PIN     30

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
#define BOARD_NR_GPIO_PINS      44

/* Number of pins capable of PWM output. */
#define BOARD_NR_PWM_PINS       15

/* Number of pins capable of ADC conversion. */
#define BOARD_NR_ADC_PINS       15

/* Number of pins already connected to external hardware.  For Maple,
 * these are just led pin , button , and the debug port
 * pins (see below). */
#define BOARD_NR_USED_PINS       7

/* Debug port pins. */
#define BOARD_JTMS_SWDIO_PIN    39
#define BOARD_JTCK_SWCLK_PIN    40
#define BOARD_JTDI_PIN          41
#define BOARD_JTDO_PIN          42
#define BOARD_NJTRST_PIN        43

/* USB configuration.  BOARD_USB_DISC_DEV is the GPIO port containing
 * the USB_DISC pin, and BOARD_USB_DISC_BIT is that pin's bit. */
#define BOARD_USB_DISC_DEV      GPIOC
#define BOARD_USB_DISC_BIT      12

/* Pin aliases: these give the GPIO port/bit for each pin as an
 * enum. These are optional, but recommended. They make it easier to
 * write code using low-level GPIO functionality. */
enum {
    PA3, PA2, PA0, PA1, PB5, PB6, PA8, PA9, PA10, PB7, PA4, PA7, PA6, PA5, PB8,
    PC0, PC1, PC2, PC3, PC4, PC5, PC13, PC14, PC15, PB9, PD2, PC10, PB0, PB1,
    PB10, PB11, PB12, PB13, PB14, PB15, PC6, PC7, PC8, PC9, PA13, PA14, PA15,
    PB3, PB4
};

#endif
