#ifndef DFU_QUIRKS_H
#define DFU_QUIRKS_H

#define VENDOR_OPENMOKO 0x1d50 /* Openmoko Freerunner / GTA02 */
#define VENDOR_FIC      0x1457 /* Openmoko Freerunner / GTA02 */
#define VENDOR_VOTI	0x16c0 /* OpenPCD Reader */
#define VENDOR_LEAFLABS 0x1eaf /* Maple */
#define VENDOR_SIEMENS 0x0908 /* Siemens AG */
#define VENDOR_MIDIMAN  0x0763 /* Midiman */

#define PRODUCT_FREERUNNER_FIRST 0x5117
#define PRODUCT_FREERUNNER_LAST  0x5126
#define PRODUCT_OPENPCD	0x076b
#define PRODUCT_MAPLE3	0x0003 /* rev 3 and 5 */
#define PRODUCT_PXM40	0x02c4 /* Siemens AG, PXM 40 */
#define PRODUCT_PXM50	0x02c5 /* Siemens AG, PXM 50 */
#define PRODUCT_TRANSIT	0x2806 /* M-Audio Transit (Midiman) */

#define QUIRK_POLLTIMEOUT  (1<<0)
#define QUIRK_FORCE_DFU11  (1<<1)

/* Fallback value, works for OpenMoko */
#define DEFAULT_POLLTIMEOUT  5

uint16_t get_quirks(uint16_t vendor, uint16_t product, uint16_t bcdDevice);

#endif /* DFU_QUIRKS_H */
