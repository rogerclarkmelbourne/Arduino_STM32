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

#ifndef _LIBMAPLE_STM32_PRIVATE_H_
#define _LIBMAPLE_STM32_PRIVATE_H_

typedef enum stm32_mem_block_purpose {
    STM32_BLOCK_CODE,
    STM32_BLOCK_SRAM,
    STM32_BLOCK_PERIPH,
    STM32_BLOCK_FSMC_1_2,
    STM32_BLOCK_FSMC_3_4,
    STM32_BLOCK_FSMC_REG,
    STM32_BLOCK_UNUSED,
    STM32_BLOCK_CORTEX_INTERNAL,
} stm32_mem_block_purpose;

static inline stm32_mem_block_purpose stm32_block_purpose(void *addr) {
    return (stm32_mem_block_purpose)((unsigned)addr >> 29);
}

#endif
