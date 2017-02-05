#ifndef DFU_LOAD_H
#define DFU_LOAD_H

int dfuload_do_upload(struct dfu_if *dif, int xfer_size, int expected_size, int fd);
int dfuload_do_dnload(struct dfu_if *dif, int xfer_size, struct dfu_file *file);

#endif /* DFU_LOAD_H */
