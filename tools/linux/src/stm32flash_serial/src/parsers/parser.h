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


#ifndef _H_PARSER
#define _H_PARSER

enum parser_err {
	PARSER_ERR_OK,
	PARSER_ERR_SYSTEM,
	PARSER_ERR_INVALID_FILE,
	PARSER_ERR_WRONLY,
	PARSER_ERR_RDONLY
};
typedef enum   parser_err parser_err_t;

struct parser {
	const char *name;
	void*        (*init )();							/* initialise the parser */
	parser_err_t (*open )(void *storage, const char *filename, const char write);	/* open the file for read|write */
	parser_err_t (*close)(void *storage);						/* close and free the parser */
	unsigned int (*size )(void *storage);						/* get the total data size */
	parser_err_t (*read )(void *storage, void *data, unsigned int *len);		/* read a block of data */
	parser_err_t (*write)(void *storage, void *data, unsigned int len);		/* write a block of data */
};
typedef struct parser     parser_t;

static inline const char* parser_errstr(parser_err_t err) {
	switch(err) {
		case PARSER_ERR_OK          : return "OK";
		case PARSER_ERR_SYSTEM      : return "System Error";
		case PARSER_ERR_INVALID_FILE: return "Invalid File";
		case PARSER_ERR_WRONLY      : return "Parser can only write";
		case PARSER_ERR_RDONLY      : return "Parser can only read";
		default:
			return "Unknown Error";
	}
}

#endif
