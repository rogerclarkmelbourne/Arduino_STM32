
#include "usb_scsi.h"

uint8_t SCSI_page00InquiryData[] = {
  0x00, /* PERIPHERAL QUALIFIER & PERIPHERAL DEVICE TYPE*/
  0x00,
  0x00,
  0x00,
  0x00 /* Supported Pages 00*/
};

uint8_t SCSI_standardInquiryData[] = {
  0x00, /* Direct Access Device */
  0x80, /* RMB = 1: Removable Medium */
  0x02, /* Version: No conformance claim to standard */
  0x02,

  36 - 4, /* Additional Length */
  0x00, /* SCCS = 1: Storage Controller Component */
  0x00,
  0x00,
  /* Vendor Identification */
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ',
  /* Product Identification */
  'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
  'D', 'i', 's', 'k', ' ', ' ', ' ',
  /* Product Revision Level */
  '1', '.', '0', ' '
};

uint8_t SCSI_standardInquiryData2[] = {
  0x00, /* Direct Access Device */
  0x80, /* RMB = 1: Removable Medium */
  0x02, /* Version: No conformance claim to standard */
  0x02,

  36 - 4, /* Additional Length */
  0x00, /* SCCS = 1: Storage Controller Component */
  0x00,
  0x00,
  /* Vendor Identification */
  'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ',
  /* Product Identification */
  'N', 'A', 'N', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',
  'D', 'i', 's', 'k', ' ',
  /* Product Revision Level */
  '1', '.', '0', ' '
};

uint8_t SCSI_senseData[] = {
  0x70, /*RespCode*/
  0x00, /*SegmentNumber*/
  SCSI_NO_SENSE, /* Sens_Key*/
  0x00,
  0x00,
  0x00,
  0x00, /*Information*/
  0x0A, /*AdditionalSenseLength*/
  0x00,
  0x00,
  0x00,
  0x00, /*CmdInformation*/
  SCSI_NO_SENSE, /*Asc*/
  0x00, /*ASCQ*/
  0x00, /*FRUC*/
  0x00, /*TBD*/
  0x00,
  0x00 /*SenseKeySpecific*/
};

uint8_t SCSI_modeSense6Data[] = {
  0x03,
  0x00,
  0x00,
  0x00,
};

uint8_t SCSI_modeSense10Data[] = {
  0x00,
  0x06,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

uint8_t SCSI_readFormatCapacityData[] = {
  0x00,
  0x00,
  0x00,
  0x08, /* Capacity List Length */

  /* Block Count */
  0,
  0,
  0,
  0,

  /* Block Length */
  0x02, /* Descriptor Code: Formatted Media */
  0,
  0,
  0
};

uint8_t SCSI_readFormatCapacity10Data[] = {
  /* Last Logical Block */
  0,
  0,
  0,
  0,

  /* Block Length */
  0,
  0,
  0,
  0
};