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

#include <stdint.h>
#include "utils.h"

/* detect CPU endian */
char cpu_le() {
	const uint32_t cpu_le_test = 0x12345678;
	return ((const unsigned char*)&cpu_le_test)[0] == 0x78;
}

uint32_t be_u32(const uint32_t v) {
	if (cpu_le())
		return	((v & 0xFF000000) >> 24) |
			((v & 0x00FF0000) >>  8) |
			((v & 0x0000FF00) <<  8) |
			((v & 0x000000FF) << 24);
	return v;
}

uint32_t le_u32(const uint32_t v) {
        if (!cpu_le())
                return  ((v & 0xFF000000) >> 24) |
                        ((v & 0x00FF0000) >>  8) |
                        ((v & 0x0000FF00) <<  8) |
                        ((v & 0x000000FF) << 24);
        return v;
}
