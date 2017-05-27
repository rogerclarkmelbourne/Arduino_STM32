/* This implements the ST Microsystems DFU extensions (DfuSe)
 * as per the DfuSe 1.1a specification (ST documents AN3156, AN2606)
 * The DfuSe file format is described in ST document UM0391.
 *
 * (C) 2010-2012 Tormod Volden <debian.tormod@gmail.com>
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "portable.h"
#include "dfu.h"
#include "usb_dfu.h"
#include "dfu_file.h"
#include "dfuse.h"
#include "dfuse_mem.h"

#define DFU_TIMEOUT 5000

extern int verbose;
static unsigned int last_erased = 0;
static struct memsegment *mem_layout;
static unsigned int dfuse_address = 0;
static unsigned int dfuse_length = 0;
static int dfuse_force = 0;
static int dfuse_leave = 0;
static int dfuse_unprotect = 0;
static int dfuse_mass_erase = 0;

unsigned int quad2uint(unsigned char *p)
{
	return (*p + (*(p + 1) << 8) + (*(p + 2) << 16) + (*(p + 3) << 24));
}

void dfuse_parse_options(const char *options)
{
	char *end;
	const char *endword;
	unsigned int number;

	/* address, possibly empty, must be first */
	if (*options != ':') {
		endword = strchr(options, ':');
		if (!endword)
			endword = options + strlen(options); /* GNU strchrnul */

		number = strtoul(options, &end, 0);
		if (end == endword) {
			dfuse_address = number;
		} else {
			fprintf(stderr, "Error: Invalid dfuse address: "
				"%s\n", options);
			exit(2);
		}
		options = endword;
	}

	while (*options) {
		if (*options == ':') {
			options++;
			continue;
		}
		endword = strchr(options, ':');
		if (!endword)
			endword = options + strlen(options);

		if (!strncmp(options, "force", endword - options)) {
			dfuse_force++;
			options += 5;
			continue;
		}
		if (!strncmp(options, "leave", endword - options)) {
			dfuse_leave = 1;
			options += 5;
			continue;
		}
		if (!strncmp(options, "unprotect", endword - options)) {
			dfuse_unprotect = 1;
			options += 9;
			continue;
		}
		if (!strncmp(options, "mass-erase", endword - options)) {
			dfuse_mass_erase = 1;
			options += 10;
			continue;
		}

		/* any valid number is interpreted as upload length */
		number = strtoul(options, &end, 0);
		if (end == endword) {
			dfuse_length = number;
		} else {
			fprintf(stderr, "Error: Invalid dfuse modifier: "
				"%s\n", options);
			exit(2);
		}
		options = endword;
	}
}

/* DFU_UPLOAD request for DfuSe 1.1a */
int dfuse_upload(struct dfu_if *dif, const unsigned short length,
		 unsigned char *data, unsigned short transaction)
{
	int status;

	status = libusb_control_transfer(dif->dev_handle,
		 /* bmRequestType */	 LIBUSB_ENDPOINT_IN |
					 LIBUSB_REQUEST_TYPE_CLASS |
					 LIBUSB_RECIPIENT_INTERFACE,
		 /* bRequest      */	 DFU_UPLOAD,
		 /* wValue        */	 transaction,
		 /* wIndex        */	 dif->interface,
		 /* Data          */	 data,
		 /* wLength       */	 length,
					 DFU_TIMEOUT);
	if (status < 0) {
		fprintf(stderr, "%s: libusb_control_msg returned %d\n",
			__FUNCTION__, status);
	}
	return status;
}

/* DFU_DNLOAD request for DfuSe 1.1a */
int dfuse_download(struct dfu_if *dif, const unsigned short length,
		   unsigned char *data, unsigned short transaction)
{
	int status;

	status = libusb_control_transfer(dif->dev_handle,
		 /* bmRequestType */	 LIBUSB_ENDPOINT_OUT |
					 LIBUSB_REQUEST_TYPE_CLASS |
					 LIBUSB_RECIPIENT_INTERFACE,
		 /* bRequest      */	 DFU_DNLOAD,
		 /* wValue        */	 transaction,
		 /* wIndex        */	 dif->interface,
		 /* Data          */	 data,
		 /* wLength       */	 length,
					 DFU_TIMEOUT);
	if (status < 0) {
		fprintf(stderr, "%s: libusb_control_transfer returned %d\n",
			__FUNCTION__, status);
	}
	return status;
}

/* DfuSe only commands */
int dfuse_special_command(struct dfu_if *dif, unsigned int address,
			  enum dfuse_command command)
{
	unsigned char buf[5];
	int length;
	int ret;
	struct dfu_status dst;

