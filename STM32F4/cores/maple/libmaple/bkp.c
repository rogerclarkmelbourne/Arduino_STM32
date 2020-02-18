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
 * @file bkp.c
 * @brief Backup register support.
 */

#include "bkp.h"
#include "bitband.h"


/*
 * Data register memory layout is not contiguous. It's split up from
 * 1--NR_LOW_DRS, beginning at BKP->DR1, through to
 * (NR_LOW_DRS+1)--BKP_NR_DATA_REGS, beginning at BKP->DR11.
 */
static __IO uint32* data_register(uint8 reg)
{
    if ( reg==0 || reg > BKP_NR_DATA_REGS) {
        return 0;
    }

#if BKP_NR_DATA_REGS == NR_LOW_DRS
    return (uint32*)BKP + reg;
#else
    if (reg <= NR_LOW_DRS) {
        return (uint32*)BKP + reg;
    } else {
        return (uint32*)&(BKP->DR11) + (reg - NR_LOW_DRS - 1);
    }
#endif
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
