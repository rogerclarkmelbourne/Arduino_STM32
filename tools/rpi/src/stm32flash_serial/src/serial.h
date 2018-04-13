/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2010 Geoffrey McRae <geoff@spacevs.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef _SERIAL_H
#define _SERIAL_H

typedef struct serial serial_t;

typedef enum {
	SERIAL_PARITY_NONE,
	SERIAL_PARITY_EVEN,
	SERIAL_PARITY_ODD,

	SERIAL_PARITY_INVALID
} serial_parity_t;

typedef enum {
	SERIAL_BITS_5,
	SERIAL_BITS_6,
	SERIAL_BITS_7,
	SERIAL_BITS_8,

	SERIAL_BITS_INVALID
} serial_bits_t;

typedef enum {
	SERIAL_BAUD_1200,
	SERIAL_BAUD_1800,
	SERIAL_BAUD_2400,
	SERIAL_BAUD_4800,
	SERIAL_BAUD_9600,
	SERIAL_BAUD_19200,
	SERIAL_BAUD_38400,
	SERIAL_BAUD_57600,
	SERIAL_BAUD_115200,
	SERIAL_BAUD_128000,
	SERIAL_BAUD_230400,
	SERIAL_BAUD_256000,
	SERIAL_BAUD_460800,
	SERIAL_BAUD_500000,
	SERIAL_BAUD_576000,
	SERIAL_BAUD_921600,
	SERIAL_BAUD_1000000,
	SERIAL_BAUD_1500000,
	SERIAL_BAUD_2000000,

	SERIAL_BAUD_INVALID
} serial_baud_t;

typedef enum {
	SERIAL_STOPBIT_1,
	SERIAL_STOPBIT_2,

	SERIAL_STOPBIT_INVALID
} serial_stopbit_t;

typedef enum {
	GPIO_RTS = 1,
	GPIO_DTR,
	GPIO_BRK,
} serial_gpio_t;

/* common helper functions */
serial_baud_t serial_get_baud(const unsigned int baud);
unsigned int serial_get_baud_int(const serial_baud_t baud);
serial_bits_t serial_get_bits(const char *mode);
unsigned int serial_get_bits_int(const serial_bits_t bits);
serial_parity_t serial_get_parity(const char *mode);
char serial_get_parity_str(const serial_parity_t parity);
serial_stopbit_t serial_get_stopbit(const char *mode);
unsigned int serial_get_stopbit_int(const serial_stopbit_t stopbit);

#endif
