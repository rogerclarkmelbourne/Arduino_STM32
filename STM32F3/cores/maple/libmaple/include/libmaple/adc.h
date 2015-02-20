/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/include/libmaple/adc.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>,
 *				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief Analog-to-Digital Conversion (ADC) header.
 */

#ifndef _LIBMAPLE_ADC_H_
#define _LIBMAPLE_ADC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple.h>
#include <libmaple/bitband.h>
#include <libmaple/rcc.h>

/**
 * TODO document
 */
typedef struct adc_callback_data {
	uint32 irq_flags;	
	void *arg;
} adc_callback_data;

/**
 * TODO document
 */
typedef struct adc_private_data {
    void (*handler)(adc_callback_data*);
    uint32 handler_flags;
    adc_callback_data cb_data;
} adc_private_data;

/* Pull in the series header
 *
 * IMPORTANT: The series header must define the following:
 *
 * - enum adc_extsel_event (and typedef to adc_extsel_event): One per
 *   external event used to trigger start of conversion of a regular
 *   group. If two different series support the same event as a
 *   trigger, they must use the same token for the enumerator for that
 *   event. (The value of the enumerator is of course allowed to be
 *   different).
 *
 * - enum adc_smp_rate (and typedef to adc_smp_rate): One per
 *   available sampling time.  These must be in the form ADC_SMPR_X_Y
 *   for X.Y cycles (e.g. ADC_SMPR_1_5 means 1.5 cycles), or
 *   ADC_SMPR_X for X cycles (e.g. ADC_SMPR_3 means 3 cycles).
 *
 * - enum adc_prescaler (and typedef): One per available prescaler,
 *   suitable for adc_set_prescaler. Series which have the same
 *   prescaler dividers (e.g. STM32F1 and STM32F2 both divide PCLK2 by
 *   2, 4, 6, or 8) must provide the same tokens as enumerators, for
 *   portability.
 */

#include <series/adc.h>

/*
 * Routines
 */

extern void adc_init(const adc_dev *dev);

/**
 * @brief Set external event select for regular group
 * @param dev ADC device
 * @param event Event used to trigger the start of conversion.
 * @see adc_extsel_event
 */
extern void adc_set_extsel(const adc_dev *dev, adc_extsel_event event);

/**
 * @brief Set the sample rate for all channels on an ADC device.
 *
 * Don't call this during conversion.
 *
 * @param dev adc device
 * @param smp_rate sample rate to set
 * @see adc_smp_rate
 */
extern void adc_set_sample_rate(const adc_dev *dev, adc_smp_rate smp_rate);

/**
 * @brief Enable scan mode for an ADC.
 *
 * In scan mode, the ADC converts all channels in a regular or
 * injected sequence. After each conversion is done, the ADC
 * automatically starts converting the next channel in the sequence.
 *
 * Scan mode is disabled by default.
 *
 * @see adc_disable_scan()
 */
extern void adc_enable_scan(const adc_dev *dev);

/**
 * @brief Disable scan mode for an ADC.
 *
 * This is the default setting.
 *
 * @see adc_enable_scan()
 */
extern void adc_disable_scan(const adc_dev *dev);

/**
 * @brief Enable continuous mode for an ADC.
 *
 * In this mode, the ADC will automatically perform conversions
 * continuously until taken out of this mode or disabled.
 *
 * Continuous mode is disabled by default.
 *
 * @see adc_disable_continuous()
 */
extern void adc_enable_continuous(const adc_dev *dev);

/**
 * @brief Disable continuous mode for an ADC.
 *
 * This is the default setting.
 *
 * @see adc_enable_continuous()
 */
extern void adc_disable_continuous(const adc_dev *dev);

/**
 * @brief Set the sequence of channels to convert.
 *
 * This sets the (regular) sequence of up to 16 channels to convert.
 *
 * @param dev ADC device
 * @param channels ADC channels to convert; these can repeat and may
 *                 be in any order.
 * @param len Length of `channels', from 1 to 16.
 * @see adc_start_conv()
 */
extern void adc_set_conv_seq(const adc_dev *dev, const uint8 *channels, uint8 len);

