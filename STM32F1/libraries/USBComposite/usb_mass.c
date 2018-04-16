#include <string.h>

#include "usb_generic.h"
#include "usb_mass.h"
#include "usb_scsi.h"
#include "usb_mass_internal.h"

#include <libmaple/usb.h>
#include <libmaple/nvic.h>
#include <libmaple/delay.h>

/* Private headers */
#include "usb_lib_globals.h"
#include "usb_reg_map.h"
#include "usb_regs.h"

/* usb_lib headers */
#include "usb_type.h"
#include "usb_core.h"
#include "usb_def.h"

static void usb_mass_bot_cbw_decode();

static void usb_mass_set_configuration();
static void usb_mass_clear_feature();
static RESULT usb_mass_data_setup(uint8 request);
static RESULT usb_mass_no_data_setup(uint8 request);
static void usb_mass_reset();
static uint8_t* usb_mass_get_max_lun(uint16_t Length);
static void usb_mass_in(void);
static void usb_mass_out(void);
uint32_t usb_mass_sil_write(uint8_t* pBufferPointer, uint32_t wBufferSize);
uint32_t usb_mass_sil_read(uint8_t* pBufferPointer);

#define MASS_INTERFACE_OFFSET 	0x00
#define MASS_INTERFACE_NUMBER (MASS_INTERFACE_OFFSET+usbMassPart.startInterface)


#define LUN_DATA_LENGTH            1

static uint32_t maxLun = 0;
static uint32_t deviceState = DEVICE_STATE_UNCONNECTED;
uint8_t usb_mass_botState = BOT_STATE_IDLE;
BulkOnlyCBW usb_mass_CBW;
BulkOnlyCSW usb_mass_CSW;
uint8_t usb_mass_bulkDataBuff[MAX_BULK_PACKET_SIZE];
uint16_t usb_mass_dataLength;
static uint8_t inRequestPending;
static uint8_t outRequestPending;

typedef struct mass_descriptor_config {
//    usb_descriptor_config_header Config_Header;
    usb_descriptor_interface MASS_Interface;
    usb_descriptor_endpoint DataInEndpoint;
    usb_descriptor_endpoint DataOutEndpoint;
} __packed mass_descriptor_config;


#define MAX_POWER (500 >> 1)
const mass_descriptor_config usbMassConfigDescriptor = {
  /*.Config_Header =
  {
    .bLength = sizeof (usb_descriptor_config_header),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_CONFIGURATION,
    .wTotalLength = sizeof (usb_descriptor_config),
    .bNumInterfaces = 0x01,
    .bConfigurationValue = 0x01,
    .iConfiguration = 0x00,
    .bmAttributes = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELF_POWERED),
    .bMaxPower = MAX_POWER,
  }, */

  .MASS_Interface =
  {
    .bLength = sizeof (usb_descriptor_interface),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
    .bInterfaceNumber = 0x00, // PATCH
    .bAlternateSetting = 0x00,
    .bNumEndpoints = 0x02,
    .bInterfaceClass = 8, // mass storage
    .bInterfaceSubClass = 6, // SCSI
    .bInterfaceProtocol = 0x50, // Bulk-Only
    .iInterface = 0,
  },

  .DataInEndpoint =
  {
    .bLength = sizeof (usb_descriptor_endpoint),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_IN | MASS_ENDPOINT_TX), // PATCH
    .bmAttributes = USB_EP_TYPE_BULK,
    .wMaxPacketSize = MAX_BULK_PACKET_SIZE,
    .bInterval = 0,
  },

  .DataOutEndpoint =
  {
    .bLength = sizeof (usb_descriptor_endpoint),
    .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
    .bEndpointAddress = (USB_DESCRIPTOR_ENDPOINT_OUT | MASS_ENDPOINT_RX), // PATCH
    .bmAttributes = USB_EP_TYPE_BULK,
    .wMaxPacketSize = MAX_BULK_PACKET_SIZE,
    .bInterval = 1,
  }
};

USBEndpointInfo usbMassEndpoints[2] = {
    {
        .callback = usb_mass_in,
        .bufferSize = MAX_BULK_PACKET_SIZE,
        .type = USB_EP_EP_TYPE_BULK, 
        .tx = 1,
    },
    {
        .callback = usb_mass_out,
        .bufferSize = MAX_BULK_PACKET_SIZE,
        .type = USB_EP_EP_TYPE_BULK, 
        .tx = 0,
    },
};

