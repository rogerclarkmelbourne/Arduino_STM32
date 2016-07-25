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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "binary.h"

typedef struct {
	int		fd;
	char		write;
	struct stat	stat;
} binary_t;

void* binary_init() {
	return calloc(sizeof(binary_t), 1);
}

parser_err_t binary_open(void *storage, const char *filename, const char write) {
	binary_t *st = storage;
	if (write) {
		if (filename[0] == '-')
			st->fd = 1;
		else
			st->fd = open(
				filename,
#ifndef __WIN32__
				O_WRONLY | O_CREAT | O_TRUNC,
#else
				O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
#endif
#ifndef __WIN32__
				S_IRUSR  | S_IWUSR | S_IRGRP | S_IROTH
#else
				0
#endif
			);
		st->stat.st_size = 0;
	} else {
		if (filename[0] == '-') {
			st->fd = 0;
		} else {
			if (stat(filename, &st->stat) != 0)
				return PARSER_ERR_INVALID_FILE;
			st->fd = open(filename,
#ifndef __WIN32__
				O_RDONLY
#else
				O_RDONLY | O_BINARY
#endif
			);
		}
	}

	st->write = write;
	return st->fd == -1 ? PARSER_ERR_SYSTEM : PARSER_ERR_OK;
}

parser_err_t binary_close(void *storage) {
	binary_t *st = storage;

	if (st->fd) close(st->fd);
	free(st);
	return PARSER_ERR_OK;
}

unsigned int binary_size(void *storage) {
	binary_t *st = storage;
	return st->stat.st_size;
}

parser_err_t binary_read(void *storage, void *data, unsigned int *len) {
	binary_t *st = storage;
	unsigned int left = *len;
	if (st->write) return PARSER_ERR_WRONLY;

	ssize_t r;
	while(left > 0) {
		r = read(st->fd, data, left);
		/* If there is no data to read at all, return OK, but with zero read */
		if (r == 0 && left == *len) {
			*len = 0;
			return PARSER_ERR_OK;
		}
		if (r <= 0) return PARSER_ERR_SYSTEM;
		left -= r;
		data += r;
	}

	*len = *len - left;
	return PARSER_ERR_OK;
}

parser_err_t binary_write(void *storage, void *data, unsigned int len) {
	binary_t *st = storage;
	if (!st->write) return PARSER_ERR_RDONLY;

	ssize_t r;
	while(len > 0) {
		r = write(st->fd, data, len);
		if (r < 1) return PARSER_ERR_SYSTEM;
		st->stat.st_size += r;

		len  -= r;
		data += r;
	}

	return PARSER_ERR_OK;
}

parser_t PARSER_BINARY = {
	"Raw BINARY",
	binary_init,
	binary_open,
	binary_close,
	binary_size,
	binary_read,
	binary_write
};

