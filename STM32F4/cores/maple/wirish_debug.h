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
 * @file wirish_debug.h
 * @brief High level debug port configuration
 */

/**
 * @brief Disable the JTAG and Serial Wire (SW) debug ports.
 *
 * You can call this function in order to use the JTAG and SW debug
 * pins as ordinary GPIOs.
 *
 * @see enableDebugPorts()
 */
static inline void disableDebugPorts(void) {
    afio_cfg_debug_ports(AFIO_DEBUG_NONE);
}

/**
 * @brief Enable the JTAG and Serial Wire (SW) debug ports.
 *
 * After you call this function, the JTAG and SW debug pins will no
 * longer be usable as GPIOs.
 *
 * @see disableDebugPorts()
 */
static inline void enableDebugPorts(void) {
    afio_cfg_debug_ports(AFIO_DEBUG_FULL_SWJ);
}
