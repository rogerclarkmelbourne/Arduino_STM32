/*
 * DFU transfer routines
 *
 * This is supposed to be a general DFU implementation, as specified in the
 * USB DFU 1.0 and 1.1 specification.
 *
 * The code was originally intended to interface with a USB device running the
 * "sam7dfu" firmware (see http://www.openpcd.org/) on an AT91SAM7 processor.
 *
 * Copyright 2007-2008 Harald Welte <laforge@gnumonks.org>
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <libusb.h>

#include "portable.h"
#include "dfu.h"
#include "usb_dfu.h"
#include "dfu_file.h"
#include "dfu_load.h"
#include "quirks.h"

int dfuload_do_upload(struct dfu_if *dif, int xfer_size,
    int expected_size, int fd)
{
	int total_bytes = 0;
	unsigned short transaction = 0;
	unsigned char *buf;
	int ret;

	buf = dfu_malloc(xfer_size);

	printf("Copying data from DFU device to PC\n");
	dfu_progress_bar("Upload", 0, 1);

	while (1) {
		int rc;
		rc = dfu_upload(dif->dev_handle, dif->interface,
		    xfer_size, transaction++, buf);
		if (rc < 0) {
			warnx("Error during upload");
			ret = rc;
			goto out_free;
		}

		dfu_file_write_crc(fd, 0, buf, rc);
		total_bytes += rc;

		if (total_bytes < 0)
			errx(EX_SOFTWARE, "Received too many bytes (wraparound)");

		if (rc < xfer_size) {
			/* last block, return */
			ret = total_bytes;
			break;
		}
		dfu_progress_bar("Upload", total_bytes, expected_size);
	}
	ret = 0;

out_free:
	dfu_progress_bar("Upload", total_bytes, total_bytes);
	if (total_bytes == 0)
		printf("\nFailed.\n");
	free(buf);
	if (verbose)
		printf("Received a total of %i bytes\n", total_bytes);
	if (expected_size != 0 && total_bytes != expected_size)
		errx(EX_SOFTWARE, "Unexpected number of bytes uploaded from device");
	return ret;
}

int dfuload_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file *file)
{
	int bytes_sent;
	int expected_size;
	unsigned char *buf;
	unsigned short transaction = 0;
	struct dfu_status dst;
	int ret;

	printf("Copying data from PC to DFU device\n");

	buf = file->firmware;
	expected_size = file->size.total - file->size.suffix;
	bytes_sent = 0;

	dfu_progress_bar("Download", 0, 1);
	while (bytes_sent < expected_size) {
		int bytes_left;
		int chunk_size;

		bytes_left = expected_size - bytes_sent;
		if (bytes_left < xfer_size)
			chunk_size = bytes_left;
		else
			chunk_size = xfer_size;

		ret = dfu_download(dif->dev_handle, dif->interface,
		    chunk_size, transaction++, chunk_size ? buf : NULL);
		if (ret < 0) {
			warnx("Error during download");
			goto out;
		}
		bytes_sent += chunk_size;
		buf += chunk_size;

		do {
			ret = dfu_get_status(dif, &dst);
			if (ret < 0) {
				errx(EX_IOERR, "Error during download get_status");
				goto out;
			}

			if (dst.bState == DFU_STATE_dfuDNLOAD_IDLE ||
					dst.bState == DFU_STATE_dfuERROR)
				break;

			/* Wait while device executes flashing */
			milli_sleep(dst.bwPollTimeout);

		} while (1);
		if (dst.bStatus != DFU_STATUS_OK) {
			printf(" failed!\n");
			printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
				dfu_state_to_string(dst.bState), dst.bStatus,
				dfu_status_to_string(dst.bStatus));
			ret = -1;
			goto out;
		}
		dfu_progress_bar("Download", bytes_sent, bytes_sent + bytes_left);
	}

	/* send one zero sized download request to signalize end */
	ret = dfu_download(dif->dev_handle, dif->interface,
	    0, transaction, NULL);
	if (ret < 0) {
		errx(EX_IOERR, "Error sending completion packet");
		goto out;
	}

	dfu_progress_bar("Download", bytes_sent, bytes_sent);

	if (verbose)
		printf("Sent a total of %i bytes\n", bytes_sent);

get_status:
	/* Transition to MANIFEST_SYNC state */
	ret = dfu_get_status(dif, &dst);
	if (ret < 0) {
		warnx("unable to read DFU status after completion");
		goto out;
	}
	printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
		dfu_state_to_string(dst.bState), dst.bStatus,
		dfu_status_to_string(dst.bStatus));

	milli_sleep(dst.bwPollTimeout);

	/* FIXME: deal correctly with ManifestationTolerant=0 / WillDetach bits */
	switch (dst.bState) {
	case DFU_STATE_dfuMANIFEST_SYNC:
	case DFU_STATE_dfuMANIFEST:
		/* some devices (e.g. TAS1020b) need some time before we
		 * can obtain the status */
		milli_sleep(1000);
		goto get_status;
		break;
	case DFU_STATE_dfuIDLE:
		break;
	}
	printf("Done!\n");

out:
	return bytes_sent;
}
