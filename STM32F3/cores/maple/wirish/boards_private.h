/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file wirish/boards_private.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Private board support header.
 *
 * This file declares chip-specific variables and functions which
 * determine how init() behaves. It is not part of the public Wirish
 * API, and can change without notice.
 */

#ifndef _WIRISH_BOARDS_PRIVATE_H_
#define _WIRISH_BOARDS_PRIVATE_H_

#include <libmaple/rcc.h>
#include <libmaple/adc.h>

/* Makes the PIN_MAP rows more human-readable. */
#define PMAP_ROW(gpio_dev, gpio_bit, timer_dev, timer_ch, adc_dev, adc_ch) \
    { gpio_dev, timer_dev, adc_dev, gpio_bit, timer_ch, adc_ch }

namespace wirish {
    namespace priv {

        /*
         * Chip-specific initialization data
         */

        extern rcc_pll_cfg w_board_pll_cfg;
        extern adc_prescaler w_adc_pre;
        extern adc_smp_rate w_adc_smp;

        /*
         * Chip-specific initialization routines and helper functions.
         */

        void board_reset_pll(void);
        void board_setup_clock_prescalers(void);
        void board_setup_gpio(void);
        void board_setup_usb(void);
        void series_init(void);

    }
}

#endif
