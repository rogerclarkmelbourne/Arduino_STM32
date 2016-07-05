/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file libmaple/include/libmaple/ring_buffer.h
 * @brief Simple circular buffer
 *
 * This implementation is not thread-safe.  In particular, none of
 * these functions is guaranteed re-entrant.
 */

#ifndef _LIBMAPLE_RING_BUFFER_H_
#define _LIBMAPLE_RING_BUFFER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/**
 * Ring buffer type.
 *
 * The buffer is empty when used == 0.
 *
 * The buffer is full when used == size.
 *
 * This implementation allows using the full buffer size instead of the buffer size - 1. */
typedef struct ring_buffer {
    volatile uint8 *buf; /**< Buffer items are stored into */
    uint16 head;         /**< Index of the next item to remove */
    uint16 used;         /**< Amount of used buffer */
    uint16 size;         /**< Buffer size */
} ring_buffer;

/**
 * Initialise a ring buffer.
 *
 *  @param rb   Instance to initialise
 *
 *  @param size Number of items in buf
 *
 *  @param buf  Buffer to store items into
 */
inline void rb_init(ring_buffer *rb, uint16 size);
/*
static inline void rb_init(ring_buffer *rb, uint16 size, uint8 *buf) {
    rb->head = 0;
    rb->used = 0;
    rb->size = size;
    rb->buf = buf;
}
*/
/**
 * @brief Return the number of elements stored in the ring buffer.
 * @param rb Buffer whose elements to count.
 */
static inline uint16 rb_full_count(ring_buffer *rb) {
    return rb->used;
}

/**
 * @brief Returns true if and only if the ring buffer is full.
 * @param rb Buffer to test.
 */
static inline int rb_is_full(ring_buffer *rb) {
    return rb->used == rb->size;
}

/**
 * @brief Returns true if and only if the ring buffer is empty.
 * @param rb Buffer to test.
 */
static inline int rb_is_empty(ring_buffer *rb) {
    return rb->used == 0;
}

/**
 * Append element onto the end of a ring buffer.
 * @param rb Buffer to append onto.
 * @param element Value to append.
 */
static inline void rb_insert(ring_buffer *rb, uint8 element) {
    if(rb->used < rb->size)
    {
        rb->buf[(rb->head + rb->used) % rb->size] = element;
        rb->used += 1;
    }
}

/**
 * @brief Remove and return the first item from a ring buffer.
 * @param rb Buffer to remove from, must contain at least one element.
 */
static inline int16 rb_remove(ring_buffer *rb) {
    int16 ch = -1;
    if(rb->used > 0)
    {
        ch = rb->buf[rb->head];
        rb->head = (rb->head + 1) % rb->size;
        rb->used -= 1;
    }
    return ch;
}

/*
 * Roger Clark. 20141125, 
 * added peek function.
 * @brief Return the first item from a ring buffer, without removing it
 * @param rb Buffer to remove from, must contain at least one element.
 */

static inline int16 rb_peek(ring_buffer *rb) 
{  
    return rb->used == 0 ? -1 : rb->buf[rb->head];
}


/**
 * @brief Attempt to remove the first item from a ring buffer.
 *
 * If the ring buffer is nonempty, removes and returns its first item.
 * If it is empty, does nothing and returns a negative value.
 *
 * @param rb Buffer to attempt to remove from.
 */
static inline int16 rb_safe_remove(ring_buffer *rb) {
    return rb_remove(rb);
}

/**
 * @brief Attempt to insert an element into a ring buffer.
 *
 * @param rb Buffer to insert into.
 * @param element Value to insert into rb.
 * @sideeffect If rb is not full, appends element onto buffer.
 * @return If element was appended, then true; otherwise, false. */
static inline int rb_safe_insert(ring_buffer *rb, uint8 element) {
    if (rb_is_full(rb)) {
        return 0;
    }
    rb_insert(rb, element);
    return 1;
}

/**
 * @brief Append an item onto the end of a non-full ring buffer.
 *
 * If the buffer is full, removes its first item, then inserts the new
 * element at the end.
 *
 * @param rb Ring buffer to insert into.
 * @param element Value to insert into ring buffer.
 * @return On success, returns -1.  If an element was popped, returns
 *         the popped value.
 */
static inline int16 rb_push_insert(ring_buffer *rb, uint8 element) {
    int ret = -1;
    if (rb_is_full(rb)) {
        ret = rb_remove(rb);
    }
    rb_insert(rb, element);
    return ret;
}

/**
 * @brief Discard all items from a ring buffer.
 * @param rb Ring buffer to discard all items from.
 */
static inline void rb_reset(ring_buffer *rb) {
    rb->used=0;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
