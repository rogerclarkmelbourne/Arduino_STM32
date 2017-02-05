/*
 * Load or store DFU files including suffix and prefix
 *
 * Copyright 2014 Tormod Volden <debian.tormod@gmail.com>
 * Copyright 2012 Stefan Schmidt <stefan@datenfreihafen.org>
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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

#include "portable.h"
#include "dfu_file.h"

#define DFU_SUFFIX_LENGTH 16
#define LMDFU_PREFIX_LENGTH 8
#define LPCDFU_PREFIX_LENGTH 16
#define PROGRESS_BAR_WIDTH 25
#define STDIN_CHUNK_SIZE 65536

static const unsigned long crc32_table[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

static uint32_t crc32_byte(uint32_t accum, uint8_t delta)
{
        return crc32_table[(accum ^ delta) & 0xff] ^ (accum >> 8);
}

static int probe_prefix(struct dfu_file *file)
{
	uint8_t *prefix = file->firmware;

	if (file->size.total <  LMDFU_PREFIX_LENGTH)
		return 1;
	if ((prefix[0] == 0x01) && (prefix[1] == 0x00)) {
		file->prefix_type = LMDFU_PREFIX;
		file->size.prefix = LMDFU_PREFIX_LENGTH;
		file->lmdfu_address = 1024 * ((prefix[3] << 8) | prefix[2]);
	}
	else if (((prefix[0] & 0x3f) == 0x1a) && ((prefix[1] & 0x3f)== 0x3f)) {
		file->prefix_type = LPCDFU_UNENCRYPTED_PREFIX;
		file->size.prefix = LPCDFU_PREFIX_LENGTH;
	}

	if (file->size.prefix + file->size.suffix > file->size.total)
		return 1;
	return 0;
}

void dfu_progress_bar(const char *desc, unsigned long long curr,
		unsigned long long max)
{
	static char buf[PROGRESS_BAR_WIDTH + 1];
	static unsigned long long last_progress = -1;
	static time_t last_time;
	time_t curr_time = time(NULL);
	unsigned long long progress;
	unsigned long long x;

	/* check for not known maximum */
	if (max < curr)
		max = curr + 1;
	/* make none out of none give zero */
	if (max == 0 && curr == 0)
		max = 1;

	/* compute completion */
	progress = (PROGRESS_BAR_WIDTH * curr) / max;
	if (progress > PROGRESS_BAR_WIDTH)
		progress = PROGRESS_BAR_WIDTH;
	if (progress == last_progress &&
	    curr_time == last_time)
		return;
	last_progress = progress;
	last_time = curr_time;

	for (x = 0; x != PROGRESS_BAR_WIDTH; x++) {
		if (x < progress)
			buf[x] = '=';
		else
			buf[x] = ' ';
	}
	buf[x] = 0;

	printf("\r%s\t[%s] %3lld%% %12lld bytes", desc, buf,
	    (100ULL * curr) / max, curr);

	if (progress == PROGRESS_BAR_WIDTH)
		printf("\n%s done.\n", desc);
}

void *dfu_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL)
		errx(EX_SOFTWARE, "Cannot allocate memory of size %d bytes", (int)size);
	return (ptr);
}

uint32_t dfu_file_write_crc(int f, uint32_t crc, const void *buf, int size)
{
	int x;

	/* compute CRC */
	for (x = 0; x != size; x++)
		crc = crc32_byte(crc, ((uint8_t *)buf)[x]);

	/* write data */
	if (write(f, buf, size) != size)
		err(EX_IOERR, "Could not write %d bytes to file %d", size, f);

	return (crc);
}