	if (command == ERASE_PAGE) {
		struct memsegment *segment;
		int page_size;

		segment = find_segment(mem_layout, address);
		if (!segment || !(segment->memtype & DFUSE_ERASABLE)) {
			fprintf(stderr,
				"Error: Page at 0x%08x can not be erased\n",
				address);
			exit(1);
		}
		page_size = segment->pagesize;
		if (verbose > 1)
			printf("Erasing page size %i at address 0x%08x, page "
			       "starting at 0x%08x\n", page_size, address,
			       address & ~(page_size - 1));
		buf[0] = 0x41;	/* Erase command */
		length = 5;
		last_erased = address;
	} else if (command == SET_ADDRESS) {
		if (verbose > 2)
			printf("  Setting address pointer to 0x%08x\n",
			       address);
		buf[0] = 0x21;	/* Set Address Pointer command */
		length = 5;
	} else if (command == MASS_ERASE) {
		buf[0] = 0x41;	/* Mass erase command when length = 1 */
		length = 1;
	} else if (command == READ_UNPROTECT) {
		buf[0] = 0x92;
		length = 1;
	} else {
		fprintf(stderr, "Error: Non-supported special command %d\n",
			command);
		exit(1);
	}
	buf[1] = address & 0xff;
	buf[2] = (address >> 8) & 0xff;
	buf[3] = (address >> 16) & 0xff;
	buf[4] = (address >> 24) & 0xff;

	ret = dfuse_download(dif, length, buf, 0);
	if (ret < 0) {
		fprintf(stderr, "Error during special command download\n");
		exit(1);
	}
	ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
	if (ret < 0) {
		fprintf(stderr, "Error during special command get_status\n");
		exit(1);
	}
	if (dst.bState != DFU_STATE_dfuDNBUSY) {
		fprintf(stderr, "Error: Wrong state after command download\n");
		exit(1);
	}
	/* wait while command is executed */
	if (verbose)
		printf("   Poll timeout %i ms\n", dst.bwPollTimeout);
	milli_sleep(dst.bwPollTimeout);

	if (command == READ_UNPROTECT)
		return ret;

	ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
	if (ret < 0) {
		fprintf(stderr, "Error during second get_status\n");
		printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
		       dfu_state_to_string(dst.bState), dst.bStatus,
		       dfu_status_to_string(dst.bStatus));
		exit(1);
	}
	if (dst.bStatus != DFU_STATUS_OK) {
		fprintf(stderr, "Error: Command not correctly executed\n");
		exit(1);
	}
	milli_sleep(dst.bwPollTimeout);

	ret = dfu_abort(dif->dev_handle, dif->interface);
	if (ret < 0) {
		fprintf(stderr, "Error sending dfu abort request\n");
		exit(1);
	}
	ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
	if (ret < 0) {
		fprintf(stderr, "Error during abort get_status\n");
		exit(1);
	}
	if (dst.bState != DFU_STATE_dfuIDLE) {
		fprintf(stderr, "Error: Failed to enter idle state on abort\n");
		exit(1);
	}
	milli_sleep(dst.bwPollTimeout);
	return ret;
}

int dfuse_do_upload(struct dfu_if *dif, int xfer_size, struct dfu_file file,
		    const char *dfuse_options)
{
	int total_bytes = 0;
	int upload_limit = 0;
	unsigned char *buf;
	int transaction;
	int ret;

	buf = malloc(xfer_size);
	if (!buf)
		return -ENOMEM;

	if (dfuse_options)
		dfuse_parse_options(dfuse_options);
	if (dfuse_length)
		upload_limit = dfuse_length;
	if (dfuse_address) {
		struct memsegment *segment;

		mem_layout = parse_memory_layout((char *)dif->alt_name);
		if (!mem_layout) {
			fprintf(stderr,
				"Error: Failed to parse memory layout\n");
			exit(1);
		}
		segment = find_segment(mem_layout, dfuse_address);
		if (!dfuse_force &&
		    (!segment || !(segment->memtype & DFUSE_READABLE))) {
			fprintf(stderr,
				"Error: Page at 0x%08x is not readable\n",
				dfuse_address);
			exit(1);
		}
		if (!upload_limit) {
			upload_limit = segment->end - dfuse_address + 1;
			printf("Limiting upload to end of memory segment, "
			       "%i bytes\n", upload_limit);
		}
		dfuse_special_command(dif, dfuse_address, SET_ADDRESS);
	} else {
		/* Boot loader decides the start address, unknown to us */
		/* Use a short length to lower risk of running out of bounds */
		if (!upload_limit)
			upload_limit = 0x4000;
		printf("Limiting default upload to %i bytes\n", upload_limit);
	}

	printf("bytes_per_hash=%u\n", xfer_size);
	printf("Starting upload: [");
	fflush(stdout);

	transaction = 2;
	while (1) {
		int rc, write_rc;

		/* last chunk can be smaller than original xfer_size */
		if (upload_limit - total_bytes < xfer_size)
			xfer_size = upload_limit - total_bytes;
		rc = dfuse_upload(dif, xfer_size, buf, transaction++);
		if (rc < 0) {
			ret = rc;
			goto out_free;
		}
		write_rc = fwrite(buf, 1, rc, file.filep);
		if (write_rc < rc) {
			fprintf(stderr, "Short file write: %s\n",
				strerror(errno));
			ret = -1;
			goto out_free;
		}
		total_bytes += rc;
		if (rc < xfer_size || total_bytes >= upload_limit) {
			/* last block, return successfully */
			ret = total_bytes;
			break;
		}
		putchar('#');
		fflush(stdout);
	}

	printf("] finished!\n");
	fflush(stdout);

 out_free:
	free(buf);

	return ret;
}

