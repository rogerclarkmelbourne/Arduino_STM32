/*
 * (C) 2011 - 2012 Stefan Schmidt <stefan@datenfreihafen.org>
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

#include "dfu_file.h"
#include "lmdfu.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef HAVE_FTRUNCATE
# include <unistd.h>
#endif

enum mode {
	MODE_NONE,
	MODE_ADD,
	MODE_DEL,
	MODE_CHECK
};

enum lmdfu_mode {
	LMDFU_NONE,
	LMDFU_ADD,
	LMDFU_DEL,
	LMDFU_CHECK
};

static void help(void)
{
	printf("Usage: dfu-suffix [options] <file>\n"
		"  -h --help\tPrint this help message\n"
		"  -V --version\tPrint the version number\n"
		"  -D --delete\tDelete DFU suffix from <file>\n"
		"  -p --pid\tAdd product ID into DFU suffix in <file>\n"
		"  -v --vid\tAdd vendor ID into DFU suffix in <file>\n"
		"  -d --did\tAdd device ID into DFU suffix in <file>\n"
		"  -c --check\tCheck DFU suffix of <file>\n"
		"  -a --add\tAdd DFU suffix to <file>\n"
		);
	printf( "  -s --stellaris-address <address>  Add TI Stellaris address "
		"prefix to <file>,\n\t\tto be used together with -a\n"
		"  -T --stellaris  Act on TI Stellaris extension prefix of "
		"<file>, to be used\n\t\tin combination with -D or -c\n"
		);
}

static void print_version(void)
{
	printf("dfu-suffix (" PACKAGE ") " PACKAGE_VERSION "\n\n");
	printf("(C) 2011-2012 Stefan Schmidt\n"
	       "This program is Free Software and has ABSOLUTELY NO WARRANTY\n"
	       "Please report bugs to " PACKAGE_BUGREPORT "\n\n");

}

static struct option opts[] = {
	{ "help", 0, 0, 'h' },
	{ "version", 0, 0, 'V' },
	{ "delete", 1, 0, 'D' },
	{ "pid", 1, 0, 'p' },
	{ "vid", 1, 0, 'v' },
	{ "did", 1, 0, 'd' },
	{ "check", 1, 0, 'c' },
	{ "add", 1, 0, 'a' },
	{ "stellaris-address", 1, 0, 's' },
	{ "stellaris", 0, 0, 'T' },
};

static int check_suffix(struct dfu_file *file) {
	int ret;

	ret = parse_dfu_suffix(file);
	if (ret > 0) {
		printf("The file %s contains a DFU suffix with the following properties:\n", file->name);
		printf("BCD device:\t0x%04X\n", file->bcdDevice);
		printf("Product ID:\t0x%04X\n",file->idProduct);
		printf("Vendor ID:\t0x%04X\n", file->idVendor);
		printf("BCD DFU:\t0x%04X\n", file->bcdDFU);
		printf("Length:\t\t%i\n", file->suffixlen);
		printf("CRC:\t\t0x%08X\n", file->dwCRC);
	}

	return ret;
}

static int remove_suffix(struct dfu_file *file)
{
	int ret;

	ret = parse_dfu_suffix(file);
	if (ret <= 0)
		return 0;

#ifdef HAVE_FTRUNCATE
	/* There is no easy way to truncate to a size with stdio */
	ret = ftruncate(fileno(file->filep),
			(long) file->size - file->suffixlen);
	if (ret < 0) {
		perror("ftruncate");
		exit(1);
	}
	printf("DFU suffix removed\n");
#else
	printf("Suffix removal not implemented on this platform\n");
#endif /* HAVE_FTRUNCATE */
	return 1;
}

static void add_suffix(struct dfu_file *file, int pid, int vid, int did) {
	int ret;

	file->idProduct = pid;
	file->idVendor = vid;
	file->bcdDevice = did;

	ret = generate_dfu_suffix(file);
	if (ret < 0) {
		perror("generate");
		exit(1);
	}
	printf("New DFU suffix added.\n");
}

int main(int argc, char **argv)
{
	struct dfu_file file;
	int pid, vid, did;
	enum mode mode = MODE_NONE;
	enum lmdfu_mode lmdfu_mode = LMDFU_NONE;
	unsigned int lmdfu_flash_address=0;
	int lmdfu_prefix=0;
	char *end;

	print_version();

	pid = vid = did = 0xffff;
	file.name = NULL;

	while (1) {
		int c, option_index = 0;
		c = getopt_long(argc, argv, "hVD:p:v:d:c:a:s:T", opts,
				&option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'h':
			help();
			exit(0);
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
		case 'c':
			file.name = optarg;
			mode = MODE_CHECK;
			break;
		case 'a':
			file.name = optarg;
			mode = MODE_ADD;
			break;
		case 's':
			lmdfu_mode = LMDFU_ADD;
			lmdfu_flash_address = strtoul(optarg, &end, 0);
			if (*end) {
				fprintf(stderr, "Error: Invalid lmdfu "
					"address: %s\n", optarg);
				exit(2);
			}
			break;
		case 'T':
			lmdfu_mode = LMDFU_CHECK; /* or LMDFU_DEL */
			break;
		default:
			help();
			exit(2);
		}
	}

	if(mode == MODE_DEL && lmdfu_mode == LMDFU_CHECK)
		lmdfu_mode = LMDFU_DEL;

	if (!file.name) {
		fprintf(stderr, "You need to specify a filename\n");
		help();
		exit(2);
	}

	if (mode != MODE_NONE) {
		file.filep = fopen(file.name, "r+b");
		if (file.filep == NULL) {
			perror(file.name);
			exit(1);
		}
	}

	switch(mode) {
	case MODE_ADD:
		if (check_suffix(&file)) {
			if(lmdfu_prefix) lmdfu_check_prefix(&file);
			printf("Please remove existing DFU suffix before adding a new one.\n");
			exit(1);
		}
		if(lmdfu_mode == LMDFU_ADD) {
			if(lmdfu_check_prefix(&file)) {
				fprintf(stderr, "Adding new anyway\n");
			}
			lmdfu_add_prefix(file, lmdfu_flash_address);
		}
		add_suffix(&file, pid, vid, did);
		break;
	case MODE_CHECK:
		/* FIXME: could open read-only here */
		check_suffix(&file);
		if(lmdfu_mode == LMDFU_CHECK)
			lmdfu_check_prefix(&file);
		break;
	case MODE_DEL:
		if(!remove_suffix(&file)) {
			if(lmdfu_mode == LMDFU_DEL)
				if (lmdfu_check_prefix(&file))
					lmdfu_remove_prefix(&file);
			exit(1);
		}
		break;
	default:
		help();
		exit(2);
	}

	if(lmdfu_mode == LMDFU_DEL) {
		if (check_suffix(&file)) {
			fprintf(stderr, "DFU suffix exist. Remove suffix before using -T or use it with -D to delete suffix\n");
			exit(1);
		} else {
			if(lmdfu_check_prefix(&file))
				lmdfu_remove_prefix(&file);
		}
	}

	fclose(file.filep);
	exit(0);
}
