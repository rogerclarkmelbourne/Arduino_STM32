/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs, LLC.
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
 * @file libmaple/stm32f1/bkp.c
 * @brief STM32F1 Backup register support.
 */

#include <libmaple/bkp.h>
#include <libmaple/pwr.h>
#include <libmaple/rcc.h>
#include <libmaple/bitband.h>

static inline __IO uint32* data_register(uint8 reg);

bkp_dev bkp = {
    .regs = BKP_BASE,
};
/** Backup device. */
const bkp_dev *BKP = &bkp;

/**
 * @brief Initialize backup interface.
 *
 * Enables the power and backup interface clocks, and resets the
 * backup device.
 */
void bkp_init(void) {
    /* Don't call pwr_init(), or you'll reset the device.  We just
     * need the clock. */
    rcc_clk_enable(RCC_PWR);
    rcc_clk_enable(RCC_BKP);
    rcc_reset_dev(RCC_BKP);
}

/**
 * Enable write access to the backup registers.  Backup interface must
 * be initialized for subsequent register writes to work.
 * @see bkp_init()
 */
void bkp_enable_writes(void) {
    *bb_perip(&PWR_BASE->CR, PWR_CR_DBP_BIT) = 1;
}

/**
 * Disable write access to the backup registers.
 */
void bkp_disable_writes(void) {
    *bb_perip(&PWR_BASE->CR, PWR_CR_DBP_BIT) = 0;
}

/**
 * Read a value from given backup data register.
 * @param reg Data register to read, from 1 to BKP_NR_DATA_REGS (10 on
 *            medium-density devices, 42 on high-density devices).
 */
uint16 bkp_read(uint8 reg) {
    __IO uint32* dr = data_register(reg);
    if (!dr) {
        ASSERT(0);                  /* nonexistent register */
        return 0;
    }
    return (uint16)*dr;
}

/**
 * @brief Write a value to given data register.
 *
 * Write access to backup registers must be enabled.
 *
 * @param reg Data register to write, from 1 to BKP_NR_DATA_REGS (10
 *            on medium-density devices, 42 on high-density devices).
 * @param val Value to write into the register.
 * @see bkp_enable_writes()
 */
void bkp_write(uint8 reg, uint16 val) {
    __IO uint32* dr = data_register(reg);
    if (!dr) {
        ASSERT(0);                  /* nonexistent register */
        return;
    }
    *dr = (uint32)val;
}

/*
 * Data register memory layout is not contiguous. It's split up from
 * 1--NR_LOW_DRS, beginning at BKP_BASE->DR1, through to
 * (NR_LOW_DRS+1)--BKP_NR_DATA_REGS, beginning at BKP_BASE->DR11.
 */
#define NR_LOW_DRS 10

static inline __IO uint32* data_register(uint8 reg) {
    if (reg < 1 || reg > BKP_NR_DATA_REGS) {
        return 0;
    }

#if BKP_NR_DATA_REGS == NR_LOW_DRS
    return (uint32*)BKP_BASE + reg;
#else
    if (reg <= NR_LOW_DRS) {
        return (uint32*)BKP_BASE + reg;
    } else {
        return (uint32*)&(BKP_BASE->DR11) + (reg - NR_LOW_DRS - 1);
    }
#endif
}
