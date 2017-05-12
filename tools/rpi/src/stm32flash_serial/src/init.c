/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2010 Geoffrey McRae <geoff@spacevs.com>
  Copyright (C) 2013 Antonio Borneo <borneo.antonio@gmail.com>

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


#include <ctype.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "init.h"
#include "serial.h"
#include "stm32.h"
#include "port.h"

struct gpio_list {
	struct gpio_list *next;
	int gpio;
};


static int write_to(const char *filename, const char *value)
{
	int fd, ret;

	fd = open(filename, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Cannot open file \"%s\"\n", filename);
		return 0;
	}
	ret = write(fd, value, strlen(value));
	if (ret < 0) {
		fprintf(stderr, "Error writing in file \"%s\"\n", filename);
		close(fd);
		return 0;
	}
	close(fd);
	return 1;
}

#if !defined(__linux__)
static int drive_gpio(int n, int level, struct gpio_list **gpio_to_release)
{
	fprintf(stderr, "GPIO control only available in Linux\n");
	return 0;
}
#else
static int drive_gpio(int n, int level, struct gpio_list **gpio_to_release)
{
	char num[16]; /* sized to carry MAX_INT */
	char file[48]; /* sized to carry longest filename */
	struct stat buf;
	struct gpio_list *new;
	int ret;

	sprintf(file, "/sys/class/gpio/gpio%d/direction", n);
	ret = stat(file, &buf);
	if (ret) {
		/* file miss, GPIO not exported yet */
		sprintf(num, "%d", n);
		ret = write_to("/sys/class/gpio/export", num);
		if (!ret)
			return 0;
		ret = stat(file, &buf);
		if (ret) {
			fprintf(stderr, "GPIO %d not available\n", n);
			return 0;
		}
		new = (struct gpio_list *)malloc(sizeof(struct gpio_list));
		if (new == NULL) {
			fprintf(stderr, "Out of memory\n");
			return 0;
		}
		new->gpio = n;
		new->next = *gpio_to_release;
		*gpio_to_release = new;
	}

	return write_to(file, level ? "high" : "low");
}
#endif

static int release_gpio(int n)
{
	char num[16]; /* sized to carry MAX_INT */

	sprintf(num, "%d", n);
	return write_to("/sys/class/gpio/unexport", num);
}

static int gpio_sequence(struct port_interface *port, const char *s, size_t l)
{
	struct gpio_list *gpio_to_release = NULL, *to_free;
	int ret, level, gpio;

	ret = 1;
	while (ret == 1 && *s && l > 0) {
		if (*s == '-') {
			level = 0;
			s++;
			l--;
		} else
			level = 1;

		if (isdigit(*s)) {
			gpio = atoi(s);
			while (isdigit(*s)) {
				s++;
				l--;
			}
		} else if (!strncmp(s, "rts", 3)) {
			gpio = -GPIO_RTS;
			s += 3;
			l -= 3;
		} else if (!strncmp(s, "dtr", 3)) {
			gpio = -GPIO_DTR;
			s += 3;
			l -= 3;
		} else if (!strncmp(s, "brk", 3)) {
			gpio = -GPIO_BRK;
			s += 3;
			l -= 3;
		} else {
			fprintf(stderr, "Character \'%c\' is not a digit\n", *s);
			ret = 0;
			break;
		}

		if (*s && (l > 0)) {
			if (*s == ',') {
				s++;
				l--;
			} else {
				fprintf(stderr, "Character \'%c\' is not a separator\n", *s);
				ret = 0;
				break;
			}
		}
		if (gpio < 0)
			ret = (port->gpio(port, -gpio, level) == PORT_ERR_OK);
		else
			ret = drive_gpio(gpio, level, &gpio_to_release);
		usleep(100000);
	}

	while (gpio_to_release) {
		release_gpio(gpio_to_release->gpio);
		to_free = gpio_to_release;
		gpio_to_release = gpio_to_release->next;
		free(to_free);
	}
	usleep(500000);
	return ret;
}

static int gpio_bl_entry(struct port_interface *port, const char *seq)
{
	char *s;

	if (seq == NULL || seq[0] == ':')
		return 1;

	s = strchr(seq, ':');
	if (s == NULL)
		return gpio_sequence(port, seq, strlen(seq));

	return gpio_sequence(port, seq, s - seq);
}

static int gpio_bl_exit(struct port_interface *port, const char *seq)
{
	char *s;

	if (seq == NULL)
		return 1;

	s = strchr(seq, ':');
	if (s == NULL || s[1] == '\0')
		return 1;

	return gpio_sequence(port, s + 1, strlen(s + 1));
}

int init_bl_entry(struct port_interface *port, const char *seq)
{
	if (seq)
		return gpio_bl_entry(port, seq);

	return 1;
}

int init_bl_exit(stm32_t *stm, struct port_interface *port, const char *seq)
{
	if (seq)
		return gpio_bl_exit(port, seq);

	if (stm32_reset_device(stm) != STM32_ERR_OK)
		return 0;
	return 1;
}
