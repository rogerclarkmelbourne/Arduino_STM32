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

#ifndef _WIRISH_HARDWARETIMER_H_
#define _WIRISH_HARDWARETIMER_H_

// TODO [0.1.0] Remove deprecated pieces, pick a better API

#include <libmaple/timer.h>

/** Timer mode. */
typedef timer_mode TimerMode;

//CARLOS
//defines for the ENCODER mode.


/**
 * @brief Interface to one of the 16-bit timer peripherals.
 */
class HardwareTimer {
private:
    timer_dev *dev;

public:
    /**
     * @brief Construct a new HardwareTimer instance.
     * @param timerNum number of the timer to control.
     */
    HardwareTimer(uint8 timerNum);

    /**
     * @brief Stop the counter, without affecting its configuration.
     *
     * @see HardwareTimer::resume()
     */
    void pause(void);

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
    void resume(void);

    /**
     * @brief Get the timer's prescale factor.
     * @return Timer prescaler, from 1 to 65,536.
     * @see HardwareTimer::setPrescaleFactor()
     */
    uint32 getPrescaleFactor();

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
    void setPrescaleFactor(uint32 factor);

    /**
     * @brief Get the timer overflow value.
     * @see HardwareTimer::setOverflow()
     */
    uint16 getOverflow();

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
    void setOverflow(uint16 val);

    /**
     * @brief Get the current timer count.
     *
     * @return The timer's current count value
     */
    uint16 getCount(void);

    /**
     * @brief Set the current timer count.
     *
     * @param val The new count value to set.  If this value exceeds
     *            the timer's overflow value, it is truncated to the
     *            overflow value.
     */
    void setCount(uint16 val);

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

    /**
     * @brief Configure a timer channel's mode.
     * @param channel Timer channel, from 1 to 4
     * @param mode Mode to set
     */
    void setMode(int channel, timer_mode mode);

    /**
     * @brief Get the compare value for the given channel.
     * @see HardwareTimer::setCompare()
     */
    uint16 getCompare(int channel);

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
    void setCompare(int channel, uint16 compare);

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
    void attachInterrupt(int channel, voidFuncPtr handler);

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
    void detachInterrupt(int channel);

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
    void refresh(void);

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

    void setSlaveFlags(uint32 flags) {
        ((this->dev)->regs).gen->SMCR = flags;
    }

//CARLOS.
/*
    added these functions to make sense for the encoder mode. 
*/
//direction of movement. (to be better described). 
    uint8 getDirection() {
        return get_direction(this->dev);
    }

//set if the encoder will count edges on one, which or both channels. 
    void setEdgeCounting(uint32 counting) {
        setSlaveFlags(counting);
    }

//set the polarity of counting... not sure how interesting this is.. 
    void setPolarity(uint8 channel, uint8 pol) {
        timer_cc_set_pol(this->dev, channel, pol);
    }

    void setInputCaptureMode(uint8 channel, timer_ic_input_select input) {
        input_capture_mode(this->dev, channel, input);
    }

    uint8_t getInputCaptureFlag(uint8 channel) {
        return ( timer_get_status(this->dev) >> channel ) & 0x1;
    }

    uint8_t getInputCaptureFlagOverflow(uint8 channel) {
		uint8 ret = ( timer_get_status(this->dev) >> (8+channel) ) & 0x1;
		if ( ret ) timer_reset_status_bit(this->dev, (8+channel)); // clear flag
        return ret;
    }

//add the filtering definition for the input channel.
    

    /**
     * @brief Enable/disable DMA request for the input channel.
     */
    void enableDMA(int channel);
    void disableDMA(int channel);

    /**
     * @brief Get a pointer to the underlying libmaple timer_dev for
     *        this HardwareTimer instance.
     */
    timer_dev* c_dev(void) { return this->dev; }

/* -- The rest of this file is deprecated. --------------------------------- */

    /** @brief Deprecated; use setMode(channel, mode) instead. */
    void setChannelMode(int channel, timer_mode mode) {
        setMode(channel, mode);
    }

    /** @brief Deprecated; use setMode(TIMER_CH1, mode) instead. */
    void setChannel1Mode(timer_mode mode) { setMode(TIMER_CH1, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH2, mode) instead. */
    void setChannel2Mode(timer_mode mode) { setMode(TIMER_CH2, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH3, mode) instead. */
    void setChannel3Mode(timer_mode mode) { setMode(TIMER_CH3, mode); }

    /** @brief Deprecated; use setMode(TIMER_CH4, mode) instead. */
    void setChannel4Mode(timer_mode mode) { setMode(TIMER_CH4, mode); }

    /** @brief Deprecated; use return getCompare(TIMER_CH1) instead. */
    uint16 getCompare1() { return getCompare(TIMER_CH1); }

    /** @brief Deprecated; use return getCompare(TIMER_CH2) instead. */
    uint16 getCompare2() { return getCompare(TIMER_CH2); }

    /** @brief Deprecated; use return getCompare(TIMER_CH3) instead. */
    uint16 getCompare3() { return getCompare(TIMER_CH3); }

    /** @brief Deprecated; use return getCompare(TIMER_CH4) instead. */
    uint16 getCompare4() { return getCompare(TIMER_CH4); }

    /** @brief Deprecated; use setCompare(TIMER_CH1, compare) instead. */
    void setCompare1(uint16 compare) { setCompare(TIMER_CH1, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH2, compare) instead. */
    void setCompare2(uint16 compare) { setCompare(TIMER_CH2, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH3, compare) instead. */
    void setCompare3(uint16 compare) { setCompare(TIMER_CH3, compare); }

    /** @brief Deprecated; use setCompare(TIMER_CH4, compare) instead. */
    void setCompare4(uint16 compare) { setCompare(TIMER_CH4, compare); }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH1, handler) instead. */
    void attachCompare1Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CH1, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH2, handler) instead. */
    void attachCompare2Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CH2, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH3, handler) instead. */
    void attachCompare3Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CH3, handler);
    }

    /** @brief Deprecated; use attachInterrupt(TIMER_CH4, handler) instead. */
    void attachCompare4Interrupt(voidFuncPtr handler) {
        attachInterrupt(TIMER_CH4, handler);
    }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH1) instead. */
    void detachCompare1Interrupt(void) { detachInterrupt(TIMER_CH1); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH2) instead. */
    void detachCompare2Interrupt(void) { detachInterrupt(TIMER_CH2); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH3) instead. */
    void detachCompare3Interrupt(void) { detachInterrupt(TIMER_CH3); }

    /** @brief Deprecated; use detachInterrupt(TIMER_CH4) instead. */
    void detachCompare4Interrupt(void) { detachInterrupt(TIMER_CH4); }

    /** @brief Deprecated; use refresh() instead. */
    void generateUpdate(void) { refresh(); }
};

/** @brief Deprecated; use TIMER_OUTPUT_COMPARE instead. */
#define TIMER_OUTPUTCOMPARE TIMER_OUTPUT_COMPARE

/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer1;
/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer2;
/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer3;
/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer4;
#if (STM32_MCU_SERIES == STM32_SERIES_F1) && defined(STM32_HIGH_DENSITY)
/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer5;
/**
 * @brief Deprecated.
 *
 * Pre-instantiated timer.
 */
extern HardwareTimer Timer8;
#endif

#endif
