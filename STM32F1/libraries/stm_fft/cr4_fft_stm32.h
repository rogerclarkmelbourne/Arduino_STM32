/*

x[N] be the time signal samples. To use the FFT functions of the DSP library, the
following conditions must be satisfied:
? All the signal samples must be 32-bit data containing the 16-bit real part followed by the
16-bit imaginary part (in the little Endian order: imaginary_real).


*/

#ifndef __STM32F10x_DSP_H
#define __STM32F10x_DSP_H
/*
 * The assembly files can be modified to use a table in RAM rather than ROM.
 * Check the assembly files comments.
 *
 * #include "table_fft.h"
 */


extern "C" {

/* Radix-4 complex FFT for STM32, in assembly  */
/* 16 points*/
void cr4_fft_16_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);

/* 64 points*/
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);

/* 256 points */
void cr4_fft_256_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);

/* 1024 points */
void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, uint16_t Nbin);


}

#endif /* __STM32F10x_DSP_H */
