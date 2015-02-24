/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file wirish/include/wirish/wirish_time.h
 * @brief Timing and delay functions.
 */

#ifndef _WIRISH_WIRISH_TIME_H_
#define _WIRISH_WIRISH_TIME_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/systick.h>

#include <boards.h>

/**
 * Returns time (in milliseconds) since the beginning of program
 * execution. On overflow, restarts at 0.
 * @see micros()
 */
static inline uint32 millis(void) {
    return systick_uptime();
}

/**
 * Returns time (in microseconds) since the beginning of program
 * execution.  On overflow, restarts at 0.
 * @see millis()
 */
static inline uint32 micros(void) {
    uint32 ms;
    uint32 cycle_cnt;

    do {
        ms = millis();
        cycle_cnt = systick_get_count();
        asm volatile("nop"); //allow interrupt to fire
        asm volatile("nop");
    } while (ms != millis());

#define US_PER_MS               1000
    /* SYSTICK_RELOAD_VAL is 1 less than the number of cycles it
     * actually takes to complete a SysTick reload */
    return ((ms * US_PER_MS) +
            (SYSTICK_RELOAD_VAL + 1 - cycle_cnt) / CYCLES_PER_MICROSECOND);
#undef US_PER_MS
}

/**
 * Delay for at least the given number of milliseconds.
 *
 * Interrupts, etc. may cause the actual number of milliseconds to
 * exceed ms.  However, this function will return no less than ms
 * milliseconds from the time it is called.
 *
 * @param ms the number of milliseconds to delay.
 * @see delayMicroseconds()
 */
void delay(unsigned long ms);

/**
 * Delay for at least the given number of microseconds.
 *
 * Interrupts, etc. may cause the actual number of microseconds to
 * exceed us.  However, this function will return no less than us
 * microseconds from the time it is called.
 *
 * @param us the number of microseconds to delay.
 * @see delay()
 */
void delayMicroseconds(uint32 us);

#endif