void dfu_load_file(struct dfu_file *file, enum suffix_req check_suffix, enum prefix_req check_prefix)
{
	off_t offset;
	int f;
	int i;
	int res;

	file->size.prefix = 0;
	file->size.suffix = 0;

	/* default values, if no valid suffix is found */
	file->bcdDFU = 0;
	file->idVendor = 0xffff; /* wildcard value */
	file->idProduct = 0xffff; /* wildcard value */
	file->bcdDevice = 0xffff; /* wildcard value */

	/* default values, if no valid prefix is found */
	file->lmdfu_address = 0;

	free(file->firmware);

	if (!strcmp(file->name, "-")) {
		int read_bytes;

#ifdef WIN32
		_setmode( _fileno( stdin ), _O_BINARY );
#endif
		file->firmware = (uint8_t*) dfu_malloc(STDIN_CHUNK_SIZE);
		read_bytes = fread(file->firmware, 1, STDIN_CHUNK_SIZE, stdin);
		file->size.total = read_bytes;
		while (read_bytes == STDIN_CHUNK_SIZE) {
			file->firmware = (uint8_t*) realloc(file->firmware, file->size.total + STDIN_CHUNK_SIZE);
			if (!file->firmware)
				err(EX_IOERR, "Could not allocate firmware buffer");
			read_bytes = fread(file->firmware + file->size.total, 1, STDIN_CHUNK_SIZE, stdin);
			file->size.total += read_bytes;
		}
		if (verbose)
			printf("Read %i bytes from stdin\n", file->size.total);
		/* Never require suffix when reading from stdin */
		check_suffix = MAYBE_SUFFIX;
	} else {
		f = open(file->name, O_RDONLY | O_BINARY);
		if (f < 0)
			err(EX_IOERR, "Could not open file %s for reading", file->name);

		offset = lseek(f, 0, SEEK_END);

		if ((int)offset < 0 || (int)offset != offset)
			err(EX_IOERR, "File size is too big");

		if (lseek(f, 0, SEEK_SET) != 0)
			err(EX_IOERR, "Could not seek to beginning");

		file->size.total = offset;
		file->firmware = dfu_malloc(file->size.total);

		if (read(f, file->firmware, file->size.total) != file->size.total) {
			err(EX_IOERR, "Could not read %d bytes from %s",
			    file->size.total, file->name);
		}
		close(f);
	}

	/* Check for possible DFU file suffix by trying to parse one */
	{
		uint32_t crc = 0xffffffff;
		const uint8_t *dfusuffix;
		int missing_suffix = 0;
		const char *reason;

		if (file->size.total < DFU_SUFFIX_LENGTH) {
			reason = "File too short for DFU suffix";
			missing_suffix = 1;
			goto checked;
		}

		dfusuffix = file->firmware + file->size.total -
		    DFU_SUFFIX_LENGTH;

		for (i = 0; i < file->size.total - 4; i++)
			crc = crc32_byte(crc, file->firmware[i]);

		if (dfusuffix[10] != 'D' ||
		    dfusuffix[9]  != 'F' ||
		    dfusuffix[8]  != 'U') {
			reason = "Invalid DFU suffix signature";
			missing_suffix = 1;
			goto checked;
		}

		file->dwCRC = (dfusuffix[15] << 24) +
		    (dfusuffix[14] << 16) +
		    (dfusuffix[13] << 8) +
		    dfusuffix[12];

		if (file->dwCRC != crc) {
			reason = "DFU suffix CRC does not match";
			missing_suffix = 1;
			goto checked;
		}

		/* At this point we believe we have a DFU suffix
		   so we require further checks to succeed */

		file->bcdDFU = (dfusuffix[7] << 8) + dfusuffix[6];

		if (verbose)
			printf("DFU suffix version %x\n", file->bcdDFU);

		file->size.suffix = dfusuffix[11];

		if (file->size.suffix < DFU_SUFFIX_LENGTH) {
			errx(EX_IOERR, "Unsupported DFU suffix length %d",
			    file->size.suffix);
		}

		if (file->size.suffix > file->size.total) {
			errx(EX_IOERR, "Invalid DFU suffix length %d",
			    file->size.suffix);
		}

		file->idVendor	= (dfusuffix[5] << 8) + dfusuffix[4];
		file->idProduct = (dfusuffix[3] << 8) + dfusuffix[2];
		file->bcdDevice = (dfusuffix[1] << 8) + dfusuffix[0];

checked:
		if (missing_suffix) {
			if (check_suffix == NEEDS_SUFFIX) {
				warnx("%s", reason);
				errx(EX_IOERR, "Valid DFU suffix needed");
			} else if (check_suffix == MAYBE_SUFFIX) {
				warnx("%s", reason);
				warnx("A valid DFU suffix will be required in "
				      "a future dfu-util release!!!");
			}
		} else {
			if (check_suffix == NO_SUFFIX) {
				errx(EX_SOFTWARE, "Please remove existing DFU suffix before adding a new one.\n");
			}
		}
	}
	res = probe_prefix(file);
	if ((res || file->size.prefix == 0) && check_prefix == NEEDS_PREFIX)
		errx(EX_IOERR, "Valid DFU prefix needed");
	if (file->size.prefix && check_prefix == NO_PREFIX)
		errx(EX_IOERR, "A prefix already exists, please delete it first");
	if (file->size.prefix && verbose) {
		uint8_t *data = file->firmware;
		if (file->prefix_type == LMDFU_PREFIX)
			printf("Possible TI Stellaris DFU prefix with "
				   "the following properties\n"
				   "Address:        0x%08x\n"
				   "Payload length: %d\n",
				   file->lmdfu_address,
				   data[4] | (data[5] << 8) |
				   (data[6] << 16) | (data[7] << 14));
		else if (file->prefix_type == LPCDFU_UNENCRYPTED_PREFIX)
			printf("Possible unencrypted NXP LPC DFU prefix with "
				   "the following properties\n"
				   "Payload length: %d kiByte\n",
				   data[2] >>1 | (data[3] << 7) );
		else
			errx(EX_IOERR, "Unknown DFU prefix type");
	}
}

