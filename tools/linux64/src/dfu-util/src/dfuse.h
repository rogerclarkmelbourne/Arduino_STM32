/* This implements the ST Microsystems DFU extensions (DfuSe)
 * as per the DfuSe 1.1a specification (Document UM0391)
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

#ifndef DFUSE_H
#define DFUSE_H

#include "dfu.h"

enum dfuse_command { SET_ADDRESS, ERASE_PAGE, MASS_ERASE, READ_UNPROTECT };

int dfuse_special_command(struct dfu_if *dif, unsigned int address,
			  enum dfuse_command command);
int dfuse_do_upload(struct dfu_if *dif, int xfer_size, int fd,
		    const char *dfuse_options);
int dfuse_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file *file,
		    const char *dfuse_options);

#endif /* DFUSE_H */
