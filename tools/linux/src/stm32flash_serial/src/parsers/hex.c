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


#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hex.h"
#include "../utils.h"

typedef struct {
	size_t		data_len, offset;
	uint8_t		*data;
	uint8_t		base;
} hex_t;

void* hex_init() {
	return calloc(sizeof(hex_t), 1);
}

parser_err_t hex_open(void *storage, const char *filename, const char write) {
	hex_t *st = storage;
	if (write) {
		return PARSER_ERR_RDONLY;
	} else {
		char mark;
		int i, fd;
		uint8_t checksum;
		unsigned int c;
		uint32_t base = 0;
		unsigned int last_address = 0x0;

		fd = open(filename, O_RDONLY);
		if (fd < 0)
			return PARSER_ERR_SYSTEM;

		/* read in the file */

		while(read(fd, &mark, 1) != 0) {
			if (mark == '\n' || mark == '\r') continue;
			if (mark != ':')
				return PARSER_ERR_INVALID_FILE;

			char buffer[9];
			unsigned int reclen, address, type;
			uint8_t *record = NULL;

			/* get the reclen, address, and type */
			buffer[8] = 0;
			if (read(fd, &buffer, 8) != 8) return PARSER_ERR_INVALID_FILE;
			if (sscanf(buffer, "%2x%4x%2x", &reclen, &address, &type) != 3) {
				close(fd);
				return PARSER_ERR_INVALID_FILE;
			}

			/* setup the checksum */
			checksum =
				reclen +
				((address & 0xFF00) >> 8) +
				((address & 0x00FF) >> 0) +
				type;

			switch(type) {
				/* data record */
				case 0:
					c = address - last_address;
					st->data = realloc(st->data, st->data_len + c + reclen);

					/* if there is a gap, set it to 0xff and increment the length */
					if (c > 0) {
						memset(&st->data[st->data_len], 0xff, c);
						st->data_len += c;
					}

					last_address = address + reclen;
					record = &st->data[st->data_len];
					st->data_len += reclen;
					break;

				/* extended segment address record */
				case 2:
					base = 0;
					break;

				/* extended linear address record */
				case 4:
					base = address;
					break;
			}

			buffer[2] = 0;
			for(i = 0; i < reclen; ++i) {
				if (read(fd, &buffer, 2) != 2 || sscanf(buffer, "%2x", &c) != 1) {
					close(fd);
					return PARSER_ERR_INVALID_FILE;
				}

				/* add the byte to the checksum */
				checksum += c;

				switch(type) {
					case 0:
						if (record != NULL) {
							record[i] = c;
						} else {
							return PARSER_ERR_INVALID_FILE;
						}
						break;

					case 2:
					case 4:
						base = (base << 8) | c;
						break;
				}
			}

			/* read, scan, and verify the checksum */
			if (
				read(fd, &buffer, 2 ) != 2 ||
				sscanf(buffer, "%2x", &c) != 1 ||
				(uint8_t)(checksum + c) != 0x00
			) {
				close(fd);
				return PARSER_ERR_INVALID_FILE;
			}

			switch(type) {
				/* EOF */
				case 1:
					close(fd);
					return PARSER_ERR_OK;

				/* address record */
				case 2: base = base << 4;
				case 4:	base = be_u32(base);
					/* Reset last_address since our base changed */
					last_address = 0;

					if (st->base == 0) {
						st->base = base;
						break;
					}

					/* we cant cope with files out of order */
					if (base < st->base) {
						close(fd);
						return PARSER_ERR_INVALID_FILE;
					}

					/* if there is a gap, enlarge and fill with zeros */
					unsigned int len = base - st->base;
					if (len > st->data_len) {
						st->data = realloc(st->data, len);
						memset(&st->data[st->data_len], 0, len - st->data_len);
						st->data_len = len;
					}
					break;
			}
		}

		close(fd);
		return PARSER_ERR_OK;
	}
}

parser_err_t hex_close(void *storage) {
	hex_t *st = storage;
	if (st) free(st->data);
	free(st);
	return PARSER_ERR_OK;
}

unsigned int hex_size(void *storage) {
	hex_t *st = storage;
	return st->data_len;
}

parser_err_t hex_read(void *storage, void *data, unsigned int *len) {
	hex_t *st = storage;
	unsigned int left = st->data_len - st->offset;
	unsigned int get  = left > *len ? *len : left;

	memcpy(data, &st->data[st->offset], get);
	st->offset += get;

	*len = get;
	return PARSER_ERR_OK;
}

parser_err_t hex_write(void *storage, void *data, unsigned int len) {
	return PARSER_ERR_RDONLY;
}

parser_t PARSER_HEX = {
	"Intel HEX",
	hex_init,
	hex_open,
	hex_close,
	hex_size,
	hex_read,
	hex_write
};

