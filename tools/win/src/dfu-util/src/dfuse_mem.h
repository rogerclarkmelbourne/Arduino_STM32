/* Helper functions for reading the memory map in a device
 * following the ST DfuSe 1.1a specification.
 *
 * (C) 2011 Tormod Volden <debian.tormod@gmail.com>
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

#ifndef DFUSE_MEM_H
#define DFUSE_MEM_H

#define DFUSE_READABLE  1
#define DFUSE_ERASABLE  2
#define DFUSE_WRITEABLE 4

struct memsegment {
	unsigned int start;
	unsigned int end;
	int pagesize;
	int memtype;
	struct memsegment *next;
};

int add_segment(struct memsegment **list, struct memsegment new_element);

struct memsegment *find_segment(struct memsegment *list, unsigned int address);

void free_segment_list(struct memsegment *list);

struct memsegment *parse_memory_layout(char *intf_desc_str);

#endif /* DFUSE_MEM_H */
