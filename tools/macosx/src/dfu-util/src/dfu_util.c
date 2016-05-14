/*
 * Functions for detecting DFU USB entities
 *
 * Written by Harald Welte <laforge@openmoko.org>
 * Copyright 2007-2008 by OpenMoko, Inc.
 * Copyright 2013 Hans Petter Selasky <hps@bitfrost.no>
 *
 * Based on existing code of dfu-programmer-0.4
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
#include "dfu_util.h"
#include "dfuse.h"
#include "quirks.h"

#ifdef HAVE_USBPATH_H
#include <usbpath.h>
#endif

/*
 * Look for a descriptor in a concatenated descriptor list. Will
 * return upon the first match of the given descriptor type. Returns length of
 * found descriptor, limited to res_size
 */
static int find_descriptor(const uint8_t *desc_list, int list_len,
    uint8_t desc_type, void *res_buf, int res_size)
{
	int p = 0;

	if (list_len < 2)
		return (-1);

	while (p + 1 < list_len) {
		int desclen;

		desclen = (int) desc_list[p];
		if (desclen == 0) {
			warnx("Invalid descriptor list");
			return -1;
		}
		if (desc_list[p + 1] == desc_type) {
			if (desclen > res_size)
				desclen = res_size;
			if (p + desclen > list_len)
				desclen = list_len - p;
			memcpy(res_buf, &desc_list[p], desclen);
			return desclen;
		}
		p += (int) desc_list[p];
	}
	return -1;
}

static void probe_configuration(libusb_device *dev, struct libusb_device_descriptor *desc)
{
	struct usb_dfu_func_descriptor func_dfu;
	libusb_device_handle *devh;
	struct dfu_if *pdfu;
	struct libusb_config_descriptor *cfg;
	const struct libusb_interface_descriptor *intf;
	const struct libusb_interface *uif;
	char alt_name[MAX_DESC_STR_LEN + 1];
	char serial_name[MAX_DESC_STR_LEN + 1];
	int cfg_idx;
	int intf_idx;
	int alt_idx;
	int ret;
	int has_dfu;

	for (cfg_idx = 0; cfg_idx != desc->bNumConfigurations; cfg_idx++) {
		memset(&func_dfu, 0, sizeof(func_dfu));
		has_dfu = 0;

		ret = libusb_get_config_descriptor(dev, cfg_idx, &cfg);
		if (ret != 0)
			return;
		if (match_config_index > -1 && match_config_index != cfg->bConfigurationValue) {
			libusb_free_config_descriptor(cfg);
			continue;
		}

		/*
		 * In some cases, noticably FreeBSD if uid != 0,
		 * the configuration descriptors are empty
		 */
		if (!cfg)
			return;

		ret = find_descriptor(cfg->extra, cfg->extra_length,
		    USB_DT_DFU, &func_dfu, sizeof(func_dfu));
		if (ret > -1)
			goto found_dfu;

		for (intf_idx = 0; intf_idx < cfg->bNumInterfaces;
		     intf_idx++) {
			uif = &cfg->interface[intf_idx];
			if (!uif)
				break;

			for (alt_idx = 0; alt_idx < cfg->interface[intf_idx].num_altsetting;
			     alt_idx++) {
				intf = &uif->altsetting[alt_idx];

				ret = find_descriptor(intf->extra, intf->extra_length, USB_DT_DFU,
				      &func_dfu, sizeof(func_dfu));
				if (ret > -1)
					goto found_dfu;

				if (intf->bInterfaceClass != 0xfe ||
				    intf->bInterfaceSubClass != 1)
					continue;

				has_dfu = 1;
			}
		}
		if (has_dfu) {
			/*
			 * Finally try to retrieve it requesting the
			 * device directly This is not supported on
			 * all devices for non-standard types
			 */
			if (libusb_open(dev, &devh) == 0) {
				ret = libusb_get_descriptor(devh, USB_DT_DFU, 0,
				    (void *)&func_dfu, sizeof(func_dfu));
				libusb_close(devh);
				if (ret > -1)
					goto found_dfu;
			}
			warnx("Device has DFU interface, "
			    "but has no DFU functional descriptor");

			/* fake version 1.0 */
			func_dfu.bLength = 7;
			func_dfu.bcdDFUVersion = libusb_cpu_to_le16(0x0100);
			goto found_dfu;
		}
		libusb_free_config_descriptor(cfg);
		continue;

found_dfu:
		if (func_dfu.bLength == 7) {
			printf("Deducing device DFU version from functional descriptor "
			    "length\n");
			func_dfu.bcdDFUVersion = libusb_cpu_to_le16(0x0100);
		} else if (func_dfu.bLength < 9) {
			printf("Error obtaining DFU functional descriptor\n");
			printf("Please report this as a bug!\n");
			printf("Warning: Assuming DFU version 1.0\n");
			func_dfu.bcdDFUVersion = libusb_cpu_to_le16(0x0100);
			printf("Warning: Transfer size can not be detected\n");
			func_dfu.wTransferSize = 0;
		}

		for (intf_idx = 0; intf_idx < cfg->bNumInterfaces;
		     intf_idx++) {
			if (match_iface_index > -1 && match_iface_index != intf_idx)
				continue;

			uif = &cfg->interface[intf_idx];
			if (!uif)
				break;

			for (alt_idx = 0;
			     alt_idx < uif->num_altsetting; alt_idx++) {
				int dfu_mode;

				intf = &uif->altsetting[alt_idx];

				if (intf->bInterfaceClass != 0xfe ||
				    intf->bInterfaceSubClass != 1)
					continue;

				dfu_mode = (intf->bInterfaceProtocol == 2);
				/* e.g. DSO Nano has bInterfaceProtocol 0 instead of 2 */
				if (func_dfu.bcdDFUVersion == 0x011a && intf->bInterfaceProtocol == 0)
					dfu_mode = 1;

				if (dfu_mode &&
				    match_iface_alt_index > -1 && match_iface_alt_index != alt_idx)
					continue;

				if (dfu_mode) {
					if ((match_vendor_dfu >= 0 && match_vendor_dfu != desc->idVendor) ||
					    (match_product_dfu >= 0 && match_product_dfu != desc->idProduct)) {
						continue;
					}
				} else {
					if ((match_vendor >= 0 && match_vendor != desc->idVendor) ||
					    (match_product >= 0 && match_product != desc->idProduct)) {
						continue;
					}
				}

				if (libusb_open(dev, &devh)) {
					warnx("Cannot open DFU device %04x:%04x", desc->idVendor, desc->idProduct);
					break;
				}
				if (intf->iInterface != 0)
					ret = libusb_get_string_descriptor_ascii(devh,
					    intf->iInterface, (void *)alt_name, MAX_DESC_STR_LEN);
				else
					ret = -1;
				if (ret < 1)
					strcpy(alt_name, "UNKNOWN");
				if (desc->iSerialNumber != 0)
					ret = libusb_get_string_descriptor_ascii(devh,
					    desc->iSerialNumber, (void *)serial_name, MAX_DESC_STR_LEN);
				else
					ret = -1;
				if (ret < 1)
					strcpy(serial_name, "UNKNOWN");
				libusb_close(devh);

				if (dfu_mode &&
				    match_iface_alt_name != NULL && strcmp(alt_name, match_iface_alt_name))
					continue;

				if (dfu_mode) {
					if (match_serial_dfu != NULL && strcmp(match_serial_dfu, serial_name))
						continue;
				} else {
					if (match_serial != NULL && strcmp(match_serial, serial_name))
						continue;
				}

				pdfu = dfu_malloc(sizeof(*pdfu));

				memset(pdfu, 0, sizeof(*pdfu));

				pdfu->func_dfu = func_dfu;
				pdfu->dev = libusb_ref_device(dev);
				pdfu->quirks = get_quirks(desc->idVendor,
				    desc->idProduct, desc->bcdDevice);
				pdfu->vendor = desc->idVendor;
				pdfu->product = desc->idProduct;
				pdfu->bcdDevice = desc->bcdDevice;
				pdfu->configuration = cfg->bConfigurationValue;
				pdfu->interface = intf->bInterfaceNumber;
				pdfu->altsetting = intf->bAlternateSetting;
				pdfu->devnum = libusb_get_device_address(dev);
				pdfu->busnum = libusb_get_bus_number(dev);
				pdfu->alt_name = strdup(alt_name);
				if (pdfu->alt_name == NULL)
					errx(EX_SOFTWARE, "Out of memory");
				pdfu->serial_name = strdup(serial_name);
				if (pdfu->serial_name == NULL)
					errx(EX_SOFTWARE, "Out of memory");
				if (dfu_mode)
					pdfu->flags |= DFU_IFF_DFU;
				if (pdfu->quirks & QUIRK_FORCE_DFU11) {
					pdfu->func_dfu.bcdDFUVersion =
					  libusb_cpu_to_le16(0x0110);
				}
				pdfu->bMaxPacketSize0 = desc->bMaxPacketSize0;

				/* queue into list */
				pdfu->next = dfu_root;
				dfu_root = pdfu;
			}
		}
		libusb_free_config_descriptor(cfg);
	}
}

