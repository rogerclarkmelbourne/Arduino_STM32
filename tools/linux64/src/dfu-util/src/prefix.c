/*
 * dfu-prefix
 *
 * Copyright 2011-2012 Stefan Schmidt <stefan@datenfreihafen.org>
 * Copyright 2013 Hans Petter Selasky <hps@bitfrost.no>
 * Copyright 2014 Uwe Bonnes <bon@elektron.ikp.physik.tu-darmstadt.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

#include "portable.h"
#include "dfu_file.h"

enum mode {
	MODE_NONE,
	MODE_ADD,
	MODE_DEL,
	MODE_CHECK
};

int verbose;

static void help(void)
{
	fprintf(stderr, "Usage: dfu-prefix [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -V --version\t\t\tPrint the version number\n"
		"  -c --check <file>\t\tCheck DFU prefix of <file>\n"
		"  -D --delete <file>\t\tDelete DFU prefix from <file>\n"
		"  -a --add <file>\t\tAdd DFU prefix to <file>\n"
		"In combination with -a:\n"
		);
	fprintf(stderr, "  -s --stellaris-address <address>  Add TI Stellaris address prefix to <file>\n"
		"In combination with -D or -c:\n"
		"  -T --stellaris\t\tAct on TI Stellaris address prefix of <file>\n"
		"In combination with -a or -D or -c:\n"
		"  -L --lpc-prefix\t\tUse NXP LPC DFU prefix format\n"
		);
	exit(EX_USAGE);
}

static void print_version(void)
{
	printf("dfu-prefix (%s) %s\n\n", PACKAGE, PACKAGE_VERSION);
	printf("Copyright 2011-2012 Stefan Schmidt, 2014 Uwe Bonnes\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n"
	       "Please report bugs to %s\n\n", PACKAGE_BUGREPORT);

}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'V' },
	{ "check", 1, 0, 'c' },
	{ "add", 1, 0, 'a' },
	{ "delete", 1, 0, 'D' },
	{ "stellaris-address", 1, 0, 's' },
	{ "stellaris", 0, 0, 'T' },
	{ "LPC", 0, 0, 'L' },
};
int main(int argc, char **argv)
{
	struct dfu_file file;
	enum mode mode = MODE_NONE;
	enum prefix_type type = ZERO_PREFIX;
	uint32_t lmdfu_flash_address = 0;
	char *end;

	/* make sure all prints are flushed */
	setvbuf(stdout, NULL, _IONBF, 0);

	print_version();

	memset(&file, 0, sizeof(file));

	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hVc:a:D:p:v:d:s:TL", opts,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			help();
			break;
		case 'V':
			exit(0);
			break;
		case 'D':
			file.name = optarg;
			mode = MODE_DEL;
			break;
		case 'c':
			file.name = optarg;
			mode = MODE_CHECK;
			break;
		case 'a':
			file.name = optarg;
			mode = MODE_ADD;
			break;
		case 's':
			lmdfu_flash_address = strtoul(optarg, &end, 0);
			if (*end) {
				errx(EX_IOERR, "Invalid lmdfu "
					"address: %s", optarg);
			}
			/* fall-through */
		case 'T':
			type = LMDFU_PREFIX;
			break;
		case 'L':
			type = LPCDFU_UNENCRYPTED_PREFIX;
			break;
		default:
			help();
			break;
		}
	}

	if (!file.name) {
		fprintf(stderr, "You need to specify a filename\n");
		help();
	}

	switch(mode) {
	case MODE_ADD:
		if (type == ZERO_PREFIX)
			errx(EX_IOERR, "Prefix type must be specified");
		dfu_load_file(&file, MAYBE_SUFFIX, NO_PREFIX);
		file.lmdfu_address = lmdfu_flash_address;
		file.prefix_type = type;
		printf("Adding prefix to file\n");
		dfu_store_file(&file, file.size.suffix != 0, 1);
		break;

	case MODE_CHECK:
		dfu_load_file(&file, MAYBE_SUFFIX, MAYBE_PREFIX);
		show_suffix_and_prefix(&file);
		if (type > ZERO_PREFIX && file.prefix_type != type)
			errx(EX_IOERR, "No prefix of requested type");
		break;

	case MODE_DEL:
		dfu_load_file(&file, MAYBE_SUFFIX, NEEDS_PREFIX);
		if (type > ZERO_PREFIX && file.prefix_type != type)
			errx(EX_IOERR, "No prefix of requested type");
		printf("Removing prefix from file\n");
		/* if there was a suffix, rewrite it */
		dfu_store_file(&file, file.size.suffix != 0, 0);
		break;

	default:
		help();
		break;
	}
	return (0);
}