void dfu_store_file(struct dfu_file *file, int write_suffix, int write_prefix)
{
	uint32_t crc = 0xffffffff;
	int f;

	f = open(file->name, O_WRONLY | O_BINARY | O_TRUNC | O_CREAT, 0666);
	if (f < 0)
		err(EX_IOERR, "Could not open file %s for writing", file->name);

	/* write prefix, if any */
	if (write_prefix) {
		if (file->prefix_type == LMDFU_PREFIX) {
			uint8_t lmdfu_prefix[LMDFU_PREFIX_LENGTH];
			uint32_t addr = file->lmdfu_address / 1024;

			/* lmdfu_dfu_prefix payload length excludes prefix and suffix */
			uint32_t len = file->size.total -
				file->size.prefix - file->size.suffix;

			lmdfu_prefix[0] = 0x01; /* STELLARIS_DFU_PROG */
			lmdfu_prefix[1] = 0x00; /* Reserved */
			lmdfu_prefix[2] = (uint8_t)(addr & 0xff);
			lmdfu_prefix[3] = (uint8_t)(addr >> 8);
			lmdfu_prefix[4] = (uint8_t)(len & 0xff);
			lmdfu_prefix[5] = (uint8_t)(len >> 8) & 0xff;
			lmdfu_prefix[6] = (uint8_t)(len >> 16) & 0xff;
			lmdfu_prefix[7] = (uint8_t)(len >> 24);

			crc = dfu_file_write_crc(f, crc, lmdfu_prefix, LMDFU_PREFIX_LENGTH);
		}
		if (file->prefix_type == LPCDFU_UNENCRYPTED_PREFIX) {
			uint8_t lpcdfu_prefix[LPCDFU_PREFIX_LENGTH] = {0};
			int i;

			/* Payload is firmware and prefix rounded to 512 bytes */
			uint32_t len = (file->size.total - file->size.suffix + 511) /512;

			lpcdfu_prefix[0] = 0x1a; /* Unencypted*/
			lpcdfu_prefix[1] = 0x3f; /* Reserved */
			lpcdfu_prefix[2] = (uint8_t)(len & 0xff);
			lpcdfu_prefix[3] = (uint8_t)((len >> 8) & 0xff);
			for (i = 12; i < LPCDFU_PREFIX_LENGTH; i++)
				lpcdfu_prefix[i] = 0xff;

			crc = dfu_file_write_crc(f, crc, lpcdfu_prefix, LPCDFU_PREFIX_LENGTH);
		}
	}
	/* write firmware binary */
	crc = dfu_file_write_crc(f, crc, file->firmware + file->size.prefix,
	    file->size.total - file->size.prefix - file->size.suffix);

	/* write suffix, if any */
	if (write_suffix) {
		uint8_t dfusuffix[DFU_SUFFIX_LENGTH];

		dfusuffix[0] = file->bcdDevice & 0xff;
		dfusuffix[1] = file->bcdDevice >> 8;
		dfusuffix[2] = file->idProduct & 0xff;
		dfusuffix[3] = file->idProduct >> 8;
		dfusuffix[4] = file->idVendor & 0xff;
		dfusuffix[5] = file->idVendor >> 8;
		dfusuffix[6] = file->bcdDFU & 0xff;
		dfusuffix[7] = file->bcdDFU >> 8;
		dfusuffix[8] = 'U';
		dfusuffix[9] = 'F';
		dfusuffix[10] = 'D';
		dfusuffix[11] = DFU_SUFFIX_LENGTH;

		crc = dfu_file_write_crc(f, crc, dfusuffix,
		    DFU_SUFFIX_LENGTH - 4);

		dfusuffix[12] = crc;
		dfusuffix[13] = crc >> 8;
		dfusuffix[14] = crc >> 16;
		dfusuffix[15] = crc >> 24;

		crc = dfu_file_write_crc(f, crc, dfusuffix + 12, 4);
	}
	close(f);
}

void show_suffix_and_prefix(struct dfu_file *file)
{
	if (file->size.prefix == LMDFU_PREFIX_LENGTH) {
		printf("The file %s contains a TI Stellaris DFU prefix with the following properties:\n", file->name);
		printf("Address:\t0x%08x\n", file->lmdfu_address);
	} else if (file->size.prefix == LPCDFU_PREFIX_LENGTH) {
		uint8_t * prefix = file->firmware;
		printf("The file %s contains a NXP unencrypted LPC DFU prefix with the following properties:\n", file->name);
		printf("Size:\t%5d kiB\n", prefix[2]>>1|prefix[3]<<7);
	} else if (file->size.prefix != 0) {
		printf("The file %s contains an unknown prefix\n", file->name);
	}
	if (file->size.suffix > 0) {
		printf("The file %s contains a DFU suffix with the following properties:\n", file->name);
		printf("BCD device:\t0x%04X\n", file->bcdDevice);
		printf("Product ID:\t0x%04X\n",file->idProduct);
		printf("Vendor ID:\t0x%04X\n", file->idVendor);
		printf("BCD DFU:\t0x%04X\n", file->bcdDFU);
		printf("Length:\t\t%i\n", file->size.suffix);
		printf("CRC:\t\t0x%08X\n", file->dwCRC);
	}
}
