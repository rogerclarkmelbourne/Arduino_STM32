#include <libmaple/ring_buffer.h>

void rb_init_new(ring_buffer *rb, uint16 size) {
    rb->head = 0;
    rb->used = 0;
    size = size == 0 ? 1 : size;
    if(rb->size != size) 
    {
        // (rb->size != size) means either no buffer allocated yet or new buffer size different from old buffer size 
        //                    this leads to the need to deallocate the old buffer (if any) and allocate the new one
        if(rb->buf != NULL)
            free(rb->buf);
        rb->buf = (uint8 *) malloc(size * sizeof(uint8));
        rb->size = size;
    }
}