int dfuse_dnload_chunk(struct dfu_if *dif, unsigned char *data, int size,
		       int transaction)
{
	int bytes_sent;
	struct dfu_status dst;
	int ret;

	ret = dfuse_download(dif, size, size ? data : NULL, transaction);
	if (ret < 0) {
		fprintf(stderr, "Error during download\n");
		return ret;
	}
	bytes_sent = ret;

	do {
		ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
		if (ret < 0) {
			fprintf(stderr, "Error during download get_status\n");
			return ret;
		}
		milli_sleep(dst.bwPollTimeout);
	} while (dst.bState != DFU_STATE_dfuDNLOAD_IDLE &&
		 dst.bState != DFU_STATE_dfuERROR &&
		 dst.bState != DFU_STATE_dfuMANIFEST);

	if (dst.bState == DFU_STATE_dfuMANIFEST)
			printf("Transitioning to dfuMANIFEST state\n");

	if (dst.bStatus != DFU_STATUS_OK) {
		printf(" failed!\n");
		printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
		       dfu_state_to_string(dst.bState), dst.bStatus,
		       dfu_status_to_string(dst.bStatus));
		return -1;
	}
	return bytes_sent;
}

/* Writes an element of any size to the device, taking care of page erases */
/* returns 0 on success, otherwise -EINVAL */
int dfuse_dnload_element(struct dfu_if *dif, unsigned int dwElementAddress,
			 unsigned int dwElementSize, unsigned char *data,
			 int xfer_size)
{
	int p;
	int ret;
	struct memsegment *segment;

	/* Check at least that we can write to the last address */
	segment =
	    find_segment(mem_layout, dwElementAddress + dwElementSize - 1);
	if (!segment || !(segment->memtype & DFUSE_WRITEABLE)) {
		fprintf(stderr, "Error: Last page at 0x%08x is not writeable\n",
			dwElementAddress + dwElementSize - 1);
		exit(1);
	}

	for (p = 0; p < dwElementSize; p += xfer_size) {
		int page_size;
		unsigned int erase_address;
		unsigned int address = dwElementAddress + p;
		int chunk_size = xfer_size;

		segment = find_segment(mem_layout, address);
		if (!segment || !(segment->memtype & DFUSE_WRITEABLE)) {
			fprintf(stderr,
				"Error: Page at 0x%08x is not writeable\n",
				address);
			exit(1);
		}
		page_size = segment->pagesize;

		/* check if this is the last chunk */
		if (p + chunk_size > dwElementSize)
			chunk_size = dwElementSize - p;

		/* Erase only for flash memory downloads */
		if ((segment->memtype & DFUSE_ERASABLE) && !dfuse_mass_erase) {
			/* erase all involved pages */
			for (erase_address = address;
			     erase_address < address + chunk_size;
			     erase_address += page_size)
				if ((erase_address & ~(page_size - 1)) !=
				    (last_erased & ~(page_size - 1)))
					dfuse_special_command(dif,
							      erase_address,
							      ERASE_PAGE);

			if (((address + chunk_size - 1) & ~(page_size - 1)) !=
			    (last_erased & ~(page_size - 1))) {
				if (verbose > 2)
					printf(" Chunk extends into next page,"
					       " erase it as well\n");
				dfuse_special_command(dif,
						      address + chunk_size - 1,
						      ERASE_PAGE);
			}
		}

		if (verbose) {
			printf(" Download from image offset "
			       "%08x to memory %08x-%08x, size %i\n",
			       p, address, address + chunk_size - 1,
			       chunk_size);
		} else {
			printf(".");
			fflush(stdout);
		}
		
		dfuse_special_command(dif, address, SET_ADDRESS);

		/* transaction = 2 for no address offset */
		ret = dfuse_dnload_chunk(dif, data + p, chunk_size, 2);
		if (ret != chunk_size) {
			fprintf(stderr, "Failed to write whole chunk: "
				"%i of %i bytes\n", ret, chunk_size);
			return -EINVAL;
		}
	}
	if (!verbose)
		printf("\n"); /* terminate line of dots */
	return 0;
}

