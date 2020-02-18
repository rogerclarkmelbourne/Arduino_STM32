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
 *  @brief Wirish timer class.
 */

#ifndef _HARDWARETIMER_H_
#define _HARDWARETIMER_H_

// TODO [0.1.0] Remove deprecated pieces, pick a better API

#include "libmaple/timer.h"
#include "wirish_math.h"

/** Timer mode. */
typedef timer_mode TimerMode;


/**
 * @brief Interface to one of the 16-bit timer peripherals.
 */
class HardwareTimer
{
private:
    const timer_dev *dev;

public:
    /**
     * @brief Construct a new HardwareTimer instance.
     * @param timerNum number of the timer to control.
     */
    HardwareTimer(uint8 timerNum);
	void init(void) { timer_init(this->dev); timer_pause(this->dev); }


    /**
     * @brief Stop the counter, without affecting its configuration.
     *
     * @see HardwareTimer::resume()
     */
    void pause(void) { timer_pause(this->dev); }

    /**
     * @brief Resume a paused timer, without affecting its configuration.
     *
     * The timer will resume counting and firing interrupts as
     * appropriate.
     *
     * Note that there is some function call overhead associated with
     * using this method, so using it in concert with
     * HardwareTimer::pause() is not a robust way to align multiple
     * timers to the same count value.
     *
     * @see HardwareTimer::pause()
     */
    void resume(void) { timer_resume(this->dev); }

    /**
     * @brief Get the timer's clock speed.
     * @return Timer input clock speed in Hz/second
     */
    uint32 getClockSpeed(void) {
        return rcc_dev_timer_clk_speed(this->dev->clk_id);
    }

    /**
     * @brief Get the timer's prescale factor.
     * @return Timer prescaler, from 1 to 65,536.
     * @see HardwareTimer::setPrescaleFactor()
     */
    uint32 getPrescaleFactor() { return timer_get_prescaler(this->dev) + 1; }

    /**
     * @brief Set the timer's prescale factor.
     *
     * The new value won't take effect until the next time the counter
     * overflows.  You can force the counter to reset using
     * HardwareTimer::refresh().
     *
     * @param factor The new prescale value to set, from 1 to 65,536.
     * @see HardwareTimer::refresh()
     */
    void setPrescaleFactor(uint32 factor) { timer_set_prescaler(this->dev, (uint16)(factor - 1)); }

    /**
     * @brief Get the timer overflow value.
     * @see HardwareTimer::setOverflow()
     */
    uint16 getOverflow() { return timer_get_reload(this->dev); }

    /**
     * @brief Set the timer overflow (or "reload") value.
     *
     * The new value won't take effect until the next time the counter
     * overflows.  You can force the counter to reset using
     * HardwareTimer::refresh().
     *
     * @param val The new overflow value to set
     * @see HardwareTimer::refresh()
     */
    void setOverflow(uint16 val) { timer_set_reload(this->dev, val); }

    /**
     * @brief Get the current timer count.
     *
     * @return The timer's current count value
     */
    uint16 getCount(void) { return timer_get_count(this->dev); }

    /**
     * @brief Set the current timer count.
     *
     * @param val The new count value to set.  If this value exceeds
     *            the timer's overflow value, it is truncated to the
     *            overflow value.
     */
    void setCount(uint16 val) { timer_set_count(this->dev, min(val, this->getOverflow())); }

    /**
     * @brief Set the timer's period in microseconds.
     *
     * Configures the prescaler and overflow values to generate a timer
     * reload with a period as close to the given number of
     * microseconds as possible.
     *
     * @param microseconds The desired period of the timer.  This must be
     *                     greater than zero.
     * @return The new overflow value.
     */
    uint16 setPeriod(uint32 microseconds);

    void setMasterMode(timer_mms_t mode) {
        timer_set_master_mode(this->dev, mode);
    }

    /**
     * @brief Configure a timer channel's mode.
     * @param channel Timer channel, from 1 to 4
     * @param mode Mode to set
     */
    void setMode(int channel, timer_mode mode) { timer_set_mode(this->dev, channel, mode); }

    /**
     * @brief Get the compare value for the given channel.
     * @see HardwareTimer::setCompare()
     */
    uint16 getCompare(int channel) { return timer_get_compare(this->dev, channel); }

    /**
     * @brief Set the compare value for the given channel.
     *
     * @param channel the channel whose compare to set, from 1 to 4.
     * @param compare The compare value to set.  If greater than this
     *                timer's overflow value, it will be truncated to
     *                the overflow value.
     *
     * @see timer_mode
     * @see HardwareTimer::setMode()
     * @see HardwareTimer::attachInterrupt()
     */
    void setCompare(uint8 channel, uint16 val) {
        timer_set_compare(this->dev, channel, min(val, this->getOverflow()));
    }

