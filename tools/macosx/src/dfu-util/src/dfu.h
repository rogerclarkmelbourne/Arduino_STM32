/*
 * dfu-programmer
 *
 * $Id: dfu.h,v 1.2 2005/09/25 01:27:42 schmidtw Exp $
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

#ifndef DFU_H
#define DFU_H

#include <libusb.h>
#include "usb_dfu.h"

/* DFU states */
#define STATE_APP_IDLE                  0x00
#define STATE_APP_DETACH                0x01
#define STATE_DFU_IDLE                  0x02
#define STATE_DFU_DOWNLOAD_SYNC         0x03
#define STATE_DFU_DOWNLOAD_BUSY         0x04
#define STATE_DFU_DOWNLOAD_IDLE         0x05
#define STATE_DFU_MANIFEST_SYNC         0x06
#define STATE_DFU_MANIFEST              0x07
#define STATE_DFU_MANIFEST_WAIT_RESET   0x08
#define STATE_DFU_UPLOAD_IDLE           0x09
#define STATE_DFU_ERROR                 0x0a


/* DFU status */
#define DFU_STATUS_OK                   0x00
#define DFU_STATUS_ERROR_TARGET         0x01
#define DFU_STATUS_ERROR_FILE           0x02
#define DFU_STATUS_ERROR_WRITE          0x03
#define DFU_STATUS_ERROR_ERASE          0x04
#define DFU_STATUS_ERROR_CHECK_ERASED   0x05
#define DFU_STATUS_ERROR_PROG           0x06
#define DFU_STATUS_ERROR_VERIFY         0x07
#define DFU_STATUS_ERROR_ADDRESS        0x08
#define DFU_STATUS_ERROR_NOTDONE        0x09
#define DFU_STATUS_ERROR_FIRMWARE       0x0a
#define DFU_STATUS_ERROR_VENDOR         0x0b
#define DFU_STATUS_ERROR_USBR           0x0c
#define DFU_STATUS_ERROR_POR            0x0d
#define DFU_STATUS_ERROR_UNKNOWN        0x0e
#define DFU_STATUS_ERROR_STALLEDPKT     0x0f

/* DFU commands */
#define DFU_DETACH      0
#define DFU_DNLOAD      1
#define DFU_UPLOAD      2
#define DFU_GETSTATUS   3
#define DFU_CLRSTATUS   4
#define DFU_GETSTATE    5
#define DFU_ABORT       6

/* DFU interface */
#define DFU_IFF_DFU             0x0001  /* DFU Mode, (not Runtime) */

/* This is based off of DFU_GETSTATUS
 *
 *  1 unsigned byte bStatus
 *  3 unsigned byte bwPollTimeout
 *  1 unsigned byte bState
 *  1 unsigned byte iString
*/

struct dfu_status {
    unsigned char bStatus;
    unsigned int  bwPollTimeout;
    unsigned char bState;
    unsigned char iString;
};

struct dfu_if {
    struct usb_dfu_func_descriptor func_dfu;
    uint16_t quirks;
    uint16_t busnum;
    uint16_t devnum;
    uint16_t vendor;
    uint16_t product;
    uint16_t bcdDevice;
    uint8_t configuration;
    uint8_t interface;
    uint8_t altsetting;
    uint8_t flags;
    uint8_t bMaxPacketSize0;
    char *alt_name;
    char *serial_name;
    libusb_device *dev;
    libusb_device_handle *dev_handle;
    struct dfu_if *next;
};

int dfu_detach( libusb_device_handle *device,
                const unsigned short interface,
                const unsigned short timeout );
int dfu_download( libusb_device_handle *device,
                  const unsigned short interface,
                  const unsigned short length,
                  const unsigned short transaction,
                  unsigned char* data );
int dfu_upload( libusb_device_handle *device,
                const unsigned short interface,
                const unsigned short length,
                const unsigned short transaction,
                unsigned char* data );
int dfu_get_status( struct dfu_if *dif,
                    struct dfu_status *status );
int dfu_clear_status( libusb_device_handle *device,
                      const unsigned short interface );
int dfu_get_state( libusb_device_handle *device,
                   const unsigned short interface );
int dfu_abort( libusb_device_handle *device,
               const unsigned short interface );
int dfu_abort_to_idle( struct dfu_if *dif);

const char *dfu_state_to_string( int state );

const char *dfu_status_to_string( int status );

#endif /* DFU_H */
