#ifndef DFU_QUIRKS_H
#define DFU_QUIRKS_H

#define VENDOR_OPENMOKO 0x1d50 /* Openmoko Freerunner / GTA02 */
#define VENDOR_FIC      0x1457 /* Openmoko Freerunner / GTA02 */
#define VENDOR_VOTI	0x16c0 /* OpenPCD Reader */
#define VENDOR_LEAFLABS 0x1eaf /* Maple */
#define PRODUCT_MAPLE3	0x0003 /* rev 3 and 5 */

#define QUIRK_POLLTIMEOUT  (1<<0)
#define QUIRK_FORCE_DFU11  (1<<1)

/* Fallback value, works for OpenMoko */
#define DEFAULT_POLLTIMEOUT  5

extern int quirks;

void set_quirks(uint16_t vendor, uint16_t product, uint16_t bcdDevice);

#endif /* DFU_QUIRKS_H */
