#ifndef __USB_SCSI_H
#define __USB_SCSI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  /* SCSI Commands */
#define SCSI_FORMAT_UNIT                            0x04
#define SCSI_INQUIRY                                0x12
#define SCSI_MODE_SELECT6                           0x15
#define SCSI_MODE_SELECT10                          0x55
#define SCSI_MODE_SENSE6                            0x1A
#define SCSI_MODE_SENSE10                           0x5A
#define SCSI_ALLOW_MEDIUM_REMOVAL                   0x1E
#define SCSI_READ6                                  0x08
#define SCSI_READ10                                 0x28
#define SCSI_READ12                                 0xA8
#define SCSI_READ16                                 0x88

#define SCSI_READ_CAPACITY10                        0x25
#define SCSI_READ_CAPACITY16                        0x9E

#define SCSI_REQUEST_SENSE                          0x03
#define SCSI_START_STOP_UNIT                        0x1B
#define SCSI_TEST_UNIT_READY                        0x00
#define SCSI_WRITE6                                 0x0A
#define SCSI_WRITE10                                0x2A
#define SCSI_WRITE12                                0xAA
#define SCSI_WRITE16                                0x8A

#define SCSI_VERIFY10                               0x2F
#define SCSI_VERIFY12                               0xAF
#define SCSI_VERIFY16                               0x8F

#define SCSI_SEND_DIAGNOSTIC                        0x1D
#define SCSI_READ_FORMAT_CAPACITIES                 0x23

#define SCSI_NO_SENSE		                            0
#define SCSI_RECOVERED_ERROR		                    1
#define SCSI_NOT_READY		                          2
#define SCSI_MEDIUM_ERROR		                        3
#define SCSI_HARDWARE_ERROR		                      4
#define SCSI_ILLEGAL_REQUEST		                    5
#define SCSI_UNIT_ATTENTION		                      6
#define SCSI_DATA_PROTECT		                        7
#define SCSI_BLANK_CHECK		                        8
#define SCSI_VENDOR_SPECIFIC		                    9
#define SCSI_COPY_ABORTED		                        10
#define SCSI_ABORTED_COMMAND		                    11
#define SCSI_VOLUME_OVERFLOW		                    13
#define SCSI_MISCOMPARE		                          14

#define SCSI_INVALID_COMMAND                        0x20
#define SCSI_INVALID_FIELED_IN_COMMAND              0x24
#define SCSI_PARAMETER_LIST_LENGTH_ERROR            0x1A
#define SCSI_INVALID_FIELD_IN_PARAMETER_LIST        0x26
#define SCSI_ADDRESS_OUT_OF_RANGE                   0x21
#define SCSI_MEDIUM_NOT_PRESENT 			              0x3A
#define SCSI_MEDIUM_HAVE_CHANGED			              0x28

#define SCSI_READ_FORMAT_CAPACITY_DATA_LEN          0x0C
#define SCSI_READ_CAPACITY10_DATA_LEN               0x08
#define SCSI_MODE_SENSE10_DATA_LEN                  0x08
#define SCSI_MODE_SENSE6_DATA_LEN                   0x04
#define SCSI_REQUEST_SENSE_DATA_LEN                 0x12
#define SCSI_STANDARD_INQUIRY_DATA_LEN              0x24
#define SCSI_BLKVFY                                 0x04

  extern uint32_t SCSI_lba;
  extern uint32_t SCSI_blkLen;

  void scsi_inquiry_cmd(uint8_t lun);
  void scsi_request_sense_cmd(uint8_t lun);
  void scsi_start_stop_unit_cmd(uint8_t lun);
  void scsi_mode_sense6_cmd(uint8_t lun);
  void scsi_mode_sense10_cmd(uint8_t lun);
  void scsi_read_format_capacity_cmd(uint8_t lun);
  void scsi_read_capacity10_cmd(uint8_t lun);
  void scsi_read10_cmd(uint8_t lun, uint32_t lba, uint32_t blockNbr);
  void scsi_write10_cmd(uint8_t lun, uint32_t lba, uint32_t blockNbr);
  void scsi_test_unit_ready_cmd(uint8_t lun);
  void scsi_verify10_cmd(uint8_t lun);
  void scsi_format_cmd(uint8_t lun);
  void scsi_set_sense_data(uint8_t lun, uint8_t sensKey, uint8_t asc);
  void scsi_invalid_cmd(uint8_t lun);

#ifdef __cplusplus
}
#endif

#endif
