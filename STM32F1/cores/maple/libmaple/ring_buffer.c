#include <libmaple/ring_buffer.h>

void rb_init(ring_buffer *rb, uint16 size) {
    rb->head = 0;
    rb->used = 0;
    rb->size = size == 0 ? 1 : size;
    if(rb->buf != NULL)
        free(rb->buf); // free previously allocated buffer if any
    rb->buf = (uint8 *) malloc(rb->size * sizeof(uint8));
}
