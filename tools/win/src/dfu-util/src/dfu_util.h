#ifndef DFU_UTIL_H
#define DFU_UTIL_H

/* USB string descriptor should contain max 126 UTF-16 characters
 * but 253 would even accomodate any UTF-8 encoding */
#define MAX_DESC_STR_LEN 253

enum mode {
	MODE_NONE,
	MODE_VERSION,
	MODE_LIST,
	MODE_DETACH,
	MODE_UPLOAD,
	MODE_DOWNLOAD
};

extern struct dfu_if *dfu_root;
extern int match_bus;
extern int match_device;
extern int match_vendor;
extern int match_product;
extern int match_vendor_dfu;
extern int match_product_dfu;
extern int match_config_index;
extern int match_iface_index;
extern int match_iface_alt_index;
extern const char *match_iface_alt_name;
extern const char *match_serial;
extern const char *match_serial_dfu;

void probe_devices(libusb_context *);
void disconnect_devices(void);
void print_dfu_if(struct dfu_if *);
void list_dfu_interfaces(void);

#endif /* DFU_UTIL_H */
