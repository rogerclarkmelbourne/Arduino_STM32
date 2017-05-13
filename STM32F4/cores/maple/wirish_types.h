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
 * @file   wirish_types.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Wirish library type definitions.
 */

#ifndef _WIRISH_TYPES_H_
#define _WIRISH_TYPES_H_

#include <libmaple/gpio_def.h>
#include <libmaple/timer.h>
#include <libmaple/adc.h>

/**
 * Invalid stm32_pin_info adc_channel value.
 * @see stm32_pin_info
 */
#define ADCx 0xFF

/**
 * @brief Stores STM32-specific information related to a given Maple pin.
 * @see PIN_MAP
 */
#ifdef BOARD_generic_f407v
// restructure members to build consecutive pairs
typedef struct stm32_pin_info {
    const gpio_dev * gpio_device;      /**< Maple pin's GPIO device */
    timer_dev * timer_device;    /**< Pin's timer device, if any. */
    uint8 timer_channel;        /**< Timer channel, or 0 if none. */
    uint8 adc_channel;          /**< Pin ADC channel, or ADCx if none. */
    const adc_dev *adc_device;  /**< ADC device, if any. */
} stm32_pin_info;

#else

typedef struct stm32_pin_info {
    const gpio_dev *gpio_device;      /**< Maple pin's GPIO device */
    timer_dev *timer_device;    /**< Pin's timer device, if any. */
    const adc_dev *adc_device;  /**< ADC device, if any. */
    uint8 timer_channel;        /**< Timer channel, or 0 if none. */
    uint8 adc_channel;          /**< Pin ADC channel, or ADCx if none. */
} stm32_pin_info;

#endif

/**
 * Variable attribute, instructs the linker to place the marked
 * variable in Flash instead of RAM. */
#define __FLASH__ __attr_flash

#endif
