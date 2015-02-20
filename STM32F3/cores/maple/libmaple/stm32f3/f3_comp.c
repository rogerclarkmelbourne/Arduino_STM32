/******************************************************************************
 * The MIT License
 *
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
 * @file libmaple/stm32f3/comp.c
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 Comparator support.
 */

#include <libmaple/comp.h>

/*
 * Devices
 */

static comp_dev comp1 = { .regs   = COMP1_BASE }; const comp_dev *COMP1 = &comp1;
static comp_dev comp2 = { .regs   = COMP2_BASE }; const comp_dev *COMP2 = &comp2;
static comp_dev comp3 = { .regs   = COMP3_BASE }; const comp_dev *COMP3 = &comp3;
static comp_dev comp4 = { .regs   = COMP4_BASE }; const comp_dev *COMP4 = &comp4;
static comp_dev comp5 = { .regs   = COMP5_BASE }; const comp_dev *COMP5 = &comp5;
static comp_dev comp6 = { .regs   = COMP6_BASE }; const comp_dev *COMP6 = &comp6;
static comp_dev comp7 = { .regs   = COMP7_BASE }; const comp_dev *COMP7 = &comp7;

void __irq_comp123(void) {
	//TODO
}

void __irq_com456(void) {
	//TODO
}

void __irq_comp7(void) {
	//TODO
}

/* TODO
 * actually implement me ;-)
 */
