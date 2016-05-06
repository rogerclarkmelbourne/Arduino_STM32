/*
 * dfu-suffix
 *
 * Copyright 2011-2012 Stefan Schmidt <stefan@datenfreihafen.org>
 * Copyright 2013 Hans Petter Selasky <hps@bitfrost.no>
 * Copyright 2014 Tormod Volden <debian.tormod@gmail.com>
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
	fprintf(stderr, "Usage: dfu-suffix [options] ...\n"
		"  -h --help\t\t\tPrint this help message\n"
		"  -V --version\t\t\tPrint the version number\n"
		"  -c --check <file>\t\tCheck DFU suffix of <file>\n"
		"  -a --add <file>\t\tAdd DFU suffix to <file>\n"
		"  -D --delete <file>\t\tDelete DFU suffix from <file>\n"
		"  -p --pid <productID>\t\tAdd product ID into DFU suffix in <file>\n"
		"  -v --vid <vendorID>\t\tAdd vendor ID into DFU suffix in <file>\n"
		"  -d --did <deviceID>\t\tAdd device ID into DFU suffix in <file>\n"
		"  -S --spec <specID>\t\tAdd DFU specification ID into DFU suffix in <file>\n"
		);
	exit(EX_USAGE);
}

static void print_version(void)
{
	printf("dfu-suffix (%s) %s\n\n", PACKAGE, PACKAGE_VERSION);
	printf("Copyright 2011-2012 Stefan Schmidt, 2013-2014 Tormod Volden\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n"
	       "Please report bugs to %s\n\n", PACKAGE_BUGREPORT);

}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'V' },
	{ "check", 1, 0, 'c' },
	{ "add", 1, 0, 'a' },
	{ "delete", 1, 0, 'D' },
	{ "pid", 1, 0, 'p' },
	{ "vid", 1, 0, 'v' },
	{ "did", 1, 0, 'd' },
	{ "spec", 1, 0, 'S' },
};

int main(int argc, char **argv)
{
	struct dfu_file file;
	int pid, vid, did, spec;
	enum mode mode = MODE_NONE;

	/* make sure all prints are flushed */
	setvbuf(stdout, NULL, _IONBF, 0);

	print_version();

	pid = vid = did = 0xffff;
	spec = 0x0100;			/* Default to bcdDFU version 1.0 */
        memset(&file, 0, sizeof(file));

	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hVc:a:D:p:v:d:S:s:T", opts,
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
		case 'p':
			pid = strtol(optarg, NULL, 16);
			break;
		case 'v':
			vid = strtol(optarg, NULL, 16);
			break;
		case 'd':
			did = strtol(optarg, NULL, 16);
			break;
		case 'S':
			spec = strtol(optarg, NULL, 16);
			break;
		case 'c':
			file.name = optarg;
			mode = MODE_CHECK;
			break;
		case 'a':
			file.name = optarg;
			mode = MODE_ADD;
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

	if (spec != 0x0100 && spec != 0x011a) {
		fprintf(stderr, "Only DFU specification 0x0100 and 0x011a supported\n");
		help();
	}

	switch(mode) {
	case MODE_ADD:
		dfu_load_file(&file, NO_SUFFIX, MAYBE_PREFIX);
		file.idVendor = vid;
		file.idProduct = pid;
		file.bcdDevice = did;
		file.bcdDFU = spec;
		/* always write suffix, rewrite prefix if there was one */
		dfu_store_file(&file, 1, file.size.prefix != 0);
		printf("Suffix successfully added to file\n");
		break;

	case MODE_CHECK:
		dfu_load_file(&file, NEEDS_SUFFIX, MAYBE_PREFIX);
		show_suffix_and_prefix(&file);
		break;

	case MODE_DEL:
		dfu_load_file(&file, NEEDS_SUFFIX, MAYBE_PREFIX);
		dfu_store_file(&file, 0, file.size.prefix != 0);
		if (file.size.suffix) /* had a suffix */
			printf("Suffix successfully removed from file\n");
		break;

	default:
		help();
		break;
	}
	return (0);
}
