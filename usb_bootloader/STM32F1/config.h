/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 *  @file config.h
 *
 *  @brief bootloader settings and macro defines
 *
 *
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include "common.h"

/* Speed controls for strobing the LED pin */
#define BLINK_FAST 0x50000
#define BLINK_SLOW 0x100000

/* On the Mini, LED is PB1 */
#if defined(TARGET_MAPLE_MINI)
#define LED_BANK         GPIOB
#define LED              1
#define LED_BANK_CR      GPIO_CRL(LED_BANK)
#define LED_CR_MASK      0xFFFFFF0F
#define LED_CR_MODE      0x00000010
#define RCC_APB2ENR_LED  0x00000008 /* enable PB */

/* On the Mini, BUT is PB8 */
#define BUTTON_BANK      GPIOB
#define BUTTON           8
#define BUT_BANK_CR      GPIO_CRH(BUTTON_BANK)
#define BUT_CR_MASK      0xFFFFFFF0
#define BUT_CR_OUTPUT_IN 0x00000004
#define RCC_APB2ENR_BUT  0x00000008 /* enable PB */
#else

#error "No config for this target"

#endif

#define STARTUP_BLINKS 5
#define BOOTLOADER_WAIT 6

#define USER_CODE_RAM     			((u32)0x20000C00)
#define RAM_END           			((u32)0x20005000)
#define USER_CODE_FLASH0X8005000   	((u32)0x08005000)
#define USER_CODE_FLASH0X8002000	((u32)0x08002000)
#define FLASH_END         			((u32)0x08020000)

#define VEND_ID0 0xAF
#define VEND_ID1 0x1E
#define PROD_ID0 0x03
#define PROD_ID1 0x00

#endif