#define OUT_BYTE(s,v) out[(uint8*)&(s.v)-(uint8*)&s]

static void getMassPartDescriptor(uint8* out) {
    memcpy(out, &usbMassConfigDescriptor, sizeof(mass_descriptor_config));
    // patch to reflect where the part goes in the descriptor
    OUT_BYTE(usbMassConfigDescriptor, MASS_Interface.bInterfaceNumber) += usbMassPart.startInterface;
    OUT_BYTE(usbMassConfigDescriptor, DataInEndpoint.bEndpointAddress) += usbMassPart.startEndpoint;
    OUT_BYTE(usbMassConfigDescriptor, DataOutEndpoint.bEndpointAddress) += usbMassPart.startEndpoint;
}



USBCompositePart usbMassPart = {
    .numInterfaces = 1,
    .numEndpoints = sizeof(usbMassEndpoints)/sizeof(*usbMassEndpoints),
    .descriptorSize = sizeof(mass_descriptor_config),
    .getPartDescriptor = getMassPartDescriptor,
    .usbInit = NULL,
    .usbReset = usb_mass_reset,
    .usbDataSetup = usb_mass_data_setup,
    .usbNoDataSetup = usb_mass_no_data_setup,
    .usbClearFeature = usb_mass_clear_feature,
    .usbSetConfiguration = usb_mass_set_configuration,
    .endpoints = usbMassEndpoints
};

static void usb_mass_reset(void) {
  usb_mass_mal_init(0);

  pInformation->Current_Configuration = 0; // TODO: remove?

  /* current feature is current bmAttributes */
  pInformation->Current_Feature = (USB_CONFIG_ATTR_BUSPOWERED | USB_CONFIG_ATTR_SELF_POWERED); // usbMassConfigDescriptor.Config_Header.bmAttributes; // TODO: remove?

  deviceState = DEVICE_STATE_ATTACHED;
  usb_mass_CBW.dSignature = BOT_CBW_SIGNATURE;
  usb_mass_botState = BOT_STATE_IDLE;
}

static void usb_mass_set_configuration(void) {
  if (pInformation->Current_Configuration != 0) {
    deviceState = USB_CONFIGURED;
    ClearDTOG_TX(USB_MASS_TX_ENDP);
    ClearDTOG_RX(USB_MASS_RX_ENDP);
    usb_mass_botState = BOT_STATE_IDLE;
  }
}

static void usb_mass_clear_feature(void) {
  /* when the host send a usb_mass_CBW with invalid signature or invalid length the two
   Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
  if (usb_mass_CBW.dSignature != BOT_CBW_SIGNATURE) {
    usb_mass_bot_abort(BOT_DIR_BOTH);
  }
}

static RESULT usb_mass_data_setup(uint8 request) {
  uint8_t * (*copy_routine)(uint16_t);

  copy_routine = NULL;
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
          && (request == REQUEST_GET_MAX_LUN) && (pInformation->USBwValue == 0)
          && (pInformation->USBwIndex == MASS_INTERFACE_NUMBER) && (pInformation->USBwLength == 0x01)) {
    copy_routine = usb_mass_get_max_lun;
  } else {
    return USB_UNSUPPORT;
  }

  if (copy_routine == NULL) {
    return USB_UNSUPPORT;
  }

  pInformation->Ctrl_Info.CopyData = copy_routine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*copy_routine)(0);

  return USB_SUCCESS;
}

static uint8_t* usb_mass_get_max_lun(uint16_t length) {
  if (length == 0) {
    pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
    return 0;
  } else {
    return ((uint8_t*) (&maxLun));
  }
}

static RESULT usb_mass_no_data_setup(uint8 request) {
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
          && (request == REQUEST_MASS_STORAGE_RESET) && (pInformation->USBwValue == 0)
          && (pInformation->USBwIndex == MASS_INTERFACE_NUMBER) && (pInformation->USBwLength == 0x00)) {

    /* Initialize Endpoint 1 */
    ClearDTOG_TX(USB_MASS_TX_ENDP);

    /* Initialize Endpoint 2 */
    ClearDTOG_RX(USB_MASS_RX_ENDP);

    /*initialize the usb_mass_CBW signature to enable the clear feature*/
    usb_mass_CBW.dSignature = BOT_CBW_SIGNATURE;
    usb_mass_botState = BOT_STATE_IDLE;

    return USB_SUCCESS;
  }
  return USB_UNSUPPORT;
}


