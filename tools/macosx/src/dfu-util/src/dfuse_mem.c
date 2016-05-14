/*
 * Helper functions for reading the memory map of a device
 * following the ST DfuSe 1.1a specification.
 *
 * Copyright 2011-2014 Tormod Volden <debian.tormod@gmail.com>
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
#include <string.h>
#include <errno.h>

#include "portable.h"
#include "dfu_file.h"
#include "dfuse_mem.h"

int add_segment(struct memsegment **segment_list, struct memsegment segment)
{
	struct memsegment *new_element;

	new_element = dfu_malloc(sizeof(struct memsegment));
	*new_element = segment;
	new_element->next = NULL;

	if (*segment_list == NULL)
		/* list can be empty on first call */
		*segment_list = new_element;
	else {
		struct memsegment *next_element;

		/* find last element in list */
		next_element = *segment_list;
		while (next_element->next != NULL)
			next_element = next_element->next;
		next_element->next = new_element;
	}
	return 0;
}

struct memsegment *find_segment(struct memsegment *segment_list,
				unsigned int address)
{
	while (segment_list != NULL) {
		if (segment_list->start <= address &&
		    segment_list->end >= address)
			return segment_list;
		segment_list = segment_list->next;
	}
	return NULL;
}

void free_segment_list(struct memsegment *segment_list)
{
	struct memsegment *next_element;

	while (segment_list->next != NULL) {
		next_element = segment_list->next;
		free(segment_list);
		segment_list = next_element;
	}
	free(segment_list);
}

/* Parse memory map from interface descriptor string
 * encoded as per ST document UM0424 section 4.3.2.
 */
struct memsegment *parse_memory_layout(char *intf_desc)
{

	char multiplier, memtype;
	unsigned int address;
	int sectors, size;
	char *name, *typestring;
	int ret;
	int count = 0;
	char separator;
	int scanned;
	struct memsegment *segment_list = NULL;
	struct memsegment segment;

	name = dfu_malloc(strlen(intf_desc));

	ret = sscanf(intf_desc, "@%[^/]%n", name, &scanned);
	if (ret < 1) {
		free(name);
		warnx("Could not read name, sscanf returned %d", ret);
		return NULL;
	}
	printf("DfuSe interface name: \"%s\"\n", name);

	intf_desc += scanned;
	typestring = dfu_malloc(strlen(intf_desc));

	while (ret = sscanf(intf_desc, "/0x%x/%n", &address, &scanned),
	       ret > 0) {

		intf_desc += scanned;
		while (ret = sscanf(intf_desc, "%d*%d%c%[^,/]%n",
				    &sectors, &size, &multiplier, typestring,
				    &scanned), ret > 2) {
			intf_desc += scanned;

			count++;
			memtype = 0;
			if (ret == 4) {
				if (strlen(typestring) == 1
				    && typestring[0] != '/')
					memtype = typestring[0];
				else {
					warnx("Parsing type identifier '%s' "
						"failed for segment %i",
						typestring, count);
					continue;
				}
			}

			/* Quirk for STM32F4 devices */
			if (strcmp(name, "Device Feature") == 0)
				memtype = 'e';

			switch (multiplier) {
			case 'B':
				break;
			case 'K':
				size *= 1024;
				break;
			case 'M':
				size *= 1024 * 1024;
				break;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
				if (!memtype) {
					warnx("Non-valid multiplier '%c', "
						"interpreted as type "
						"identifier instead",
						multiplier);
					memtype = multiplier;
					break;
				}
				/* fallthrough if memtype was already set */
			default:
				warnx("Non-valid multiplier '%c', "
					"assuming bytes", multiplier);
			}

			if (!memtype) {
				warnx("No valid type for segment %d\n", count);
				continue;
			}

			segment.start = address;
			segment.end = address + sectors * size - 1;
			segment.pagesize = size;
			segment.memtype = memtype & 7;
			add_segment(&segment_list, segment);

			if (verbose)
				printf("Memory segment at 0x%08x %3d x %4d = "
				       "%5d (%s%s%s)\n",
				       address, sectors, size, sectors * size,
				       memtype & DFUSE_READABLE  ? "r" : "",
				       memtype & DFUSE_ERASABLE  ? "e" : "",
				       memtype & DFUSE_WRITEABLE ? "w" : "");

			address += sectors * size;

			separator = *intf_desc;
			if (separator == ',')
				intf_desc += 1;
			else
				break;
		}	/* while per segment */

	}		/* while per address */
	free(name);
	free(typestring);

	return segment_list;
}