void probe_devices(libusb_context *ctx)
{
	libusb_device **list;
	ssize_t num_devs;
	ssize_t i;

	num_devs = libusb_get_device_list(ctx, &list);
	for (i = 0; i < num_devs; ++i) {
		struct libusb_device_descriptor desc;
		struct libusb_device *dev = list[i];

		if (match_bus > -1 && match_bus != libusb_get_bus_number(dev))
			continue;
		if (match_device > -1 && match_device != libusb_get_device_address(dev))
			continue;
		if (libusb_get_device_descriptor(dev, &desc))
			continue;
		probe_configuration(dev, &desc);
	}
	libusb_free_device_list(list, 0);
}

void disconnect_devices(void)
{
	struct dfu_if *pdfu;
	struct dfu_if *prev = NULL;

	for (pdfu = dfu_root; pdfu != NULL; pdfu = pdfu->next) {
		free(prev);
		libusb_unref_device(pdfu->dev);
		free(pdfu->alt_name);
		free(pdfu->serial_name);
		prev = pdfu;
	}
	free(prev);
	dfu_root = NULL;
}

void print_dfu_if(struct dfu_if *dfu_if)
{
	printf("Found %s: [%04x:%04x] ver=%04x, devnum=%u, cfg=%u, intf=%u, "
	       "alt=%u, name=\"%s\", serial=\"%s\"\n",
	       dfu_if->flags & DFU_IFF_DFU ? "DFU" : "Runtime",
	       dfu_if->vendor, dfu_if->product,
	       dfu_if->bcdDevice, dfu_if->devnum,
	       dfu_if->configuration, dfu_if->interface,
	       dfu_if->altsetting, dfu_if->alt_name,
	       dfu_if->serial_name);
}

/* Walk the device tree and print out DFU devices */
void list_dfu_interfaces(void)
{
	struct dfu_if *pdfu;

	for (pdfu = dfu_root; pdfu != NULL; pdfu = pdfu->next)
		print_dfu_if(pdfu);
}
