
#ifndef DFU_FILE_H
#define DFU_FILE_H

#include <stdint.h>

struct dfu_file {
    /* File name */
    const char *name;
    /* Pointer to file loaded into memory */
    uint8_t *firmware;
    /* Different sizes */
    struct {
	int total;
	int prefix;
	int suffix;
    } size;
    /* From prefix fields */
    uint32_t lmdfu_address;
    /* From prefix fields */
    uint32_t prefix_type;

    /* From DFU suffix fields */
    uint32_t dwCRC;
    uint16_t bcdDFU;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
};

enum suffix_req {
	NO_SUFFIX,
	NEEDS_SUFFIX,
	MAYBE_SUFFIX
};

enum prefix_req {
	NO_PREFIX,
	NEEDS_PREFIX,
	MAYBE_PREFIX
};

enum prefix_type {
	ZERO_PREFIX,
	LMDFU_PREFIX,
	LPCDFU_UNENCRYPTED_PREFIX
};

extern int verbose;

void dfu_load_file(struct dfu_file *file, enum suffix_req check_suffix, enum prefix_req check_prefix);
void dfu_store_file(struct dfu_file *file, int write_suffix, int write_prefix);

void dfu_progress_bar(const char *desc, unsigned long long curr,
		unsigned long long max);
void *dfu_malloc(size_t size);
uint32_t dfu_file_write_crc(int f, uint32_t crc, const void *buf, int size);
void show_suffix_and_prefix(struct dfu_file *file);

#endif /* DFU_FILE_H */
