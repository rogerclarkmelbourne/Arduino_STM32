/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stm32f10x.h>

#include "bitwise.h"

void led_off() {
	GPIOC->BSRR = GPIO_BSRR_BS13; // Set PC_13 HIGH (open drain)
}

void led_on() {
	GPIOC->BRR = GPIO_BRR_BR13; // Set PC_13 LOW (Reset)
}

void led_init() {
	// I/O port C clock enable
	bit_set(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);

	// PC_13 output mode: General purpose output open drain (b01)
	bit_set(GPIOC->CRH, GPIO_CRH_CNF13_0);
	bit_clear(GPIOC->CRH, GPIO_CRH_CNF13_1);

	// Set PC_13 to output
	bit_set(GPIOC->CRH, GPIO_CRH_MODE13);  // PC_13 set as: Output mode, max speed 50 MHz.
}
