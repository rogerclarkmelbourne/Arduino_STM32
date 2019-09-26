#include "usb_mass.h"
#include "usb_mass_mal.h"
#include "usb_mass_internal.h"
#include "usb_scsi.h"

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

#define SCSI_READ_FORMAT_CAPACITY_DATA_LEN   0x0C
#define SCSI_READ_FORMAT_CAPACITY10_DATA_LEN 0x08
#define SCSI_MODE_SENSE6_DATA_LEN            0x04
#define SCSI_MODE_SENSE10_DATA_LEN           0x08

#define SCSI_TXFR_IDLE     0
#define SCSI_TXFR_ONGOING  1

extern uint32_t usb_mass_sil_write(uint8_t* pBufferPointer, uint32_t wBufferSize);

/* See usb_scsi_data.c */
extern uint8_t SCSI_page00InquiryData[];
extern uint8_t SCSI_standardInquiryData[];
extern uint8_t SCSI_standardInquiryData2[];
extern uint8_t SCSI_senseData[];
extern uint8_t SCSI_modeSense6Data[];
extern uint8_t SCSI_modeSense10Data[];
extern uint8_t SCSI_readFormatCapacityData[];
extern uint8_t SCSI_readFormatCapacity10Data[];

uint32_t SCSI_lba;
uint32_t SCSI_blkLen;
uint8_t SCSI_transferState = SCSI_TXFR_IDLE;
uint32_t SCSI_blockReadCount = 0;
uint32_t SCSI_blockOffset;
uint32_t SCSI_counter = 0;
uint8_t SCSI_dataBuffer[512]; /* 512 bytes (SDCard block size) */

uint8_t scsi_address_management(uint8_t lun, uint8_t cmd, uint32_t lba, uint32_t blockNbr);
void scsi_read_memory(uint8_t lun, uint32_t memoryOffset, uint32_t transferLength);
void scsi_write_memory(uint8_t lun, uint32_t memoryOffset, uint32_t transferLength);

void scsi_inquiry_cmd(uint8_t lun) {
  uint8_t* inquiryData;
  uint16_t inquiryDataLength;

  if (usb_mass_CBW.CB[1] & 0x01) /*Evpd is set*/ {
    inquiryData = SCSI_page00InquiryData;
    inquiryDataLength = 5;
  } else {
    if (lun == 0) {
      inquiryData = SCSI_standardInquiryData;
    } else {
      inquiryData = SCSI_standardInquiryData2;
    }

    if (usb_mass_CBW.CB[4] <= SCSI_STANDARD_INQUIRY_DATA_LEN) {
      inquiryDataLength = usb_mass_CBW.CB[4];
    } else {
      inquiryDataLength = SCSI_STANDARD_INQUIRY_DATA_LEN;
    }
  }
  usb_mass_transfer_data_request(inquiryData, inquiryDataLength);
}

void scsi_request_sense_cmd(uint8_t lun) {
  uint8_t requestSenseDataLength;
  if (usb_mass_CBW.CB[4] <= SCSI_REQUEST_SENSE_DATA_LEN) {
    requestSenseDataLength = usb_mass_CBW.CB[4];
  } else {
    requestSenseDataLength = SCSI_REQUEST_SENSE_DATA_LEN;
  }
  usb_mass_transfer_data_request(SCSI_senseData, requestSenseDataLength);
}

void scsi_start_stop_unit_cmd(uint8_t lun) {
  usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
}

void scsi_mode_sense6_cmd(uint8_t lun) {
  usb_mass_transfer_data_request(SCSI_modeSense6Data, SCSI_MODE_SENSE6_DATA_LEN);
}

void scsi_mode_sense10_cmd(uint8_t lun) {
  usb_mass_transfer_data_request(SCSI_modeSense10Data, SCSI_MODE_SENSE10_DATA_LEN);
}

void scsi_read_format_capacity_cmd(uint8_t lun) {
  if (usb_mass_mal_get_status(lun)) {
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_NOT_READY, SCSI_MEDIUM_NOT_PRESENT);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_ENABLE);
    usb_mass_bot_abort(BOT_DIR_IN);
    return;
  }
  SCSI_readFormatCapacityData[4] = (uint8_t) (usb_mass_drives[lun].blockCount >> 24);
  SCSI_readFormatCapacityData[5] = (uint8_t) (usb_mass_drives[lun].blockCount >> 16);
  SCSI_readFormatCapacityData[6] = (uint8_t) (usb_mass_drives[lun].blockCount >> 8);
  SCSI_readFormatCapacityData[7] = (uint8_t) (usb_mass_drives[lun].blockCount);

  SCSI_readFormatCapacityData[9] = (uint8_t) (SCSI_BLOCK_SIZE >> 16);
  SCSI_readFormatCapacityData[10] = (uint8_t) (SCSI_BLOCK_SIZE >> 8);
  SCSI_readFormatCapacityData[11] = (uint8_t) (SCSI_BLOCK_SIZE);
  usb_mass_transfer_data_request(SCSI_readFormatCapacityData, SCSI_READ_FORMAT_CAPACITY_DATA_LEN);
}

