/*
 mempool.cpp - sleek implementation of a memory pool
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mempool.h"
#include <string.h>

#define POOLOFFSET 1

struct memblock MemoryPool::blocks[MEMPOOL_NUM_MEMBLOCKS+1];

void
MemoryPool::init()
{
  memset(&blocks[0], 0, sizeof(blocks));
  blocks[POOLSTART].begin = MEMPOOL_STARTADDRESS;
  blocks[POOLSTART].size = 0;
  blocks[POOLSTART].nextblock = NOBLOCK;
}

memhandle
MemoryPool::allocBlock(memaddress size)
{
  memblock* best = NULL;
  memhandle cur = POOLSTART;
  memblock* block = &blocks[POOLSTART];
  memaddress bestsize = MEMPOOL_SIZE + 1;

  do
    {
      memhandle next = block->nextblock;
      memaddress freesize = ( next == NOBLOCK ? blocks[POOLSTART].begin + MEMPOOL_SIZE : blocks[next].begin) - block->begin - block->size;
      if (freesize == size)
        {
          best = &blocks[cur];
          goto found;
        }
      if (freesize > size && freesize < bestsize)
        {
          bestsize = freesize;
          best = &blocks[cur];
        }
      if (next == NOBLOCK)
        {
          if (best)
            goto found;
          else
            goto collect;
        }
      block = &blocks[next];
      cur = next;
    }
  while (true);

  collect:
    {
      cur = POOLSTART;
      block = &blocks[POOLSTART];
      memhandle next;
      while ((next = block->nextblock) != NOBLOCK)
        {
          memaddress dest = block->begin + block->size;
          memblock* nextblock = &blocks[next];
          memaddress* src = &nextblock->begin;
          if (dest != *src)
            {
#ifdef MEMPOOL_MEMBLOCK_MV
              MEMPOOL_MEMBLOCK_MV(dest,*src,nextblock->size);
#endif
              *src = dest;
            }
          block = nextblock;
        }
      if (blocks[POOLSTART].begin + MEMPOOL_SIZE - block->begin - block->size >= size)
        best = block;
      else
        goto notfound;
    }

  found:
    {
      block = &blocks[POOLOFFSET];
      for (cur = POOLOFFSET; cur < MEMPOOL_NUM_MEMBLOCKS + POOLOFFSET; cur++)
        {
          if (block->size)
            {
              block++;
              continue;
            }
          memaddress address = best->begin + best->size;
#ifdef MEMBLOCK_ALLOC
          MEMBLOCK_ALLOC(address,size);
#endif
          block->begin = address;
          block->size = size;
          block->nextblock = best->nextblock;
          best->nextblock = cur;
          return cur;
        }
    }

  notfound: return NOBLOCK;
}

void
MemoryPool::freeBlock(memhandle handle)
{
  if (handle == NOBLOCK)
    return;
  memblock *b = &blocks[POOLSTART];

  do
    {
      memhandle next = b->nextblock;
      if (next == handle)
        {
          memblock *f = &blocks[next];
#ifdef MEMBLOCK_FREE
          MEMBLOCK_FREE(f->begin,f->size);
#endif
          b->nextblock = f->nextblock;
          f->size = 0;
          f->nextblock = NOBLOCK;
          return;
        }
      if (next == NOBLOCK)
        return;
      b = &blocks[next];
    }
  while (true);
}

void
MemoryPool::resizeBlock(memhandle handle, memaddress position)
{
  memblock * block = &blocks[handle];
  block->begin += position;
  block->size -= position;
}

void
MemoryPool::resizeBlock(memhandle handle, memaddress position, memaddress size)
{
  memblock * block = &blocks[handle];
  block->begin += position;
  block->size = size;
}

memaddress
MemoryPool::blockSize(memhandle handle)
{
  return blocks[handle].size;
}
