/*
 * DfuSe specific functions
 * 
 * This implements the ST Microsystems DFU extensions (DfuSe)
 * as per the DfuSe 1.1a specification (ST documents AN3156, AN2606)
 * The DfuSe file format is described in ST document UM0391.
 *
 * Copyright 2010-2014 Tormod Volden <debian.tormod@gmail.com>
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
static unsigned int last_erased_page = 1; /* non-aligned value, won't match */
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
			errx(EX_IOERR, "Invalid dfuse address: %s", options);
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
			errx(EX_IOERR, "Invalid dfuse modifier: %s", options);
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
		errx(EX_IOERR, "%s: libusb_control_msg returned %d",
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
		errx(EX_IOERR, "%s: libusb_control_transfer returned %d",
			__FUNCTION__, status);
	}
	return status;
}

/* DfuSe only commands */
/* Leaves the device in dfuDNLOAD-IDLE state */
int dfuse_special_command(struct dfu_if *dif, unsigned int address,
			  enum dfuse_command command)
{
	const char* dfuse_command_name[] = { "SET_ADDRESS" , "ERASE_PAGE",
					     "MASS_ERASE", "READ_UNPROTECT"};
	unsigned char buf[5];
	int length;
	int ret;
	struct dfu_status dst;
	int firstpoll = 1;

	if (command == ERASE_PAGE) {
		struct memsegment *segment;
		int page_size;

		segment = find_segment(mem_layout, address);
		if (!segment || !(segment->memtype & DFUSE_ERASABLE)) {
			errx(EX_IOERR, "Page at 0x%08x can not be erased",
				address);
		}
		page_size = segment->pagesize;
		if (verbose > 1)
			printf("Erasing page size %i at address 0x%08x, page "
			       "starting at 0x%08x\n", page_size, address,
			       address & ~(page_size - 1));
		buf[0] = 0x41;	/* Erase command */
		length = 5;
		last_erased_page = address & ~(page_size - 1);
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
		errx(EX_IOERR, "Non-supported special command %d", command);
	}
	buf[1] = address & 0xff;
	buf[2] = (address >> 8) & 0xff;
	buf[3] = (address >> 16) & 0xff;
	buf[4] = (address >> 24) & 0xff;

