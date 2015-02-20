/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/include/libmaple/syscfg.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief System configuration controller (SYSCFG)
 *
 * Availability: STM32F2, STM32F3, STM32F4.
 */

#ifndef _LIBMAPLE_SYSCFG_H_
#define _LIBMAPLE_SYSCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <series/syscfg.h>
#include <libmaple/rcc.h>

/* External interrupt configuration register 1 */

#define SYSCFG_EXTICR1_EXTI0            0xF
#define SYSCFG_EXTICR1_EXTI1            0xF0
#define SYSCFG_EXTICR1_EXTI2            0xF00
#define SYSCFG_EXTICR1_EXTI3            0xF000

/* External interrupt configuration register 2 */

#define SYSCFG_EXTICR2_EXTI4            0xF
#define SYSCFG_EXTICR2_EXTI5            0xF0
#define SYSCFG_EXTICR2_EXTI6            0xF00
#define SYSCFG_EXTICR2_EXTI7            0xF000

/* External interrupt configuration register 3 */

#define SYSCFG_EXTICR3_EXTI8            0xF
#define SYSCFG_EXTICR3_EXTI9            0xF0
#define SYSCFG_EXTICR3_EXTI10           0xF00
#define SYSCFG_EXTICR3_EXTI11           0xF000

/* External interrupt configuration register 4 */

#define SYSCFG_EXTICR4_EXTI12           0xF
#define SYSCFG_EXTICR4_EXTI13           0xF0
#define SYSCFG_EXTICR4_EXTI14           0xF00
#define SYSCFG_EXTICR4_EXTI15           0xF000

/*
 * Routines
 */

/**
 * @brief Initialize the SYSCFG peripheral.
 */
void syscfg_init(void);

/**
 * @brief System memory mode
 * These values specify what memory to map to address 0x00000000.
 * @see syscfg_set_mem_mode
 */
typedef enum syscfg_mem_mode {
    /** Main flash memory is mapped at 0x0. */
    SYSCFG_MEM_MODE_FLASH        = 0x0,
    /** System flash (i.e. ST's baked-in bootloader) is mapped at 0x0. */
    SYSCFG_MEM_MODE_SYSTEM_FLASH = 0x1,
    /** FSMC bank 1 (NOR/PSRAM 1 and 2) is mapped at 0x0. */
    SYSCFG_MEM_MODE_FSMC_BANK_1  = 0x2,
    /** Embedded SRAM (i.e., not backup SRAM) is mapped at 0x0. */
    SYSCFG_MEM_MODE_SRAM         = 0x3,
} syscfg_mem_mode;


/**
 * @brief Set the memory to be mapped at address 0x00000000.
 *
 * This function can be used to override the BOOT pin
 * configuration. Some restrictions apply; see your chip's reference
 * manual for the details.
 *
 * @param mode Mode to set
 * @see syscfg_mem_mode
 */
void syscfg_set_mem_mode(syscfg_mem_mode);

#ifdef __cplusplus
}
#endif

#endif
