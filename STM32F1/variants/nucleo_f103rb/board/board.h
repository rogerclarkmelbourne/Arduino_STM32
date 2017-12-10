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
 * @brief  Maple board header. */
 // edited and rewritten for STM32DUINO by Matthias Diro 07.05.2015

#ifndef _BOARD_MAPLE_H_
#define _BOARD_MAPLE_H_
#define _BOARD_NUCLEOF103RB_ 1 // hack for HardwareSerial.cpp for a new order of serials
#define CYCLES_PER_MICROSECOND	(F_CPU / 1000000U)
#define SYSTICK_RELOAD_VAL     (F_CPU/1000) - 1 /* takes a cycle to reload */

/* Roger clark. Removed defines for LED pin and Button pin as they are not Arduino API defines */
// Pin number for the built-in button
//BOARD _ BUTTON _ PIN        27

// Pin number for the built-in LED.
//BOARD _ LED _ PIN           13

// Arduino compatible A0-A5 defines:
#define A0 16
#define A1 17
#define A2 18
#define A3 19
#define A4 20
#define A5 21
// Wire.h Soft I2C standard pins for custom boards:
 #define BOARD_I2CSOFT_SCL_PIN     PC0
 #define BOARD_I2CSOFT_SDA_PIN     PC1

#define BOARD_NR_USARTS         3
//#define SERIAL_ORDER1     3 // HW Serial order for custom boards
//#define SERIAL_ORDER2      2 // not implemented yet
//#define SERIAL_ORDER3      1

/* Number of USARTs/UARTs whose pins are broken out to headers. */
// hope this would be obsolet soon
#define BOARD_USART1_TX_PIN     PA9
#define BOARD_USART1_RX_PIN     PA10

#define BOARD_USART2_TX_PIN     PA2
#define BOARD_USART2_RX_PIN     PA3

#define BOARD_USART3_TX_PIN     PC10
#define BOARD_USART3_RX_PIN     PC11

/* Number of SPI ports broken out to headers. */
#define BOARD_NR_SPI            2

/* SPI pin numbers. */
#define BOARD_SPI1_NSS_PIN      PA4
#define BOARD_SPI1_MOSI_PIN     PA7
#define BOARD_SPI1_MISO_PIN     PA6
#define BOARD_SPI1_SCK_PIN      PA5
#define BOARD_SPI2_NSS_PIN      PB12
#define BOARD_SPI2_MOSI_PIN     PB15
#define BOARD_SPI2_MISO_PIN     PB14
#define BOARD_SPI2_SCK_PIN      PB13

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
#define BOARD_USB_DISC_DEV        NULL
#define BOARD_USB_DISC_BIT        NULL

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
