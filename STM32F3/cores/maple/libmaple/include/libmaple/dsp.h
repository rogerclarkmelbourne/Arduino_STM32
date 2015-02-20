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
 * @file libmaple/include/libmaple/dsp.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief Convenience macros for the digital signal processing (DSP)
 *				instruction set of the ARM Cortex M4 microcontroller.
 */

#ifndef _LIBMAPLE_DSP_H_
#define _LIBMAPLE_DSP_H_

#ifdef __cplusplus
extern "C"{
#endif

#define DSP3(ID, A, B, C) \
({ \
	uint32_t X; \
	asm volatile (ID" %[res], %[val1], %[val2], %[val3]" \
		: [res]"=r" (X) \
		: [val1]"r" (A), [val2]"r" (B), [val3]"r" (C) \
	); \
	(uint32_t)X; \
})

#define DSP2(ID, A, B) \
({ \
	uint32_t X; \
	asm volatile (ID" %[res], %[val1], %[val2]" \
		: [res]"=r" (X) \
		: [val1]"r" (A), [val2]"r" (B) \
	);\
	(uint32_t)X; \
})

#define DSP1(ID, A) \
({ \
	uint32_t X; \
	asm volatile (ID" %[res], %[val1]" \
		: [res]"=r" (X) \
		: [val1]"r" (A) \
	); \
	(uint32_t)X; \
})

/* General data processing instructions */
#define __rev16(A)				DSP1("REV16", A)
//TODO

/* Multiply and divide instructions */
//TODO

/* Saturating instructions */
//TODO

/* Packing and unpacking instructions */
//TODO

/* Bitfield instructions */
//TODO

/* Floating-point instructions */
//TODO

/* Miscellaneous instructions */
//TODO

/* SIMD instructions (single instruction multiple data) */
#define __sadd16(A, B)		DSP2("SADD16", A, B)
#define __shadd16(A, B)		DSP2("SHADD16", A, B)
#define __ssub16(A, B)		DSP2("SSUB16", A, B)
#define __shsub16(A, B)		DSP2("SHSUB16", A, B)
#define __uadd16(A, B)		DSP2("UADD16", A, B)
#define __uhadd16(A, B)		DSP2("UHADD16", A, B)
#define __usub16(A, B)		DSP2("USUB16", A, B)
#define __uhsub16(A, B)		DSP2("UHSUB16", A, B)

#define __sadd8(A, B)			DSP2("SADD8", A, B)
#define __shadd8(A, B)		DSP2("SHADD8", A, B)
#define __ssub8(A, B)			DSP2("SSUB8", A, B)
#define __shsub8(A, B)		DSP2("SHSUB8", A, B)
#define __uadd8(A, B)			DSP2("UADD8", A, B)
#define __uhadd8(A, B)		DSP2("UHADD8", A, B)
#define __usub8(A, B)			DSP2("USUB8", A, B)
#define __uhsub8(A, B)		DSP2("UHSUB8", A, B)

#define __sasx(A, B)			DSP2("SASX", A, B)
#define __ssax(A, B)			DSP2("SSAX", A, B)
#define __shasx(A, B)			DSP2("SHASX", A, B)
#define __shsax(A, B)			DSP2("SHSAX", A, B)
#define __uasx(A, B)			DSP2("UASX", A, B)
#define __usax(A, B)			DSP2("USAX", A, B)
#define __uhasx(A, B)			DSP2("UHASX", A, B)
#define __uhsax(A, B)			DSP2("UHSAX", A, B)

#define __usad8(A, B)			DSP2("USAD8", A, B)
#define __usada8(A, B, C)	DSP3("USADA8", A, B, C)

/* MAC instructions (multiply and accumulate) */
/* TODO */

#ifdef __cplusplus
}
#endif

#endif
