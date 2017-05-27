
#ifndef DFU_FILE_H
#define DFU_FILE_H

#include <stdio.h>
#include <stdint.h>

struct dfu_file {
    const char *name;
    FILE *filep;
    long size;
    /* From DFU suffix fields */
    uint32_t dwCRC;
    unsigned char suffixlen;
    uint16_t bcdDFU;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
};

int parse_dfu_suffix(struct dfu_file *file);
int generate_dfu_suffix(struct dfu_file *file);

#endif /* DFU_FILE_H */