    /**
     * @brief Attach an interrupt handler to the given channel.
     *
     * This interrupt handler will be called when the timer's counter
     * reaches the given channel compare value.
     *
     * @param channel the channel to attach the ISR to, from 0 to 4.
     *   Channel 0 is for overflow interrupt (update interrupt).
     * @param handler The ISR to attach to the given channel.
     * @see voidFuncPtr
     */
    void attachInterrupt(uint8 channel, voidFuncPtr handler) {
        timer_attach_interrupt(this->dev, channel, handler);
    }

    /**
     * @brief Remove the interrupt handler attached to the given
     *        channel, if any.
     *
     * The handler will no longer be called by this timer.
     *
     * @param channel the channel whose interrupt to detach, from 0 to 4.
     *   Channel 0 is for overflow interrupt (update interrupt).
     * @see HardwareTimer::attachInterrupt()
     */
    void detachInterrupt(uint8 channel) {
        timer_detach_interrupt(this->dev, (uint8)channel);
    }

    /**
     * @brief Reset the counter, and update the prescaler and overflow
     *        values.
     *
     * This will reset the counter to 0 in upcounting mode (the
     * default).  It will also update the timer's prescaler and
     * overflow, if you have set them up to be changed using
     * HardwareTimer::setPrescaleFactor() or
     * HardwareTimer::setOverflow().
     *
     * @see HardwareTimer::setPrescaleFactor()
     * @see HardwareTimer::setOverflow()
     */
    void refresh(void) { timer_generate_update(this->dev); }

	// SYFRE
    /**
     * @brief Set the Master mode TRGO signal 
     *        These bits allow to select the information to be sent in master mode to slave timers for 
     *        synchronization (TRGO). 
	 *	mode:
	 * 		TIMER_CR2_MMS_RESET
	 * 		TIMER_CR2_MMS_ENABLE
	 * 		TIMER_CR2_MMS_UPDATE
	 * 		TIMER_CR2_MMS_COMPARE_PULSE
	 * 		TIMER_CR2_MMS_COMPARE_OC1REF
	 * 		TIMER_CR2_MMS_COMPARE_OC2REF
	 * 		TIMER_CR2_MMS_COMPARE_OC3REF
	 * 		TIMER_CR2_MMS_COMPARE_OC4REF
     */
	void setMasterModeTrGo(uint32_t mode);

    void setSlaveFlags(uint32 flags) { ((this->dev)->regs).gen->SMCR = flags; }

//CARLOS.
/*
    added these functions to make sense for the encoder mode. 
*/
//direction of movement. (to be better described). 
    uint8 getDirection() { return get_direction(this->dev); }

//set if the encoder will count edges on one, which or both channels. 
    void setEdgeCounting(uint32 counting) { setSlaveFlags(counting); }

//set the polarity of counting... not sure how interesting this is.. 
    void setPolarity(timer_channel channel, uint8 pol) {
        timer_cc_set_pol(this->dev, channel, pol);
    }

    uint8_t getPolarity(timer_channel channel) {
        return timer_cc_get_pol(this->dev, channel);
    }

    void setInputCaptureMode(timer_channel channel, timer_ic_input_select input) {
        input_capture_mode(this->dev, channel, input);
    }

    uint8_t getInputCaptureFlag(timer_channel channel) {
        return ( timer_get_status(this->dev) >> channel ) & 0x1;
    }

    uint8_t getInputCaptureFlagOverflow(timer_channel channel) {
		uint8 ret = ( timer_get_status(this->dev) >> (8+channel) ) & 0x1;
		if ( ret ) timer_reset_status_bit(this->dev, (8+channel)); // clear flag
        return ret;
    }

    /**
     * @brief Enable/disable DMA request for the input channel.
     */
    void enableDMA(timer_channel channel) { timer_dma_enable_req(this->dev, channel); }

    void disableDMA(timer_channel channel) { timer_dma_disable_req(this->dev, channel); }


    /**
     * @brief Get a pointer to the underlying libmaple timer_dev for
     *        this HardwareTimer instance.
     */
    const timer_dev* c_dev(void) { return this->dev; }


};


/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer instances.
 */
extern HardwareTimer Timer1;
extern HardwareTimer Timer2;
extern HardwareTimer Timer3;
extern HardwareTimer Timer4;
extern HardwareTimer Timer5;
extern HardwareTimer Timer6;
extern HardwareTimer Timer7;
extern HardwareTimer Timer8;
extern HardwareTimer Timer9;
extern HardwareTimer Timer10;
extern HardwareTimer Timer11;
extern HardwareTimer Timer12;
extern HardwareTimer Timer13;
extern HardwareTimer Timer14;


#endif
