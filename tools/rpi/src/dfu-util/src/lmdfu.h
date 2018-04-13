
/* This implements the TI Stellaris DFU
 * as per the Application Update Using the USB Device Firmware Upgrade Class
 * (Document AN012373)
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

#ifndef LMDFU_H
#define LMDFU_H

int lmdfu_add_prefix(struct dfu_file file, unsigned int address);
int lmdfu_remove_prefix(struct dfu_file *file);
int lmdfu_check_prefix(struct dfu_file *file);

#endif /* LMDFU_H */