void usb_mass_loop() {
  if (inRequestPending) {
    inRequestPending = 0;

    switch (usb_mass_botState) {
      case BOT_STATE_CSW_Send:
      case BOT_STATE_ERROR:
        usb_mass_botState = BOT_STATE_IDLE;
        SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_VAL); /* enable the Endpoint to receive the next cmd*/
        break;
      case BOT_STATE_DATA_IN:
        switch (usb_mass_CBW.CB[0]) {
          case SCSI_READ10:
            scsi_read10_cmd(usb_mass_CBW.bLUN, SCSI_lba, SCSI_blkLen);
            break;
        }
        break;
      case BOT_STATE_DATA_IN_LAST:
        usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
        SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_VAL);
        break;

      default:
        break;
    }
  }

  if (outRequestPending) {
    outRequestPending = 0;

    uint8_t CMD;
    CMD = usb_mass_CBW.CB[0];

    switch (usb_mass_botState) {
      case BOT_STATE_IDLE:
        usb_mass_bot_cbw_decode();
        break;
      case BOT_STATE_DATA_OUT:
        if (CMD == SCSI_WRITE10) {
          scsi_write10_cmd(usb_mass_CBW.bLUN, SCSI_lba, SCSI_blkLen);
          break;
        }
        usb_mass_bot_abort(BOT_DIR_OUT);
        scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
        usb_mass_bot_set_csw(BOT_CSW_PHASE_ERROR, BOT_SEND_CSW_DISABLE);
        break;
      default:
        usb_mass_bot_abort(BOT_DIR_BOTH);
        scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
        usb_mass_bot_set_csw(BOT_CSW_PHASE_ERROR, BOT_SEND_CSW_DISABLE);
        break;
    }
  }
}

/*
 *  IN
 */
static void usb_mass_in(void) {
  inRequestPending = 1;
}

/*
 *  OUT
 */
static void usb_mass_out(void) {
  usb_mass_dataLength = usb_mass_sil_read(usb_mass_bulkDataBuff);
  outRequestPending = 1;
}

static void usb_mass_bot_cbw_decode() {
  uint32_t counter;

  for (counter = 0; counter < usb_mass_dataLength; counter++) {
    *((uint8_t *) & usb_mass_CBW + counter) = usb_mass_bulkDataBuff[counter];
  }
  usb_mass_CSW.dTag = usb_mass_CBW.dTag;
  usb_mass_CSW.dDataResidue = usb_mass_CBW.dDataLength;
  if (usb_mass_dataLength != BOT_CBW_PACKET_LENGTH) {
    usb_mass_bot_abort(BOT_DIR_BOTH);
    /* reset the usb_mass_CBW.dSignature to disable the clear feature until receiving a Mass storage reset*/
    usb_mass_CBW.dSignature = 0;
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_PARAMETER_LIST_LENGTH_ERROR);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
    return;
  }

  if ((usb_mass_CBW.CB[0] == SCSI_READ10) || (usb_mass_CBW.CB[0] == SCSI_WRITE10)) {
    /* Calculate Logical Block Address */
    SCSI_lba = (usb_mass_CBW.CB[2] << 24) | (usb_mass_CBW.CB[3] << 16) | (usb_mass_CBW.CB[4] << 8) | usb_mass_CBW.CB[5];
    /* Calculate the Number of Blocks to transfer */
    SCSI_blkLen = (usb_mass_CBW.CB[7] << 8) | usb_mass_CBW.CB[8];
  }

  if (usb_mass_CBW.dSignature == BOT_CBW_SIGNATURE) {
    /* Valid usb_mass_CBW */
    if ((usb_mass_CBW.bLUN > maxLun) || (usb_mass_CBW.bCBLength < 1) || (usb_mass_CBW.bCBLength > 16)) {
      usb_mass_bot_abort(BOT_DIR_BOTH);
      scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
      usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
    } else {
      switch (usb_mass_CBW.CB[0]) {
        case SCSI_REQUEST_SENSE:
          scsi_request_sense_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_INQUIRY:
          scsi_inquiry_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_START_STOP_UNIT:
          scsi_start_stop_unit_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_ALLOW_MEDIUM_REMOVAL:
          scsi_start_stop_unit_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_MODE_SENSE6:
          scsi_mode_sense6_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_MODE_SENSE10:
          scsi_mode_sense10_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_READ_FORMAT_CAPACITIES:
          scsi_read_format_capacity_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_READ_CAPACITY10:
          scsi_read_capacity10_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_TEST_UNIT_READY:
          scsi_test_unit_ready_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_READ10:
          scsi_read10_cmd(usb_mass_CBW.bLUN, SCSI_lba, SCSI_blkLen);
          break;
        case SCSI_WRITE10:
          scsi_write10_cmd(usb_mass_CBW.bLUN, SCSI_lba, SCSI_blkLen);
          break;
        case SCSI_VERIFY10:
          scsi_verify10_cmd(usb_mass_CBW.bLUN);
          break;
        case SCSI_FORMAT_UNIT:
          scsi_format_cmd(usb_mass_CBW.bLUN);
          break;

        case SCSI_MODE_SELECT10:
        case SCSI_MODE_SELECT6:
        case SCSI_SEND_DIAGNOSTIC:
        case SCSI_READ6:
        case SCSI_READ12:
        case SCSI_READ16:
        case SCSI_READ_CAPACITY16:
        case SCSI_WRITE6:
        case SCSI_WRITE12:
        case SCSI_VERIFY12:
        case SCSI_VERIFY16:
        case SCSI_WRITE16:
          scsi_invalid_cmd(usb_mass_CBW.bLUN);
          break;

        default:
        {
          usb_mass_bot_abort(BOT_DIR_BOTH);
          scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_COMMAND);
          usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
        }
      }
    }
  } else {
    /* Invalid usb_mass_CBW */
    usb_mass_bot_abort(BOT_DIR_BOTH);
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_COMMAND);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
  }
}

