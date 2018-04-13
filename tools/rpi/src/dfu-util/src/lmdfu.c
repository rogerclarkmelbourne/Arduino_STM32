/* This implements the TI Stellaris DFU
 * as per the Application Update Using the USB Device Firmware Upgrade Class
 * (Document AN012373)
 * http://www.ti.com/general/docs/lit/getliterature.tsp?literatureNumber=spma003&fileType=pdf
 *
 * Copyright 2012 Tommi Keisala <tommi.keisala@ray.fi>
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
#include <string.h>
#include <errno.h>

#include "portable.h"
#include "dfu.h"
#include "dfu_file.h"
#include "quirks.h"

/* lmdfu_dfu_prefix payload length excludes prefix and suffix */
unsigned char lmdfu_dfu_prefix[] = {
	0x01,			/* STELLARIS_DFU_PROG */
	0x00,			/* Reserved */
	0x00,			/* LSB start address / 1024 */
	0x20,			/* MSB start address / 1024 */
	0x00,			/* LSB file payload length */
	0x00,			/* Byte 2 file payload length */
	0x00,			/* Byte 3 file payload length */
	0x00,			/* MSB file payload length */
};

int lmdfu_add_prefix(struct dfu_file file, unsigned int address)
{
	int ret;
	uint16_t addr;
	uint32_t len;

	unsigned char *data = NULL;

	fseek(file.filep, 0, SEEK_END);
	len = ftell(file.filep);
	rewind(file.filep);

	data = (unsigned char *)malloc(len);
	if (!data) {
		fprintf(stderr, "Unable to allocate buffer.\n");
		exit(1);
	}

	ret = fread(data, 1, len, file.filep);
	if (ret < 0) {
		fprintf(stderr, "Could not read file\n");
		perror(file.name);
		free(data);
		return ret;
	} else if (ret < len) {
		fprintf(stderr, "Could not read whole file\n");
		free(data);
		return -EIO;
	}

	/* fill Stellaris lmdfu_dfu_prefix with correct data */
	addr = address / 1024;
	lmdfu_dfu_prefix[2] = (unsigned char)addr & 0xff;
	lmdfu_dfu_prefix[3] = (unsigned char)addr >> 8;
	lmdfu_dfu_prefix[4] = (unsigned char)len & 0xff;
	lmdfu_dfu_prefix[5] = (unsigned char)(len >> 8) & 0xff;
	lmdfu_dfu_prefix[6] = (unsigned char)(len >> 16) & 0xff;
	lmdfu_dfu_prefix[7] = (unsigned char)(len) >> 24;

	rewind(file.filep);
	ret = fwrite(lmdfu_dfu_prefix, 1, sizeof(lmdfu_dfu_prefix), file.filep);
	if (ret < 0) {
		fprintf(stderr, "Could not write TI Stellaris DFU prefix\n");
		perror(file.name);
	} else if (ret < sizeof(lmdfu_dfu_prefix)) {
		fprintf(stderr, "Could not write while file\n");
		ret = -EIO;
	}

	ret = fwrite(data, 1, len, file.filep);
	if (ret < 0) {
		fprintf(stderr, "Could not write data after TI Stellaris DFU "
			"prefix\n");
		perror(file.name);
	} else if (ret < sizeof(lmdfu_dfu_prefix)) {
		fprintf(stderr, "Could not write whole file\n");
		ret = -EIO;
	}

	rewind(file.filep);
	printf("TI Stellaris DFU prefix added.\n");
	return 0;
}

int lmdfu_remove_prefix(struct dfu_file *file)
{
	long len;
	unsigned char *data;
	int ret;

#ifdef HAVE_FTRUNCATE
	printf("Remove TI Stellaris prefix\n");

	fseek(file->filep, 0, SEEK_END);
	len = ftell(file->filep);
	rewind(file->filep);

	data = (unsigned char *)malloc(len);
	if (!data) {
		fprintf(stderr, "Unable to allocate buffer.\n");
		exit(1);
	}

	ret = fread(data, 1, len, file->filep);
	if (ret < 0) {
		fprintf(stderr, "Could not read file\n");
		perror(file->name);
		free(data);
		return ret;
	} else if (ret < len) {
		fprintf(stderr, "Could not read whole file\n");
		free(data);
		return -EIO;
	}

	ret = ftruncate(fileno(file->filep), 0);
	if (ret < 0) {
		fprintf(stderr, "Error truncating\n");
	}
	rewind(file->filep);

	fwrite(data + sizeof(lmdfu_dfu_prefix), 1, len - sizeof(lmdfu_dfu_prefix),
	       file->filep);

	printf("TI Stellaris prefix removed\n");
#else
	printf("Prefix removal not implemented on this platform\n");
#endif /* HAVE_FTRUNCATE */

	return ret;
}

int lmdfu_check_prefix(struct dfu_file *file)
{
	unsigned char *data;
	int ret;

	data = malloc(sizeof(lmdfu_dfu_prefix));

	ret = fread(data, 1, sizeof(lmdfu_dfu_prefix), file->filep);
	if (ret < sizeof(lmdfu_dfu_prefix)) {
		fprintf(stderr, "Could not read prefix\n");
		perror(file->name);
	}

	if ((data[0] != 0x01) && (data[1] != 0x00)) {
		printf("Not valid TI Stellaris DFU prefix\n");
		ret = 0;
		goto out_rewind;
	} else {
		printf
		    ("Possible TI Stellaris DFU prefix with the following properties\n");
		printf("Address:        0x%08x\n",
		       1024 * (data[3] << 8 | data[2]));
		printf("Payload length: %d\n",
		       data[4] | data[5] << 8 | data[6] << 16 | data[7] << 14);
	}

out_rewind:
	rewind(file->filep);
	return ret;
}
