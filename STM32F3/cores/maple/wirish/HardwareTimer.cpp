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

#include <wirish/HardwareTimer.h>

#include <libmaple/rcc.h>
#include <wirish/ext_interrupts.h> // for noInterrupts(), interrupts()
#include <wirish/wirish_math.h>
#include <board/board.h>           // for CYCLES_PER_MICROSECOND

// TODO [0.1.0] Remove deprecated pieces

/*
 * Evil hack to infer this->dev from timerNum in the HardwareTimer
 * constructor. See:
 *
 * http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.2
 * http://yosefk.com/c++fqa/function.html#fqa-33.2
 */

extern "C" {
    static timer_dev **this_devp;
    static rcc_clk_id this_id;
    static void set_this_dev(timer_dev *dev) {
        if (dev->clk_id == this_id) {
            *this_devp = dev;
        }
    }
}

/*
 * HardwareTimer routines
 */

HardwareTimer::HardwareTimer(uint8 timerNum) {
    rcc_clk_id timerID = (rcc_clk_id)(RCC_TIMER1 + (timerNum - 1));
    this->dev = NULL;
    noInterrupts(); // Hack to ensure we're the only ones using
                    // set_this_dev() and friends. TODO: use a lock.
    this_id = timerID;
    this_devp = &this->dev;
    timer_foreach(set_this_dev);
    interrupts();
    ASSERT(this->dev != NULL);
}

void HardwareTimer::pause(void) {
    timer_pause(this->dev);
}

void HardwareTimer::resume(void) {
    timer_resume(this->dev);
}

uint32 HardwareTimer::getPrescaleFactor(void) {
    return timer_get_prescaler(this->dev) + 1;
}

void HardwareTimer::setPrescaleFactor(uint32 factor) {
    timer_set_prescaler(this->dev, (uint16)(factor - 1));
}

uint16 HardwareTimer::getOverflow() {
    return timer_get_reload(this->dev);
}

void HardwareTimer::setOverflow(uint16 val) {
    timer_set_reload(this->dev, val);
}

uint16 HardwareTimer::getCount(void) {
    return timer_get_count(this->dev);
}

void HardwareTimer::setCount(uint16 val) {
    uint16 ovf = this->getOverflow();
    timer_set_count(this->dev, min(val, ovf));
}

#define MAX_RELOAD ((1 << 16) - 1)
uint16 HardwareTimer::setPeriod(uint32 microseconds) {
    // Not the best way to handle this edge case?
    if (!microseconds) {
        this->setPrescaleFactor(1);
        this->setOverflow(1);
        return this->getOverflow();
    }

    uint32 period_cyc = microseconds * CYCLES_PER_MICROSECOND;
    uint16 prescaler = (uint16)(period_cyc / MAX_RELOAD + 1);
    uint16 overflow = (uint16)((period_cyc + (prescaler / 2)) / prescaler);
    this->setPrescaleFactor(prescaler);
    this->setOverflow(overflow);
    return overflow;
}

void HardwareTimer::setMode(int channel, timer_mode mode) {
    timer_set_mode(this->dev, (uint8)channel, (timer_mode)mode);
}

uint16 HardwareTimer::getCompare(int channel) {
    return timer_get_compare(this->dev, (uint8)channel);
}

void HardwareTimer::setCompare(int channel, uint16 val) {
    uint16 ovf = this->getOverflow();
    timer_set_compare(this->dev, (uint8)channel, min(val, ovf));
}

void HardwareTimer::attachInterrupt(int channel, voidFuncPtr handler) {
    timer_attach_interrupt(this->dev, (uint8)channel, handler);
}

void HardwareTimer::detachInterrupt(int channel) {
    timer_detach_interrupt(this->dev, (uint8)channel);
}

void HardwareTimer::refresh(void) {
    timer_generate_update(this->dev);
}

/* -- Deprecated predefined instances -------------------------------------- */

HardwareTimer Timer1(1);
HardwareTimer Timer2(2);
HardwareTimer Timer3(3);
HardwareTimer Timer4(4);
#ifdef STM32_HIGH_DENSITY
HardwareTimer Timer5(5);
HardwareTimer Timer6(6);
HardwareTimer Timer7(7);
HardwareTimer Timer8(8);
#endif
