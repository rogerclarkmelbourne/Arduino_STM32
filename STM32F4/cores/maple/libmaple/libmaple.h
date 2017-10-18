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
 *  @file libmaple.h
 *  @brief General include file for libmaple
 */

#ifndef _LIBMAPLE_H_
#define _LIBMAPLE_H_

#include "util.h"

/*
 * Where to put usercode, based on space reserved for bootloader.
 *
 * FIXME this has no business being here
 */
/*
#if defined(MCU_STM32F103VE) || defined(MCU_STM32F205VE) || defined(MCU_STM32F406VG)
    // e.g., Aeroquad32
    #define USER_ADDR_ROM 0x08010000    // ala42
    #define USER_ADDR_RAM 0x20000C00
    #define STACK_TOP     0x20000800
#elif defined(BOARD_freeflight)
*/
#ifndef USER_ADDR_ROM
#define USER_ADDR_ROM 0x08000000
#endif
#define USER_ADDR_RAM 0x20000C00
#define STACK_TOP     0x20000800
/*
#else
#define USER_ADDR_ROM 0x08005000
#define USER_ADDR_RAM 0x20000C00
#define STACK_TOP     0x20000800
#endif
*/

#endif