void scsi_read_capacity10_cmd(uint8_t lun) {
  if (usb_mass_mal_get_status(lun)) {
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_NOT_READY, SCSI_MEDIUM_NOT_PRESENT);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_ENABLE);
    usb_mass_bot_abort(BOT_DIR_IN);
    return;
  }
  
  SCSI_readFormatCapacity10Data[0] = (uint8_t) ((usb_mass_drives[lun].blockCount - 1) >> 24);
  SCSI_readFormatCapacity10Data[1] = (uint8_t) ((usb_mass_drives[lun].blockCount - 1) >> 16);
  SCSI_readFormatCapacity10Data[2] = (uint8_t) ((usb_mass_drives[lun].blockCount - 1) >> 8);
  SCSI_readFormatCapacity10Data[3] = (uint8_t) (usb_mass_drives[lun].blockCount - 1);

  SCSI_readFormatCapacity10Data[4] = (uint8_t) (SCSI_BLOCK_SIZE >> 24);
  SCSI_readFormatCapacity10Data[5] = (uint8_t) (SCSI_BLOCK_SIZE >> 16);
  SCSI_readFormatCapacity10Data[6] = (uint8_t) (SCSI_BLOCK_SIZE >> 8);
  SCSI_readFormatCapacity10Data[7] = (uint8_t) (SCSI_BLOCK_SIZE);
  usb_mass_transfer_data_request(SCSI_readFormatCapacity10Data, SCSI_READ_FORMAT_CAPACITY10_DATA_LEN);
}

void scsi_read10_cmd(uint8_t lun, uint32_t lba, uint32_t blockNbr) {
  if (usb_mass_botState == BOT_STATE_IDLE) {
    if (!(scsi_address_management(usb_mass_CBW.bLUN, SCSI_READ10, lba, blockNbr))) /*address out of range*/ {
      return;
    }

    if ((usb_mass_CBW.bmFlags & 0x80) != 0) {
      usb_mass_botState = BOT_STATE_DATA_IN;
      scsi_read_memory(lun, lba, blockNbr);
    } else {
      usb_mass_bot_abort(BOT_DIR_BOTH);
      scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
      usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_ENABLE);
    }
    return;
  } else if (usb_mass_botState == BOT_STATE_DATA_IN) {
    scsi_read_memory(lun, lba, blockNbr);
  }
}

void scsi_write10_cmd(uint8_t lun, uint32_t lba, uint32_t blockNbr) {
  if (usb_mass_botState == BOT_STATE_IDLE) {
    if (!(scsi_address_management(usb_mass_CBW.bLUN, SCSI_WRITE10, lba, blockNbr)))/*address out of range*/ {
      return;
    }

    if ((usb_mass_CBW.bmFlags & 0x80) == 0) {
      usb_mass_botState = BOT_STATE_DATA_OUT;
      SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_VAL);
    } else {
      usb_mass_bot_abort(BOT_DIR_IN);
      scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
      usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
    }
    return;
  } else if (usb_mass_botState == BOT_STATE_DATA_OUT) {
    scsi_write_memory(lun, lba, blockNbr);
  }
}

void scsi_test_unit_ready_cmd(uint8_t lun) {
  if (usb_mass_mal_get_status(lun)) {
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_NOT_READY, SCSI_MEDIUM_NOT_PRESENT);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_ENABLE);
    usb_mass_bot_abort(BOT_DIR_IN);
    return;
  } else {
    usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
  }
}

void scsi_verify10_cmd(uint8_t lun) {
  if ((usb_mass_CBW.dDataLength == 0) && !(usb_mass_CBW.CB[1] & SCSI_BLKVFY))/* BLKVFY not set*/ {
    usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
  } else {
    usb_mass_bot_abort(BOT_DIR_BOTH);
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
  }
}

void scsi_format_cmd(uint8_t lun) {
  if (usb_mass_mal_get_status(lun)) {
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_NOT_READY, SCSI_MEDIUM_NOT_PRESENT);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_ENABLE);
    usb_mass_bot_abort(BOT_DIR_IN);
    return;
  }
  usb_mass_mal_format(lun);
  usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
}

void scsi_set_sense_data(uint8_t lun, uint8_t sensKey, uint8_t asc) {
  SCSI_senseData[2] = sensKey;
  SCSI_senseData[12] = asc;
}

void scsi_invalid_cmd(uint8_t lun) {
  if (usb_mass_CBW.dDataLength == 0) {
    usb_mass_bot_abort(BOT_DIR_IN);
  } else {
    if ((usb_mass_CBW.bmFlags & 0x80) != 0) {
      usb_mass_bot_abort(BOT_DIR_IN);
    } else {
      usb_mass_bot_abort(BOT_DIR_BOTH);
    }
  }
  scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_COMMAND);
  usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
}

