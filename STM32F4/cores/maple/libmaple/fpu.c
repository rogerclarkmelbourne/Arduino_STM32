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
 * @file libmaple/stm32f3/fpu.c
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 FPU.
 *
 * revision history:
 * added comments making the asm codes more readable - andrew, Jun 2019
 */

#include <libmaple/fpu.h>

// Enable the FPU (Cortex-M4 - STM32F4xx and higher)
// http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/BEHBJHIG.html

void fpu_enable(void) {
	/*
	 * set coprocessor access control registers
	 */
	asm("\tLDR.W	R0, =0xE000ED88\n" 			/* The FPU enable bits are in the CPACR. */
		"\tLDR		R1, [R0]\n"					/* read CAPCR */
		"\tORR		R1, R1, #(0xF << 20)\n"		/* Set bits 20-23 to enable CP10 and CP11 coprocessors */
		"\tSTR		R1, [R0]\n"					/* Write back the modified value to the CPACR */
		"\tDSB\n"								/* wait for store to complete */
		"\tISB");								/* reset pipeline now the FPU is enabled */
}

void fpu_disable(void) {
	/*
	 * clear coprocessor access control registers
	 */
	asm("\tLDR.W	R0, =0xE000ED88\n"			/* The FPU enable bits are in the CPACR. */
		"\tLDR		R1, [R0]\n"					/* read CAPCR */
		"\tORR		R1, R1, #(0x0 << 20)\n"		/* Set bits 20-23 to enable CP10 and CP11 coprocessors */
		"\tSTR		R1, [R0]\n"					/* Write back the modified value to the CPACR */
		"\tDSB\n"								/* wait for store to complete */
		"\tISB");								/* reset pipeline now the FPU is enabled */
}
