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

#include <libmaple/bkp.h>
#include <libmaple/bitband.h>
#include <string.h>

/*
 * returns data register address
 * reg is 1 to BKP_NR_DATA_REGS
 */
static __IO uint32* data_register(uint8 reg)
{
    if ( reg==0 || reg > BKP_NR_DATA_REGS) {
        return 0;
    }

    return (uint32*)BKP + reg - 1;
}

/**
 * Read a value from given backup data register.
 * @param reg Data register to read, from 1 to BKP_NR_DATA_REGS (10 on
 *            medium-density devices, 42 on high-density devices).
 */
uint32 bkp_read(uint8 reg) {
    __IO uint32* dr = data_register(reg);
    if (!dr) {
        ASSERT(0);                  /* nonexistent register */
        return 0;
    }
    return *dr;
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
void bkp_write(uint8 reg, uint32 val) {
    __IO uint32* dr = data_register(reg);
    if (!dr) {
        ASSERT(0);                  /* nonexistent register */
        return;
    }
    *dr = val;
}

/*
 * BKPSRAM functions
 */

uint8_t bkp_sramread8(uint16_t offset) {
	return *((uint8_t *)(BKPSRAM_BASE) + offset);
}

void bkp_sramwrite8(uint16_t offset, uint8_t data) {
	if (offset < BKPSIZE)
		*((uint8_t *)(BKPSRAM_BASE) + offset) = data;
}

uint16_t bkp_sramread16(uint16_t offset) {
	return *((uint16_t *)(BKPSRAM_BASE) + offset);
}

void bkp_sramwrite16(uint16_t offset, uint16_t data) {
	uint16_t *p = (uint16_t *)(BKPSRAM_BASE) + offset;
	if (offset * 2 < BKPSIZE)
		*p = data;
}

uint32_t bkp_sramread32(uint16_t offset) {
	return *((uint32_t *)(BKPSRAM_BASE) + offset);
}

void bkp_sramwrite32(uint16_t offset, uint32_t data) {
	uint32_t *p = (uint32_t *)(BKPSRAM_BASE) + offset;
	if (offset * 4 < BKPSIZE)
		*p = data;
}

/* copies  data to bkpsram
 *
 */
void bkp_sramwrite(uint16_t offset, uint8_t *data, uint16_t length) {
	if(length > BKPSIZE - offset)
		length = BKPSIZE - offset;
	memcpy((void *)(BKPSRAM_BASE + offset), data, length);
}

void bkpsram_clear() {
	memset((void*)BKPSRAM_BASE, 0, BKPSIZE);
}