	ret = dfuse_download(dif, length, buf, 0);
	if (ret < 0) {
		errx(EX_IOERR, "Error during special command \"%s\" download",
			dfuse_command_name[command]);
	}
	do {
		ret = dfu_get_status(dif, &dst);
		if (ret < 0) {
			errx(EX_IOERR, "Error during special command \"%s\" get_status",
			     dfuse_command_name[command]);
		}
		if (firstpoll) {
			firstpoll = 0;
			if (dst.bState != DFU_STATE_dfuDNBUSY) {
				printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
				       dfu_state_to_string(dst.bState), dst.bStatus,
				       dfu_status_to_string(dst.bStatus));
				errx(EX_IOERR, "Wrong state after command \"%s\" download",
				     dfuse_command_name[command]);
			}
		}
		/* wait while command is executed */
		if (verbose)
			printf("   Poll timeout %i ms\n", dst.bwPollTimeout);
		milli_sleep(dst.bwPollTimeout);
		if (command == READ_UNPROTECT)
			return ret;
	} while (dst.bState == DFU_STATE_dfuDNBUSY);

	if (dst.bStatus != DFU_STATUS_OK) {
		errx(EX_IOERR, "%s not correctly executed",
			dfuse_command_name[command]);
	}
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
		errx(EX_IOERR, "Error during download");
		return ret;
	}
	bytes_sent = ret;

	do {
		ret = dfu_get_status(dif, &dst);
		if (ret < 0) {
			errx(EX_IOERR, "Error during download get_status");
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

int dfuse_do_upload(struct dfu_if *dif, int xfer_size, int fd,
		    const char *dfuse_options)
{
	int total_bytes = 0;
	int upload_limit = 0;
	unsigned char *buf;
	int transaction;
	int ret;

	buf = dfu_malloc(xfer_size);

	if (dfuse_options)
		dfuse_parse_options(dfuse_options);
	if (dfuse_length)
		upload_limit = dfuse_length;
	if (dfuse_address) {
		struct memsegment *segment;

		mem_layout = parse_memory_layout((char *)dif->alt_name);
		if (!mem_layout)
			errx(EX_IOERR, "Failed to parse memory layout");

		segment = find_segment(mem_layout, dfuse_address);
		if (!dfuse_force &&
		    (!segment || !(segment->memtype & DFUSE_READABLE)))
			errx(EX_IOERR, "Page at 0x%08x is not readable",
				dfuse_address);

		if (!upload_limit) {
			upload_limit = segment->end - dfuse_address + 1;
			printf("Limiting upload to end of memory segment, "
			       "%i bytes\n", upload_limit);
		}
		dfuse_special_command(dif, dfuse_address, SET_ADDRESS);
		dfu_abort_to_idle(dif);
	} else {
		/* Boot loader decides the start address, unknown to us */
		/* Use a short length to lower risk of running out of bounds */
		if (!upload_limit)
			upload_limit = 0x4000;
		printf("Limiting default upload to %i bytes\n", upload_limit);
	}

	dfu_progress_bar("Upload", 0, 1);

	transaction = 2;
	while (1) {
		int rc;

		/* last chunk can be smaller than original xfer_size */
		if (upload_limit - total_bytes < xfer_size)
			xfer_size = upload_limit - total_bytes;
		rc = dfuse_upload(dif, xfer_size, buf, transaction++);
		if (rc < 0) {
			ret = rc;
			goto out_free;
		}

		dfu_file_write_crc(fd, 0, buf, rc);
		total_bytes += rc;

		if (total_bytes < 0)
			errx(EX_SOFTWARE, "Received too many bytes");

		if (rc < xfer_size || total_bytes >= upload_limit) {
			/* last block, return successfully */
			ret = total_bytes;
			break;
		}
		dfu_progress_bar("Upload", total_bytes, upload_limit);
	}

	dfu_progress_bar("Upload", total_bytes, total_bytes);

	dfu_abort_to_idle(dif);
	if (dfuse_leave) {
		dfuse_special_command(dif, dfuse_address, SET_ADDRESS);
		dfuse_dnload_chunk(dif, NULL, 0, 2); /* Zero-size */
	}

 out_free:
	free(buf);

	return ret;
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
		errx(EX_IOERR, "Last page at 0x%08x is not writeable",
			dwElementAddress + dwElementSize - 1);
	}

	dfu_progress_bar("Download", 0, 1);

	for (p = 0; p < (int)dwElementSize; p += xfer_size) {
		int page_size;
		unsigned int erase_address;
		unsigned int address = dwElementAddress + p;
		int chunk_size = xfer_size;

		segment = find_segment(mem_layout, address);
		if (!segment || !(segment->memtype & DFUSE_WRITEABLE)) {
			errx(EX_IOERR, "Page at 0x%08x is not writeable",
				address);
		}
		page_size = segment->pagesize;

		/* check if this is the last chunk */
		if (p + chunk_size > (int)dwElementSize)
			chunk_size = dwElementSize - p;

		/* Erase only for flash memory downloads */
		if ((segment->memtype & DFUSE_ERASABLE) && !dfuse_mass_erase) {
			/* erase all involved pages */
			for (erase_address = address;
			     erase_address < address + chunk_size;
			     erase_address += page_size)
				if ((erase_address & ~(page_size - 1)) !=
				    last_erased_page)
					dfuse_special_command(dif,
							      erase_address,
							      ERASE_PAGE);

			if (((address + chunk_size - 1) & ~(page_size - 1)) !=
			    last_erased_page) {
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
			dfu_progress_bar("Download", p, dwElementSize);
		}
		
		dfuse_special_command(dif, address, SET_ADDRESS);

		/* transaction = 2 for no address offset */
		ret = dfuse_dnload_chunk(dif, data + p, chunk_size, 2);
		if (ret != chunk_size) {
			errx(EX_IOERR, "Failed to write whole chunk: "
				"%i of %i bytes", ret, chunk_size);
			return -EINVAL;
		}
	}
	if (!verbose)
		dfu_progress_bar("Download", dwElementSize, dwElementSize);
	return 0;
}

static void
dfuse_memcpy(unsigned char *dst, unsigned char **src, int *rem, int size)
{
	if (size > *rem) {
		errx(EX_IOERR, "Corrupt DfuSe file: "
		    "Cannot read %d bytes from %d bytes", size, *rem);
	}
	if (dst != NULL)
		memcpy(dst, *src, size);
	(*src) += size;
	(*rem) -= size;
}

/* Download raw binary file to DfuSe device */
int dfuse_do_bin_dnload(struct dfu_if *dif, int xfer_size,
			struct dfu_file *file, unsigned int start_address)
{
	unsigned int dwElementAddress;
	unsigned int dwElementSize;
	unsigned char *data;
	int ret;

	dwElementAddress = start_address;
	dwElementSize = file->size.total -
	    file->size.suffix - file->size.prefix;

	printf("Downloading to address = 0x%08x, size = %i\n",
	       dwElementAddress, dwElementSize);

	data = file->firmware + file->size.prefix;

	ret = dfuse_dnload_element(dif, dwElementAddress, dwElementSize, data,
				   xfer_size);
	if (ret != 0)
		goto out_free;

	printf("File downloaded successfully\n");
	ret = dwElementSize;

 out_free:
	return ret;
}

/* Parse a DfuSe file and download contents to device */
int dfuse_do_dfuse_dnload(struct dfu_if *dif, int xfer_size,
			  struct dfu_file *file)
{
	uint8_t dfuprefix[11];
	uint8_t targetprefix[274];
	uint8_t elementheader[8];
	int image;
	int element;
	int bTargets;
	int bAlternateSetting;
	int dwNbElements;
	unsigned int dwElementAddress;
	unsigned int dwElementSize;
	uint8_t *data;
	int ret;
	int rem;
	int bFirstAddressSaved = 0;

	rem = file->size.total - file->size.prefix - file->size.suffix;
	data = file->firmware + file->size.prefix;

        /* Must be larger than a minimal DfuSe header and suffix */
	if (rem < (int)(sizeof(dfuprefix) +
	    sizeof(targetprefix) + sizeof(elementheader))) {
		errx(EX_SOFTWARE, "File too small for a DfuSe file");
        }

	dfuse_memcpy(dfuprefix, &data, &rem, sizeof(dfuprefix));

	if (strncmp((char *)dfuprefix, "DfuSe", 5)) {
		errx(EX_IOERR, "No valid DfuSe signature");
		return -EINVAL;
	}
	if (dfuprefix[5] != 0x01) {
		errx(EX_IOERR, "DFU format revision %i not supported",
			dfuprefix[5]);
		return -EINVAL;
	}
	bTargets = dfuprefix[10];
	printf("file contains %i DFU images\n", bTargets);

	for (image = 1; image <= bTargets; image++) {
		printf("parsing DFU image %i\n", image);
		dfuse_memcpy(targetprefix, &data, &rem, sizeof(targetprefix));
		if (strncmp((char *)targetprefix, "Target", 6)) {
			errx(EX_IOERR, "No valid target signature");
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
			dfuse_memcpy(elementheader, &data, &rem, sizeof(elementheader));
			dwElementAddress =
			    quad2uint((unsigned char *)elementheader);
			dwElementSize =
			    quad2uint((unsigned char *)elementheader + 4);
			printf("address = 0x%08x, ", dwElementAddress);
			printf("size = %i\n", dwElementSize);

			if (!bFirstAddressSaved) {
				bFirstAddressSaved = 1;
				dfuse_address = dwElementAddress;
			}
			/* sanity check */
			if ((int)dwElementSize > rem)
				errx(EX_SOFTWARE, "File too small for element size");

			if (bAlternateSetting == dif->altsetting) {
				ret = dfuse_dnload_element(dif, dwElementAddress,
				    dwElementSize, data, xfer_size);
			} else {
				ret = 0;
			}

			/* advance read pointer */
			dfuse_memcpy(NULL, &data, &rem, dwElementSize);

			if (ret != 0)
				return ret;
		}
	}

	if (rem != 0)
		warnx("%d bytes leftover", rem);

	printf("done parsing DfuSe file\n");

	return 0;
}

int dfuse_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file *file,
		    const char *dfuse_options)
{
	int ret;

	if (dfuse_options)
		dfuse_parse_options(dfuse_options);
	mem_layout = parse_memory_layout((char *)dif->alt_name);
	if (!mem_layout) {
		errx(EX_IOERR, "Failed to parse memory layout");
	}
	if (dfuse_unprotect) {
		if (!dfuse_force) {
			errx(EX_IOERR, "The read unprotect command "
				"will erase the flash memory"
				"and can only be used with force\n");
		}
		dfuse_special_command(dif, 0, READ_UNPROTECT);
		printf("Device disconnects, erases flash and resets now\n");
		exit(0);
	}
	if (dfuse_mass_erase) {
		if (!dfuse_force) {
			errx(EX_IOERR, "The mass erase command "
				"can only be used with force");
		}
		printf("Performing mass erase, this can take a moment\n");
		dfuse_special_command(dif, 0, MASS_ERASE);
	}
	if (dfuse_address) {
		if (file->bcdDFU == 0x11a) {
			errx(EX_IOERR, "This is a DfuSe file, not "
				"meant for raw download");
		}
		ret = dfuse_do_bin_dnload(dif, xfer_size, file, dfuse_address);
	} else {
		if (file->bcdDFU != 0x11a) {
			warnx("Only DfuSe file version 1.1a is supported");
			errx(EX_IOERR, "(for raw binary download, use the "
			     "--dfuse-address option)");
		}
		ret = dfuse_do_dfuse_dnload(dif, xfer_size, file);
	}
	free_segment_list(mem_layout);

	dfu_abort_to_idle(dif);

	if (dfuse_leave) {
		dfuse_special_command(dif, dfuse_address, SET_ADDRESS);
		dfuse_dnload_chunk(dif, NULL, 0, 2); /* Zero-size */
	}
	return ret;
}
