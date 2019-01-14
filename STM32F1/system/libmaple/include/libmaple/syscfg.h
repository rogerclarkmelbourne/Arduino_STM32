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
 * @file libmaple/include/libmaple/syscfg.h
 * @brief System configuration controller (SYSCFG)
 *
 * Availability: STM32F2, STM32F4.
 */

#ifndef _LIBMAPLE_SYSCFG_H_
#define _LIBMAPLE_SYSCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map and base pointer
 */

/**
 * @brief SYSCFG register map type.
 */
typedef struct syscfg_reg_map {
    __IO uint32 MEMRMP;    /**< Memory remap register */
    __IO uint32 PMC;       /**< Peripheral mode configuration */
    __IO uint32 EXTICR[4]; /**< External interrupt configuration registers */
    const uint32 RESERVED1;
    const uint32 RESERVED2;
    __IO uint32 CMPCR;   /**< Compensation cell control register */
} syscfg_reg_map;

/** SYSCFG register map base pointer */
#define SYSCFG_BASE                     ((struct syscfg_reg_map*)0x40013800)

/*
 * Register bit definitions
 */

/* Memory remap register */

#define SYSCFG_MEMRMP_MEM_MODE           0x3
#define SYSCFG_MEMRMP_MEM_MODE_FLASH     0x0
#define SYSCFG_MEMRMP_MEM_MODE_SYS_FLASH 0x1
#define SYSCFG_MEMRMP_MEM_MODE_FSMC_1    0x2
#define SYSCFG_MEMRMP_MEM_MODE_EMB_SRAM  0x3

/* Peripheral mode configuration register */

#define SYSCFG_PMC_MII_RMII_SEL_BIT     23

#define SYSCFG_PMC_MII_RMII_SEL         (1U << SYSCFG_PMC_MII_RMII_SEL_BIT)
#define SYSCFG_PMC_MII_RMII_SEL_MII     (0U << SYSCFG_PMC_MII_RMII_SEL_BIT)
#define SYSCFG_PMC_MII_RMII_SEL_RMII    (1U << SYSCFG_PMC_MII_RMII_SEL_BIT)

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

/* Compensation cell control register */

#define SYSCFG_CMPCR_READY_BIT          8
#define SYSCFG_CMPCR_CMP_PD_BIT         0

#define SYSCFG_CMPCR_READY              (1U << SYSCFG_CMPCR_READY_BIT)
#define SYSCFG_CMPCR_CMP_PD             (1U << SYSCFG_CMPCR_CMP_PD_BIT)
#define SYSCFG_CMPCR_CMP_PD_PDWN        (0U << SYSCFG_CMPCR_CMP_PD_BIT)
#define SYSCFG_CMPCR_CMP_PD_ENABLE      (1U << SYSCFG_CMPCR_CMP_PD_BIT)

/*
 * Routines
 */

void syscfg_init(void);

void syscfg_enable_io_compensation(void);
void syscfg_disable_io_compensation(void);

/**
 * @brief System memory mode
 * These values specify what memory to map to address 0x00000000.
 * @see syscfg_set_mem_mode
 */
typedef enum syscfg_mem_mode {
    /** Main flash memory is mapped at 0x0. */
    SYCFG_MEM_MODE_FLASH        = SYSCFG_MEMRMP_MEM_MODE_FLASH,
    /** System flash (i.e. ST's baked-in bootloader) is mapped at 0x0. */
    SYCFG_MEM_MODE_SYSTEM_FLASH = SYSCFG_MEMRMP_MEM_MODE_SYS_FLASH,
    /** FSMC bank 1 (NOR/PSRAM 1 and 2) is mapped at 0x0. */
    SYCFG_MEM_MODE_FSMC_BANK_1  = SYSCFG_MEMRMP_MEM_MODE_FSMC_1,
    /** Embedded SRAM (i.e., not backup SRAM) is mapped at 0x0. */
    SYCFG_MEM_MODE_SRAM         = SYSCFG_MEMRMP_MEM_MODE_EMB_SRAM,
} syscfg_mem_mode;

void syscfg_set_mem_mode(syscfg_mem_mode);

#ifdef __cplusplus
}
#endif

#endif
