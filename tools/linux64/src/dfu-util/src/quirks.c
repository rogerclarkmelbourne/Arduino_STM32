/*
 *  Simple quirk system for dfu-util
 *
 *  Copyright 2010-2014 Tormod Volden
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

#include <stdint.h>
#include "quirks.h"

uint16_t get_quirks(uint16_t vendor, uint16_t product, uint16_t bcdDevice)
{
	uint16_t quirks = 0;

	/* Device returns bogus bwPollTimeout values */
	if ((vendor == VENDOR_OPENMOKO || vendor == VENDOR_FIC) &&
	    product >= PRODUCT_FREERUNNER_FIRST &&
	    product <= PRODUCT_FREERUNNER_LAST)
		quirks |= QUIRK_POLLTIMEOUT;

	if (vendor == VENDOR_VOTI &&
	    product == PRODUCT_OPENPCD)
		quirks |= QUIRK_POLLTIMEOUT;

	/* Reports wrong DFU version in DFU descriptor */
	if (vendor == VENDOR_LEAFLABS &&
	    product == PRODUCT_MAPLE3 &&
	    bcdDevice == 0x0200)
		quirks |= QUIRK_FORCE_DFU11;

	/* old devices(bcdDevice == 0) return bogus bwPollTimeout values */
	if (vendor == VENDOR_SIEMENS &&
	    (product == PRODUCT_PXM40 || product == PRODUCT_PXM50) &&
	    bcdDevice == 0)
		quirks |= QUIRK_POLLTIMEOUT;

	/* M-Audio Transit returns bogus bwPollTimeout values */
	if (vendor == VENDOR_MIDIMAN &&
	    product == PRODUCT_TRANSIT)
		quirks |= QUIRK_POLLTIMEOUT;

	return (quirks);
}
