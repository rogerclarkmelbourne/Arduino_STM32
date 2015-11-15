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
 * @file libmaple/stm32f1/include/series/usart.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F1 USART support.
 */

#ifndef _LIBMAPLE_STM32F1_USART_H_
#define _LIBMAPLE_STM32F1_USART_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Register map base pointers
 */

struct usart_reg_map;

/** USART1 register map base pointer */
#define USART1_BASE                     ((struct usart_reg_map*)0x40013800)
/** USART2 register map base pointer */
#define USART2_BASE                     ((struct usart_reg_map*)0x40004400)
/** USART3 register map base pointer */
#define USART3_BASE                     ((struct usart_reg_map*)0x40004800)
#ifdef STM32_HIGH_DENSITY
/** UART4 register map base pointer */
#define UART4_BASE                      ((struct usart_reg_map*)0x40004C00)
/** UART5 register map base pointer */
#define UART5_BASE                      ((struct usart_reg_map*)0x40005000)
#endif

/*
 * Devices
 */

struct usart_dev;
extern struct usart_dev *USART1;
extern struct usart_dev *USART2;
extern struct usart_dev *USART3;
#ifdef STM32_HIGH_DENSITY
extern struct usart_dev *UART4;
extern struct usart_dev *UART5;
#endif

#ifdef __cplusplus
}
#endif

#endif
