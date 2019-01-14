
#ifndef _LIBMAPLE_USB_MASS_H_
#define _LIBMAPLE_USB_MASS_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/gpio.h>
#include <libmaple/usb.h>
#include <inttypes.h>
#include "usb_generic.h"
#include "usb_mass_mal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define N_STRING_DESCRIPTORS 4

// 512 needs to be divisible by the following two values
#define MAX_PACKET_SIZE            64  /* 64B, maximum for USB FS Devices */
#define MAX_BULK_PACKET_SIZE       64  /* 64B, max bulk  Can't use 512 because the internal buffers for USB is only 512B */
#if 512/MAX_PACKET_SIZE*MAX_PACKET_SIZE != 512
# error MAX_PACKET_SIZE must divide 512
#endif
#if 512/MAX_BULK_PACKET_SIZE*MAX_BULK_PACKET_SIZE != 512
# error MAX_BULK_PACKET_SIZE must divide 512
#endif


  /* MASS Storage Requests */
#define REQUEST_GET_MAX_LUN                0xFE
#define REQUEST_MASS_STORAGE_RESET         0xFF

  /* USB device state */
  typedef enum _DEVICE_STATE {
    DEVICE_STATE_UNCONNECTED,
    DEVICE_STATE_ATTACHED,
    DEVICE_STATE_POWERED,
    DEVICE_STATE_SUSPENDED,
    DEVICE_STATE_ADDRESSED,
    DEVICE_STATE_CONFIGURED
  } DEVICE_STATE;

#define BOT_DIR_IN                    0
#define BOT_DIR_OUT                   1
#define BOT_DIR_BOTH                  2

  /*****************************************************************************/
  /*********************** Bulk-Only Transfer State machine ********************/
  /*****************************************************************************/
#define BOT_STATE_IDLE                0       /* Idle state */
#define BOT_STATE_DATA_OUT            1       /* Data Out state */
#define BOT_STATE_DATA_IN             2       /* Data In state */
#define BOT_STATE_DATA_IN_LAST        3       /* Last Data In Last */
#define BOT_STATE_CSW_Send            4       /* Command Status Wrapper */
#define BOT_STATE_ERROR               5       /* error state */

#define BOT_CBW_SIGNATURE             0x43425355
#define BOT_CSW_SIGNATURE             0x53425355
#define BOT_CBW_PACKET_LENGTH         31

#define BOT_CSW_DATA_LENGTH           0x000D

  /* CSW Status Definitions */
#define BOT_CSW_CMD_PASSED            0x00
#define BOT_CSW_CMD_FAILED            0x01
#define BOT_CSW_PHASE_ERROR           0x02

#define BOT_SEND_CSW_DISABLE          0
#define BOT_SEND_CSW_ENABLE           1

#define USB_EP1_IN 0x81

  /* Bulk-only Command Block Wrapper */
  typedef struct _BulkOnlyCBW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataLength;
    uint8_t bmFlags;
    uint8_t bLUN;
    uint8_t bCBLength;
    uint8_t CB[16];
  } BulkOnlyCBW;

  /* Bulk-only Command Status Wrapper */
  typedef struct _BulkOnlyCSW {
    uint32_t dSignature;
    uint32_t dTag;
    uint32_t dDataResidue;
    uint8_t bStatus;
  } BulkOnlyCSW;

  typedef struct _usb_descriptor_config {
    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface MASS_Interface;
    usb_descriptor_endpoint DataInEndpoint;
    usb_descriptor_endpoint DataOutEndpoint;
  } __packed usb_descriptor_config;

  void usb_mass_enable(gpio_dev *disc_dev, uint8 disc_bit);
  void usb_mass_disable(gpio_dev *disc_dev, uint8 disc_bit);
  void usb_mass_loop();

  void usb_mass_bot_set_csw(uint8_t cswStatus, uint8_t sendPermission);
  void usb_mass_transfer_data_request(uint8_t* dataPointer, uint16_t dataLen);
  void usb_mass_bot_abort(uint8_t direction);
  
  extern USBCompositePart usbMassPart;

extern uint8_t usb_mass_botState;
extern BulkOnlyCBW usb_mass_CBW;
extern BulkOnlyCSW usb_mass_CSW;
extern uint8_t usb_mass_bulkDataBuff[MAX_BULK_PACKET_SIZE];
extern uint16_t usb_mass_dataLength;

#ifdef __cplusplus
}
#endif

#endif