/* Download raw binary file to DfuSe device */
int dfuse_do_bin_dnload(struct dfu_if *dif, int xfer_size,
			struct dfu_file file, unsigned int start_address)
{
	unsigned int dwElementAddress;
	unsigned int dwElementSize;
	unsigned char *data;
	int read_bytes = 0;
	int ret;

	dwElementAddress = start_address;
	dwElementSize = file.size;
	printf("Downloading to address = 0x%08x, size = %i\n",
	       dwElementAddress, dwElementSize);

	data = malloc(dwElementSize);
	if (!data) {
		fprintf(stderr, "Could not allocate data buffer\n");
		return -ENOMEM;
	}
	ret = fread(data, 1, dwElementSize, file.filep);
	read_bytes += ret;
	if (ret < dwElementSize) {
		fprintf(stderr, "Could not read data\n");
		ret = -EINVAL;
		goto out_free;
	}

	ret = dfuse_dnload_element(dif, dwElementAddress, dwElementSize, data,
				   xfer_size);
	if (ret != 0)
		goto out_free;

	if (read_bytes != file.size) {
		fprintf(stderr, "Warning: Read %i bytes, file size %li\n",
			read_bytes, file.size);
	}
	printf("File downloaded successfully\n");
	ret = read_bytes;

 out_free:
	free(data);
	return ret;
}

/* Parse a DfuSe file and download contents to device */
int dfuse_do_dfuse_dnload(struct dfu_if *dif, int xfer_size,
			  struct dfu_file file)
{
	char dfuprefix[11];
	char targetprefix[274];
	char elementheader[8];
	int image;
	int element;
	int bTargets;
	int bAlternateSetting;
	int dwNbElements;
	unsigned int dwElementAddress;
	unsigned int dwElementSize;
	unsigned char *data;
	int read_bytes = 0;
	int ret;

	/* Must be larger than a minimal DfuSe header and suffix */
	if (file.size <= sizeof(dfuprefix) + file.suffixlen +
	    sizeof(targetprefix) + sizeof(elementheader)) {
		fprintf(stderr, "File too small for a DfuSe file\n");
		return -EINVAL;
	}

	ret = fread(dfuprefix, 1, sizeof(dfuprefix), file.filep);
	if (ret < (int)sizeof(dfuprefix)) {
		fprintf(stderr, "Could not read DfuSe header\n");
		return -EIO;
	}
	read_bytes = ret;
	if (strncmp(dfuprefix, "DfuSe", 5)) {
		fprintf(stderr, "No valid DfuSe signature\n");
		return -EINVAL;
	}
	if (dfuprefix[5] != 0x01) {
		fprintf(stderr, "DFU format revision %i not supported\n",
			dfuprefix[5]);
		return -EINVAL;
	}
	bTargets = dfuprefix[10];
	printf("file contains %i DFU images\n", bTargets);

