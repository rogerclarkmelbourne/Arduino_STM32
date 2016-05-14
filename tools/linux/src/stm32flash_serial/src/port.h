/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2014 Antonio Borneo <borneo.antonio@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef _H_PORT
#define _H_PORT

typedef enum {
	PORT_ERR_OK = 0,
	PORT_ERR_NODEV,		/* No such device */
	PORT_ERR_TIMEDOUT,	/* Operation timed out */
	PORT_ERR_UNKNOWN,
} port_err_t;

/* flags */
#define PORT_BYTE	(1 << 0)	/* byte (not frame) oriented */
#define PORT_GVR_ETX	(1 << 1)	/* cmd GVR returns protection status */
#define PORT_CMD_INIT	(1 << 2)	/* use INIT cmd to autodetect speed */
#define PORT_RETRY	(1 << 3)	/* allowed read() retry after timeout */
#define PORT_STRETCH_W	(1 << 4)	/* warning for no-stretching commands */

/* all options and flags used to open and configure an interface */
struct port_options {
	const char *device;
	serial_baud_t baudRate;
	const char *serial_mode;
	int bus_addr;
	int rx_frame_max;
	int tx_frame_max;
};

/*
 * Specify the length of reply for command GET
 * This is helpful for frame-oriented protocols, e.g. i2c, to avoid time
 * consuming try-fail-timeout-retry operation.
 * On byte-oriented protocols, i.e. UART, this information would be skipped
 * after read the first byte, so not needed.
 */
struct varlen_cmd {
	uint8_t version;
	uint8_t length;
};

struct port_interface {
	const char *name;
	unsigned flags;
	port_err_t (*open)(struct port_interface *port, struct port_options *ops);
	port_err_t (*close)(struct port_interface *port);
	port_err_t (*read)(struct port_interface *port, void *buf, size_t nbyte);
	port_err_t (*write)(struct port_interface *port, void *buf, size_t nbyte);
	port_err_t (*gpio)(struct port_interface *port, serial_gpio_t n, int level);
	const char *(*get_cfg_str)(struct port_interface *port);
	struct varlen_cmd *cmd_get_reply;
	void *private;
};

port_err_t port_open(struct port_options *ops, struct port_interface **outport);

#endif