uint8_t scsi_address_management(uint8_t lun, uint8_t cmd, uint32_t lba, uint32_t blockNbr) {

  if ((lba + blockNbr) > usb_mass_drives[lun].blockCount) {
    if (cmd == SCSI_WRITE10) {
      usb_mass_bot_abort(BOT_DIR_BOTH);
    }
    usb_mass_bot_abort(BOT_DIR_IN);
    scsi_set_sense_data(lun, SCSI_ILLEGAL_REQUEST, SCSI_ADDRESS_OUT_OF_RANGE);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
    return (FALSE);
  }


  if (usb_mass_CBW.dDataLength != blockNbr * SCSI_BLOCK_SIZE) {
    if (cmd == SCSI_WRITE10) {
      usb_mass_bot_abort(BOT_DIR_BOTH);
    } else {
      usb_mass_bot_abort(BOT_DIR_IN);
    }
    scsi_set_sense_data(usb_mass_CBW.bLUN, SCSI_ILLEGAL_REQUEST, SCSI_INVALID_FIELED_IN_COMMAND);
    usb_mass_bot_set_csw(BOT_CSW_CMD_FAILED, BOT_SEND_CSW_DISABLE);
    return (FALSE);
  }
  return (TRUE);
}

void scsi_read_memory(uint8_t lun, uint32_t startSector, uint32_t numSectors) {
  static uint32_t length;
  static uint64_t offset;

  if (SCSI_transferState == SCSI_TXFR_IDLE) {
    offset = (uint64_t)startSector * SCSI_BLOCK_SIZE;
    length = numSectors * SCSI_BLOCK_SIZE;
    SCSI_transferState = SCSI_TXFR_ONGOING;
  }

  if (SCSI_transferState == SCSI_TXFR_ONGOING) {
    if (SCSI_blockReadCount == 0) {
      usb_mass_mal_read_memory(lun, SCSI_dataBuffer, (uint32_t)(offset/SCSI_BLOCK_SIZE), 1);

      usb_mass_sil_write(SCSI_dataBuffer, MAX_BULK_PACKET_SIZE);

      SCSI_blockReadCount = SCSI_BLOCK_SIZE - MAX_BULK_PACKET_SIZE;
      SCSI_blockOffset = MAX_BULK_PACKET_SIZE;
    } else {
      usb_mass_sil_write(SCSI_dataBuffer + SCSI_blockOffset, MAX_BULK_PACKET_SIZE);

      SCSI_blockReadCount -= MAX_BULK_PACKET_SIZE;
      SCSI_blockOffset += MAX_BULK_PACKET_SIZE;
    }

    SetEPTxStatus(USB_MASS_TX_ENDP, USB_EP_ST_TX_VAL);

    offset += MAX_BULK_PACKET_SIZE;
    length -= MAX_BULK_PACKET_SIZE;

    usb_mass_CSW.dDataResidue -= MAX_BULK_PACKET_SIZE;
    usb_mass_CSW.bStatus = BOT_CSW_CMD_PASSED;
    // TODO: Led_RW_ON();
  }

  if (length == 0) {
    SCSI_blockReadCount = 0;
    SCSI_blockOffset = 0;
    offset = 0;
    usb_mass_botState = BOT_STATE_DATA_IN_LAST;
    SCSI_transferState = SCSI_TXFR_IDLE;
    // TODO: Led_RW_OFF();
  }
}

void scsi_write_memory(uint8_t lun, uint32_t startSector, uint32_t numSectors) {
  static uint32_t length;
  static uint64_t offset;
  uint32_t idx;
  uint32_t temp = SCSI_counter + 64;

  if (SCSI_transferState == SCSI_TXFR_IDLE) {
    offset = (uint64_t)startSector * SCSI_BLOCK_SIZE;
    length = numSectors * SCSI_BLOCK_SIZE;
    SCSI_transferState = SCSI_TXFR_ONGOING;
  }

  if (SCSI_transferState == SCSI_TXFR_ONGOING) {

    for (idx = 0; SCSI_counter < temp; SCSI_counter++) {
      *((uint8_t *) SCSI_dataBuffer + SCSI_counter) = usb_mass_bulkDataBuff[idx++];
    }

    offset += usb_mass_dataLength;
    length -= usb_mass_dataLength;

    if (!(length % SCSI_BLOCK_SIZE)) {
      SCSI_counter = 0;
      usb_mass_mal_write_memory(lun, SCSI_dataBuffer, (uint32_t)(offset/SCSI_BLOCK_SIZE) - 1, 1);
    }

    usb_mass_CSW.dDataResidue -= usb_mass_dataLength;
    SetEPRxStatus(USB_MASS_RX_ENDP, USB_EP_ST_RX_VAL); /* enable the next transaction*/

    // TODO: Led_RW_ON();
  }

  if ((length == 0) || (usb_mass_botState == BOT_STATE_CSW_Send)) {
    SCSI_counter = 0;
    usb_mass_bot_set_csw(BOT_CSW_CMD_PASSED, BOT_SEND_CSW_ENABLE);
    SCSI_transferState = SCSI_TXFR_IDLE;
    // TODO: Led_RW_OFF();
  }
}