void usb_mass_bot_abort(uint8_t direction) {
  switch (direction) {
    case BOT_DIR_IN:
      SetEPTxStatus(USB_MASS_TX_ENDP, USB_EP_ST_TX_STL);
      break;
    case BOT_DIR_OUT:
      SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_STL);
      break;
    case BOT_DIR_BOTH:
      SetEPTxStatus(USB_MASS_TX_ENDP, USB_EP_ST_TX_STL);
      SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_STL);
      break;
    default:
      break;
  }
}

void usb_mass_transfer_data_request(uint8_t* dataPointer, uint16_t dataLen) {
  usb_mass_sil_write(dataPointer, dataLen);

  SetEPTxStatus(USB_MASS_TX_ENDP, USB_EP_ST_TX_VAL);
  usb_mass_botState = BOT_STATE_DATA_IN_LAST;
  usb_mass_CSW.dDataResidue -= dataLen;
  usb_mass_CSW.bStatus = BOT_CSW_CMD_PASSED;
}

void usb_mass_bot_set_csw(uint8_t status, uint8_t sendPermission) {
  usb_mass_CSW.dSignature = BOT_CSW_SIGNATURE;
  usb_mass_CSW.bStatus = status;

  usb_mass_sil_write(((uint8_t *) & usb_mass_CSW), BOT_CSW_DATA_LENGTH);

  usb_mass_botState = BOT_STATE_ERROR;
  if (sendPermission) {
    usb_mass_botState = BOT_STATE_CSW_Send;
    SetEPTxStatus(USB_MASS_TX_ENDP, USB_EP_ST_TX_VAL);
  }
}

uint32_t usb_mass_sil_write(uint8_t* pBufferPointer, uint32_t wBufferSize) {
  /* Use the memory interface function to write to the selected endpoint */
  usb_copy_to_pma(pBufferPointer, wBufferSize, USB_MASS_TX_ADDR);

  /* Update the data length in the control register */
  SetEPTxCount(USB_MASS_TX_ENDP, wBufferSize);

  return 0;
}

uint32_t usb_mass_sil_read(uint8_t* pBufferPointer) {
  uint32_t usb_mass_dataLength = 0;

  /* Get the number of received data on the selected Endpoint */
  usb_mass_dataLength = GetEPRxCount(USB_MASS_RX_ENDP);

  /* Use the memory interface function to write to the selected endpoint */
  usb_copy_from_pma(pBufferPointer, usb_mass_dataLength, USB_MASS_RX_ADDR);

  /* Return the number of received data */
  return usb_mass_dataLength;
}