	for (image = 1; image <= bTargets; image++) {
		printf("parsing DFU image %i\n", image);
		ret = fread(targetprefix, 1, sizeof(targetprefix), file.filep);
		read_bytes += ret;
		if (ret < sizeof(targetprefix)) {
			fprintf(stderr, "Could not read DFU header\n");
			return -EIO;
		}
		if (strncmp(targetprefix, "Target", 6)) {
			fprintf(stderr, "No valid target signature\n");
			return -EINVAL;
		}
		bAlternateSetting = targetprefix[6];
		dwNbElements = quad2uint((unsigned char *)targetprefix + 270);
		printf("image for alternate setting %i, ", bAlternateSetting);
		printf("(%i elements, ", dwNbElements);
		printf("total size = %i)\n",
		       quad2uint((unsigned char *)targetprefix + 266));
		if (bAlternateSetting != dif->altsetting)
			printf("Warning: Image does not match current alternate"
			       " setting.\n"
			       "Please rerun with the correct -a option setting"
			       " to download this image!\n");
		for (element = 1; element <= dwNbElements; element++) {
			printf("parsing element %i, ", element);
			ret = fread(elementheader, 1, sizeof(elementheader),
				    file.filep);
			read_bytes += ret;
			if (ret < sizeof(elementheader)) {
				fprintf(stderr,
					"Could not read element header\n");
				return -EINVAL;
			}
			dwElementAddress =
			    quad2uint((unsigned char *)elementheader);
			dwElementSize =
			    quad2uint((unsigned char *)elementheader + 4);
			printf("address = 0x%08x, ", dwElementAddress);
			printf("size = %i\n", dwElementSize);

			/* sanity check */
			if (read_bytes + dwElementSize + file.suffixlen >
			    file.size) {
				fprintf(stderr,
					"File too small for element size\n");
				return -EINVAL;
			}
			data = malloc(dwElementSize);
			if (!data) {
				fprintf(stderr,
					"Could not allocate data buffer\n");
				return -ENOMEM;
			}
			ret = fread(data, 1, dwElementSize, file.filep);
			read_bytes += ret;
			if (ret < dwElementSize) {
				fprintf(stderr, "Could not read data\n");
				free(data);
				return -EIO;
			}

			if (bAlternateSetting == dif->altsetting)
				ret =
				    dfuse_dnload_element(dif, dwElementAddress,
							 dwElementSize, data,
							 xfer_size);
			else
				ret = 0;
			free(data);
			if (ret != 0)
				return ret;
		}
	}

	/* Just for book-keeping, read through the whole file */
	data = malloc(file.suffixlen);
	if (!data) {
		fprintf(stderr, "Could not allocate data buffer for suffix\n");
		return -ENOMEM;
	}
	ret = fread(data, 1, file.suffixlen, file.filep);
	free(data);
	if (ret < file.suffixlen) {
		fprintf(stderr, "Could not read through suffix\n");
		return -EIO;
	}
	read_bytes += ret;

	if (read_bytes != file.size) {
		fprintf(stderr, "Warning: Read %i bytes, file size %li\n",
			read_bytes, file.size);
	}

	printf("done parsing DfuSe file\n");
	return read_bytes;
}

int dfuse_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file file,
		    const char *dfuse_options)
{
	int ret;

	if (dfuse_options)
		dfuse_parse_options(dfuse_options);
	mem_layout = parse_memory_layout((char *)dif->alt_name);
	if (!mem_layout) {
		fprintf(stderr, "Error: Failed to parse memory layout\n");
		exit(1);
	}
	if (dfuse_unprotect) {
		if (!dfuse_force) {
			fprintf(stderr, "Error: The read unprotect command "
				"will erase the flash memory\n"
				"and can only be used with force\n");
			exit(1);
		}
		dfuse_special_command(dif, 0, READ_UNPROTECT);
		printf("Device disconnects, erases flash and resets now\n");
		exit(0);
	}
	if (dfuse_mass_erase) {
		if (!dfuse_force) {
			fprintf(stderr, "Error: The mass erase command "
				"can only be used with force\n");
			exit(1);
		}
		printf("Performing mass erase, this can take a moment\n");
		dfuse_special_command(dif, 0, MASS_ERASE);
	}
	if (dfuse_address) {
		if (file.bcdDFU == 0x11a) {
			fprintf(stderr, "Error: This is a DfuSe file, not "
				"meant for raw download\n");
			return -EINVAL;
		}
		ret = dfuse_do_bin_dnload(dif, xfer_size, file, dfuse_address);
	} else {
		if (file.bcdDFU != 0x11a) {
			fprintf(stderr, "Error: Only DfuSe file version 1.1a "
				"is supported\n");
			fprintf(stderr, "(for raw binary download, use the "
				"--dfuse-address option)\n");
			return -EINVAL;
		}
		ret = dfuse_do_dfuse_dnload(dif, xfer_size, file);
	}
	free_segment_list(mem_layout);

	if (dfuse_leave) {
		int ret2;
		struct dfu_status dst;

		dfuse_dnload_chunk(dif, NULL, 0, 2); /* Zero-size */
		ret2 = dfu_get_status(dif->dev_handle, dif->interface, &dst);
		if (ret2 < 0)
			fprintf(stderr, "Error during download get_status\n");
		if (verbose)
			printf("bState = %i and bStatus = %i\n",
			       dst.bState, dst.bStatus);
	}
	return ret;
}