/**
 * @brief Attach an interrupt handler and enable its interrupts.
 *
 * This function sets `handler' as the function to be called when any
 * ADC interrupts for `dev' occur. At most one ADC interrupt handler
 * may be attached at any time. Subsequent calls to this function will
 * overwrite any previously attached handler.
 *
 * When `handler' is called, its argument will point to a struct
 * adc_callback_data. The .irq_flags field in this struct will be a
 * logical OR of adc_interrupt_id values encoding the reason(s) for
 * the call. Its .arg field will be the `arg' argument to this
 * function.
 *
 * The interrupt bits set in the adc_callback_data's .irq_flags will
 * always be a subset of those set in the `interrupt_flags' argument
 * to this function. That is, interrupts not given here in the
 * `interrupt_flags' argument will never cause `handler' to be
 * called. This has the effect that any enabled ADC interrupts not
 * specified in `interrupt_flags' will be ignored.
 *
 * This function additionally enables the ADC interrupts specified by
 * `interrupt_flags'.
 *
 * @param dev ADC device whose interrupts to attach to.
 * @param interrupt_flags Logical or of adc_interrupt_id values
 *                        specifying interrupts to enable.
 * @param handler Interrupt handler to call when any interrupt in
 *                interrupt_flags occurs.
 * @param arg Value to store in .arg field of handler's callback data.
 * @see enum adc_interrupt_id
 * @see struct adc_callback_data
 */
extern void adc_attach_interrupt(const adc_dev *dev, uint32 interrupt_flags,
																void (*handler)(adc_callback_data*), void *arg);

/**
 * @brief Disable ADC interrupts and detach interrupt handlers.
 *
 * This function disables all interrupts for `dev', and unsets any
 * handler attached with adc_attach_interrupt().
 *
 * @param dev ADC device whose handler to detach.
 */
extern void adc_detach_interrupt(const adc_dev *dev);

/**
 * @brief Enable ADC interrupts
 * @param dev ADC device
 * @param interrupt_flags Logical or of adc_interrupt_id values to enable.
 * @see adc_disable_interrupt()
 * @see adc_attach_interrupt()
 */
extern void adc_enable_interrupts(const adc_dev *dev, uint32 interrupt_flags);

/**
 * @brief Disable ADC interrupts.
 * @param dev ADC device
 * @param interrupt_flags Logical or of adc_interrupt_id values to disable.
 * @brief adc_enable_interrupt()
 */
extern void adc_disable_interrupts(const adc_dev *dev, uint32 interrupt_flags);

/**
 * @brief Perform a single synchronous software triggered conversion on a
 *        channel.
 * @param dev ADC device to use for reading.
 * @param channel channel to convert
 * @return conversion result
 */
extern uint16 adc_read(const adc_dev *dev, uint8 channel);

/**
 * @brief Set the ADC prescaler.
 *
 * This determines the ADC clock for all devices.
 */
extern void adc_set_prescaler(adc_prescaler pre);

/**
 * @brief Call a function on all ADC devices.
 * @param fn Function to call on each ADC device.
 */
extern void adc_foreach(void (*fn)(const adc_dev*));

struct gpio_dev;
/**
 * @brief Configure a GPIO pin for ADC conversion.
 * @param dev ADC device to use for conversion (currently ignored on
 *            all targets).
 * @param gdev GPIO device to configure.
 * @param bit Bit on gdev to configure for ADC conversion.
 */
extern void adc_config_gpio(const struct adc_dev *dev,
                            struct gpio_dev *gdev,
                            uint8 bit);

/**
 * @brief Enable an ADC and configure it for single conversion mode.
 *
 * This function performs any initialization necessary to allow the
 * ADC device to perform a single synchronous regular software
 * triggered conversion, using adc_read().
 *
 * @param dev Device to enable.
 * @see adc_read()
 */
extern void adc_enable_single_swstart(const adc_dev* dev);

/**
 * @brief Set the regular channel sequence length.
 *
 * Defines the total number of conversions in the regular channel
 * conversion sequence.
 *
 * @param dev ADC device.
 * @param length Regular channel sequence length, from 1 to 16.
 */
extern void adc_set_reg_seqlen(const adc_dev *dev, uint8 length);

/**
 * @brief Enable an adc peripheral
 * @param dev ADC device to enable
 */
extern void adc_enable(const adc_dev *dev);

/**
 * @brief Disable an ADC peripheral
 * @param dev ADC device to disable
 */
extern void adc_disable(const adc_dev *dev);

/**
 * @brief Disable all ADC peripherals.
 */
static inline void adc_disable_all(void) {
    adc_foreach(adc_disable);
}

/*
 * private
 */
extern void _adc_enable_dev_irq(const adc_dev *dev);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
