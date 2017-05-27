/* This is supposed to be a "real" DFU implementation, just as specified in the
 * USB DFU 1.0 Spec.  Not overloaded like the Atmel one...
 *
 * The code was originally intended to interface with a USB device running the
 * "sam7dfu" firmware (see http://www.openpcd.org/) on an AT91SAM7 processor.
 *
 * (C) 2007-2008 by Harald Welte <laforge@gnumonks.org>
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

extern int verbose;

int dfuload_do_upload(struct dfu_if *dif, int xfer_size, struct dfu_file file)
{
	int total_bytes = 0;
	unsigned char *buf;
	int ret;

	buf = malloc(xfer_size);
	if (!buf)
		return -ENOMEM;

	printf("bytes_per_hash=%u\n", xfer_size);
	printf("Copying data from DFU device to PC\n");
	printf("Starting upload: [");
	fflush(stdout);

	while (1) {
		int rc, write_rc;
		rc = dfu_upload(dif->dev_handle, dif->interface, xfer_size, buf);
		if (rc < 0) {
			ret = rc;
			goto out_free;
		}
		write_rc = fwrite(buf, 1, rc, file.filep);
		if (write_rc < rc) {
			fprintf(stderr, "Short file write: %s\n",
				strerror(errno));
			ret = total_bytes;
			goto out_free;
		}
		total_bytes += rc;
		if (rc < xfer_size) {
			/* last block, return */
			ret = total_bytes;
			break;
		}
		putchar('#');
		fflush(stdout);
	}
	ret = 0;

	printf("] finished!\n");
	fflush(stdout);

out_free:
	free(buf);
	if (verbose)
		printf("Received a total of %i bytes\n", total_bytes);

	return ret;
}

#define PROGRESS_BAR_WIDTH 50

int dfuload_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file file)
{
	int bytes_sent = 0;
	unsigned int bytes_per_hash, hashes = 0;
	unsigned char *buf;
	struct dfu_status dst;
	int ret;

	buf = malloc(xfer_size);
	if (!buf)
		return -ENOMEM;

	bytes_per_hash = (file.size - file.suffixlen) / PROGRESS_BAR_WIDTH;
	if (bytes_per_hash == 0)
		bytes_per_hash = 1;
	printf("bytes_per_hash=%u\n", bytes_per_hash);

	printf("Copying data from PC to DFU device\n");
	printf("Starting download: [");
	fflush(stdout);
	while (bytes_sent < file.size - file.suffixlen) {
		int hashes_todo;
		int bytes_left;
		int chunk_size;

		bytes_left = file.size - file.suffixlen - bytes_sent;
		if (bytes_left < xfer_size)
			chunk_size = bytes_left;
		else
			chunk_size = xfer_size;
		ret = fread(buf, 1, chunk_size, file.filep);
		if (ret < 0) {
			perror(file.name);
			goto out_free;
		}
		ret = dfu_download(dif->dev_handle, dif->interface, ret, ret ? buf : NULL);
		if (ret < 0) {
			fprintf(stderr, "Error during download\n");
			goto out_free;
		}
		bytes_sent += ret;

		do {
			ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
			if (ret < 0) {
				fprintf(stderr, "Error during download get_status\n");
				goto out_free;
			}

			if (dst.bState == DFU_STATE_dfuDNLOAD_IDLE ||
					dst.bState == DFU_STATE_dfuERROR)
				break;

			/* Wait while device executes flashing */
			if (quirks & QUIRK_POLLTIMEOUT)
				milli_sleep(DEFAULT_POLLTIMEOUT);
			else
				milli_sleep(dst.bwPollTimeout);

		} while (1);
		if (dst.bStatus != DFU_STATUS_OK) {
			printf(" failed!\n");
			printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
				dfu_state_to_string(dst.bState), dst.bStatus,
				dfu_status_to_string(dst.bStatus));
			ret = -1;
			goto out_free;
		}

		hashes_todo = (bytes_sent / bytes_per_hash) - hashes;
		hashes += hashes_todo;
		while (hashes_todo--)
			putchar('#');
		fflush(stdout);
	}

	/* send one zero sized download request to signalize end */
	ret = dfu_download(dif->dev_handle, dif->interface, 0, NULL);
	if (ret < 0) {
		fprintf(stderr, "Error sending completion packet\n");
		goto out_free;
	}

	printf("] finished!\n");
	fflush(stdout);
	if (verbose)
		printf("Sent a total of %i bytes\n", bytes_sent);

get_status:
	/* Transition to MANIFEST_SYNC state */
	ret = dfu_get_status(dif->dev_handle, dif->interface, &dst);
	if (ret < 0) {
		fprintf(stderr, "unable to read DFU status\n");
		goto out_free;
	}
	printf("state(%u) = %s, status(%u) = %s\n", dst.bState,
		dfu_state_to_string(dst.bState), dst.bStatus,
		dfu_status_to_string(dst.bStatus));
	if (!(quirks & QUIRK_POLLTIMEOUT))
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

out_free:
	free(buf);

	return bytes_sent;
}

void dfuload_init()
{
    dfu_debug( debug );
    dfu_init( 5000 );
}

