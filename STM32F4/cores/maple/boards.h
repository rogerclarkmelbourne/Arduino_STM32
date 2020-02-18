/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
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
 * @file boards.h
 * @author Bryan Newbold <bnewbold@leaflabs.com>,
 *         Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Board-specific pin information.
 *
 * To add a new board type, add a new pair of files to
 * /wirish/boards/, update the section below with a new "BOARD" type,
 * and update /wirish/rules.mk to include your boards/your_board.cpp
 * file in the top-level Makefile build.
 */

#ifndef _BOARDS_H_
#define _BOARDS_H_


#include <libmaple/libmaple_types.h>
#include <stdbool.h>



/**
 * @brief Pins capable of PWM output.
 *
 * Its length is BOARD_NR_PWM_PINS.
 */
extern const uint8 boardPWMPins[];

/**
 * @brief Array of pins capable of analog input.
 *
 * Its length is BOARD_NR_ADC_PINS.
 */
extern const uint8 boardADCPins[];

/**
 * @brief Pins which are connected to external hardware.
 *
 * For example, on Maple boards, it always at least includes
 * BOARD_LED_PIN.  Its length is BOARD_NR_USED_PINS.
 */
extern const uint8 boardUsedPins[];

/**
 * @brief Generic board initialization function.
 *
 * This function is called before main().  It ensures that the clocks
 * and peripherals are configured properly for use with wirish, then
 * calls boardInit().
 *
 * @see boardInit()
 */
void init(void);

/**
 * @brief Board-specific initialization function.
 *
 * This function is called from init() after all generic board
 * initialization has been performed.  Each board is required to
 * define its own.
 *
 * @see init()
 */
extern void boardInit(void);

/**
 * @brief Test if a pin is used for a special purpose on your board.
 * @param pin Pin to test
 * @return true if the given pin is in boardUsedPins, and false otherwise.
 * @see boardUsedPins
 */
extern bool boardUsesPin(uint8 pin);

/* Include the appropriate private header from boards/: */

/* FIXME HACK put boards/ before these paths once IDE uses make. */

#if defined(VARIANT_discovery_f407)
#include "discovery_f4.h"
#elif defined(VARIANT_blackpill_f401)
#include "blackpill_f401.h"
#elif defined(VARIANT_disco_f411)
#include "disco_f411.h"
#elif defined(VARIANT_generic_f407v)
#include "generic_f407v.h"
#elif defined(VARIANT_arch_max)
#include "arch_max.h"
#else
/*
 * TODO turn this into a warning so people can:
 *
 * #include "my_board_config.h"
 * #include "wirish.h"
 *
 * This will enable third-party board support without requiring that
 * anybody hack around in libmaple itself.
 */
#error "Board type has not been selected correctly."
#endif

/* Set derived definitions */

#define CYCLES_PER_MICROSECOND			CLOCK_SPEED_MHZ                 
#define CLOCK_SPEED_HZ                  (CLOCK_SPEED_MHZ * 1000000UL)

#ifndef BOARD_BUTTON_PRESSED_LEVEL
#define BOARD_BUTTON_PRESSED_LEVEL      HIGH
#endif


#endif
