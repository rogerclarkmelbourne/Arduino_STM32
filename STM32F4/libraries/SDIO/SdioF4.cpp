/* Arduino SdCard Library
 * Copyright (C) 2016 by William Greiman
 *
 * This file is part of the Arduino SdSpiCard Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdSpiCard Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include "SdioF4.h"
#include <libmaple/sdio.h>
#include <libmaple/dma.h>
#include <boards.h>

#define USE_DEBUG_MODE 0
#define USE_YIELD 0

//==============================================================================
//#define SDHC_PROCTL_DTW_4BIT 0x01
#define CMD8_RETRIES 10
#define BUSY_TIMEOUT_MILLIS 750
//==============================================================================
#define CMD_RESP_NONE SDIO_CMD_WAIT_NO_RESP
#define CMD_RESP_R1   SDIO_CMD_WAIT_SHORT_RESP // normal response
#define CMD_RESP_R1b  SDIO_CMD_WAIT_SHORT_RESP // normal response + busy data line (optional)
#define CMD_RESP_R2   SDIO_CMD_WAIT_LONG_RESP  // CID, CSD
#define CMD_RESP_R3   SDIO_CMD_WAIT_SHORT_RESP // OCR register, response to ACMD41
#define CMD_RESP_R6   SDIO_CMD_WAIT_SHORT_RESP // published RCA response, response to CMD3
#define CMD_RESP_R7   SDIO_CMD_WAIT_SHORT_RESP // response to CMD8

#define CMD0_XFERTYP    (uint16_t)( CMD0   | CMD_RESP_NONE )
#define CMD2_XFERTYP    (uint16_t)( CMD2   | CMD_RESP_R2   )
#define CMD3_XFERTYP    (uint16_t)( CMD3   | CMD_RESP_R6   )
#define CMD6_XFERTYP    (uint16_t)( CMD6   | CMD_RESP_R1   )
#define ACMD6_XFERTYP   (uint16_t)( ACMD6  | CMD_RESP_R1   )
#define CMD7_XFERTYP    (uint16_t)( CMD7   | CMD_RESP_R1b  )
#define CMD8_XFERTYP    (uint16_t)( CMD8   | CMD_RESP_R7   )
#define CMD9_XFERTYP    (uint16_t)( CMD9   | CMD_RESP_R2   )
#define CMD10_XFERTYP   (uint16_t)( CMD10  | CMD_RESP_R2   )
#define CMD12_XFERTYP   (uint16_t)( CMD12  | CMD_RESP_R1b  )
#define CMD13_XFERTYP   (uint16_t)( CMD13  | CMD_RESP_R1   )
#define CMD17_XFERTYP   (uint16_t)( CMD17  | CMD_RESP_R1   )
#define CMD18_XFERTYP   (uint16_t)( CMD18  | CMD_RESP_R1   )
#define ACMD23_XFERTYP  (uint16_t)( ACMD23 | CMD_RESP_R1   )
#define CMD24_XFERTYP   (uint16_t)( CMD24  | CMD_RESP_R1   )
#define CMD25_XFERTYP   (uint16_t)( CMD25  | CMD_RESP_R1   )
#define CMD32_XFERTYP   (uint16_t)( CMD32  | CMD_RESP_R1   )
#define CMD33_XFERTYP   (uint16_t)( CMD32  | CMD_RESP_R1   )
#define CMD38_XFERTYP   (uint16_t)( CMD38  | CMD_RESP_R1b  )
#define ACMD41_XFERTYP  (uint16_t)( ACMD41 | CMD_RESP_R3   )

#define CMD55_XFERTYP   (uint16_t)( CMD55  | CMD_RESP_R1   )

//=============================================================================
//static void enableGPIO(bool enable);
//static void enableDmaIrs();
static void initSDHC(void);
static bool isBusyCMD13(void);
static bool isBusyTransferComplete(void);
//static bool isBusyCommandComplete();
//static bool isBusyCommandInhibit();
static bool readReg16(uint32_t xfertyp, void* data);
//static void setSdclk(uint32_t kHzMax);
static bool yieldTimeout(bool (*fcn)(void));
static bool waitDmaStatus(void);
static bool waitTimeout(bool (*fcn)(void));
//-----------------------------------------------------------------------------
#define TRX_RD 0
#define TRX_WR 1
static uint8_t m_dir = TRX_RD;
static bool (*m_busyFcn)() = 0;
static bool m_initDone = false;
static uint32_t m_lba; // for raw non-DMA read(write)Start, read(write)Data, read(write)Stop
static uint32_t m_cnt; // for raw non-DMA read(write)Start, read(write)Data, read(write)Stop
static bool m_version2;
static bool m_highCapacity;
static uint8_t m_errorCode = SD_CARD_ERROR_INIT_NOT_CALLED;
static uint32_t m_errorLine = 0;
static uint32_t m_rca;
//static volatile bool m_dmaBusy = false;
static volatile uint32_t m_irqstat;
static uint32_t m_sdClkKhz = 0;
static uint32_t m_ocr;
static cid_t m_cid;
static csd_t m_csd;
static uint32_t t = 0;
//=============================================================================
#if USE_DEBUG_MODE
#define DBG_PRINT() { \
	Serial.write('_'); Serial.print(__FUNCTION__); Serial.write('_'); Serial.print(__LINE__); Serial.print(": "); \
	Serial.print("DMA->LISR: "); Serial.print(DMA2->regs->LISR, HEX); \
	/*Serial.print("DMA->HISR: "); Serial.println(DMA2->regs->HISR, HEX);*/ \
	Serial.print(", DMA->CR: "); Serial.print(DMA2->regs->STREAM[DMA_STREAM3].CR, HEX); \
	Serial.print(", DMA->NDTR: "); Serial.print(DMA2->regs->STREAM[DMA_STREAM3].NDTR, HEX); \
	/**/Serial.print(", DMA->PAR: "); Serial.print(DMA2->regs->STREAM[DMA_STREAM3].PAR, HEX); \
	/**/Serial.print(", DMA->M0AR: "); Serial.print(DMA2->regs->STREAM[DMA_STREAM3].M0AR, HEX); \
	Serial.print(", DMA->FCR: "); Serial.print(DMA2->regs->STREAM[DMA_STREAM3].FCR, HEX); \
 \
	/*Serial.print(" SDIO->POWER: "); Serial.println(SDIO->POWER, HEX);*/ \
	Serial.print(", SDIO->CLKCR: "); Serial.print(SDIO->CLKCR, HEX); \
	Serial.print(", SDIO->DTIMER: "); Serial.print(SDIO->DTIMER, HEX); \
	Serial.print(", SDIO->DCTRL: "); Serial.print(SDIO->DCTRL, HEX); \
	/**/Serial.print(", SDIO->DLEN: "); Serial.print(SDIO->DLEN); \
	Serial.print(", SDIO->DCOUNT: "); Serial.print(SDIO->DCOUNT); \
	Serial.print(", SDIO->STA: "); Serial.println(SDIO->STA, HEX); \
	/*delay(1);*/ \
}
#define DBG_PIN PD0

#else  // USE_DEBUG_MODE
#define DBG_PRINT()
#endif  // USE_DEBUG_MODE

/*****************************************************************************/
static void _panic(const char *message, uint32_t code)
{
	Serial.print(message); Serial.println(code, HEX);
	//Block the execution with blinky leds
	pinMode(BOARD_LED_PIN, OUTPUT);
	pinMode(BOARD_LED2_PIN, OUTPUT);
	while (1) {
		digitalWrite(BOARD_LED_PIN, HIGH);
		digitalWrite(BOARD_LED2_PIN, LOW);
		delay(250);
		digitalWrite(BOARD_LED_PIN, LOW);
		digitalWrite(BOARD_LED2_PIN, HIGH);
		delay(250);
	}
}
/*===========================================================================*/
#if USE_YIELD
void yield(void)
{
	uint32_t val = SDIO->STA;
	if ( val & SDIO_STA_TRX_ERROR_FLAGS ) {
		Serial.print("SDIO ERROR:: SDIO->CLKCR: "); Serial.print(SDIO->CLKCR, HEX); \
		Serial.print(", SDIO->DTIMER: "); Serial.print(SDIO->DTIMER, HEX); \
		Serial.print(", SDIO->DCTRL: "); Serial.print(SDIO->DCTRL, HEX); \
		Serial.print(", SDIO->DLEN: "); Serial.print(SDIO->DLEN); \
		Serial.print(", SDIO->DCOUNT: "); Serial.print(SDIO->DCOUNT); \
		Serial.print(", SDIO->STA: "); Serial.print(SDIO->STA, HEX); \
		Serial.print(", SDIO->RESP0: "); Serial.println(SDIO->RESP[0], HEX); \
		if (val & SDIO_STA_STBITERR) Serial.print(" STBITERR");
		if (val & SDIO_STA_RXOVERR) Serial.print(" RXOVERR");
		if (val & SDIO_STA_TXUNDERR) Serial.print(" TXUNDERR");
		if (val & SDIO_STA_DTIMEOUT) Serial.print(" DTIMEOUT");
		if (val & SDIO_STA_DCRCFAIL) Serial.print(" DCRCFAIL");
		_panic(" - SDIO: Data Transmission Error ", val);
	}

	val = dma_get_isr_bit(DMA2, DMA_STREAM3, DMA_ISR_ERROR_BITS);
	if ( val & DMA_ISR_FEIF ) {
		val ^= DMA_ISR_FEIF;
		dma_clear_isr_bit(DMA2, DMA_STREAM3, DMA_ISR_FEIF);
	}
	if ( val ) {
		if (val & DMA_ISR_TEIF) Serial.print(" TEIF");
		if (val & DMA_ISR_DMEIF) Serial.print(" DMEIF");
		//if (val & DMA_ISR_FEIF) Serial.print(" FEIF");
		_panic(" - DMA: Data Transmission Error ", val);
	}
	//Serial.write('.');
}
#endif
//=============================================================================
// Error function and macro.
inline bool setSdErrorCode(uint8_t code, uint32_t line) {
  m_errorCode = code;
  m_errorLine = line;
  return false;  // setSdErrorCode
}
#define sdError(code) setSdErrorCode(code, __LINE__)
//=============================================================================
/* ISR
void sdhc_isr() {
  SDHC_IRQSIGEN = 0;
  m_irqstat = SDHC_IRQSTAT;
  SDHC_IRQSTAT = m_irqstat;
  m_dmaBusy = false;
}*/
//=============================================================================
// Static functions.
//-----------------------------------------------------------------------------
static bool cardCommand(uint16_t xfertyp, uint32_t arg)
{
#if USE_DEBUG_MODE==2
	Serial.print("cardCommand: "); Serial.print(xfertyp&SDIO_CMD_CMDINDEX); Serial.print(", arg: "); Serial.print(arg, HEX);
#endif
	uint8_t resp = sdio_cmd_send(xfertyp, arg); // returns non-zero if fails, zero if OK
#if USE_DEBUG_MODE==2
	Serial.print(", resp: "); Serial.print(resp, HEX);
	Serial.print(", SDIO->STA: "); Serial.print(SDIO->STA, HEX); Serial.print(", cmd_resp: "); Serial.print(SDIO->RESP[0], HEX);
	if ( (xfertyp&SDIO_CMD_WAIT_LONG_RESP)==SDIO_CMD_WAIT_LONG_RESP ) {
		Serial.print(", "); Serial.print(SDIO->RESP[1], HEX); Serial.print(", "); Serial.print(SDIO->RESP[2], HEX); Serial.print(", "); Serial.println(SDIO->RESP[3], HEX);
	} else Serial.println();
#endif
	return resp; // return non-zero when OK
}
//-----------------------------------------------------------------------------
static bool cardAcmd(uint32_t rca, uint32_t xfertyp, uint32_t arg) {
  return cardCommand(CMD55_XFERTYP, rca) && cardCommand((uint16_t)xfertyp, arg);
}
/*---------------------------------------------------------------------------*/
static bool cardCMD6(uint32_t arg, uint8_t* status) {
  // CMD6 returns 64 bytes.
  // use polling only for the moment
	if (waitTimeout(isBusyCMD13)) {
		return sdError(SD_CARD_ERROR_CMD13);
	}
	// get the 64 bytes over data lines
	sdio_setup_transfer(80000, 64, (SDIO_BLOCKSIZE_64 | SDIO_DIR_RX | SDIO_DCTRL_DTEN));

	cardCommand(CMD6_XFERTYP, arg);

	// wait data Rx response
	if (waitTimeout(isBusyTransferComplete)) {
		return sdError(SD_CARD_ERROR_CMD6);
	}
DBG_PRINT();
	// copy 64 bytes as 16 words from FIFO to buffer
	for (uint8_t i=0; i<16; i++) {
		*(uint32_t*)(&status[i<<2]) = SDIO->FIFO;
	}
	//SDIO->DCTRL = 0; // disable data controller

#if USE_DEBUG_MODE
	Serial.print("read data: "); for (uint8_t i=0; i<17; i++) { Serial.print(status[i], HEX); Serial.print(", "); } Serial.println();
#endif
return true;
}
//-----------------------------------------------------------------------------
static void initSDHC(void)
{
	sdio_begin();
	DBG_PRINT();
}
/*---------------------------------------------------------------------------*/
static bool isBusyCMD13(void) {
  if (!cardCommand(CMD13_XFERTYP, m_rca)) { // SEND_STATUS
    // Caller will timeout.
    return true;
  }
  return !(SDIO->RESP[0] & CARD_STATUS_READY_FOR_DATA);
}
/*---------------------------------------------------------------------------*/
static bool isBusyDMA(void)
{
	uint8_t isr = dma_get_isr_bit(DMA2, DMA_STREAM3, (DMA_ISR_TCIF | DMA_ISR_ERROR_BITS));
	//if (isr&DMA_ISR_TCIF) dma_disable(DMA2, DMA_STREAM3);
	return !(isr^DMA_ISR_FEIF); // ignore transfer error flag
}
/*---------------------------------------------------------------------------*/
static bool isBusyTransferComplete(void)
{
	uint32_t mask = SDIO->STA &(SDIO_STA_DATAEND | SDIO_STA_TRX_ERROR_FLAGS);
#if USE_DEBUG_MODE
	if ( mask & SDIO_STA_TRX_ERROR_FLAGS ) {
		Serial.print("XFER ERROR: SDIO->STA: "); Serial.print(SDIO->STA, HEX);
		if (mask & SDIO_STA_STBITERR) Serial.print(" STBITERR");
		if (mask & SDIO_STA_RXOVERR)  Serial.print(" RXOVERR");
		if (mask & SDIO_STA_TXUNDERR) Serial.print(" TXUNDERR");
		if (mask & SDIO_STA_DTIMEOUT) Serial.print(" DTIMEOUT");
		if (mask & SDIO_STA_DCRCFAIL) Serial.print(" DCRCFAIL");
		Serial.println();
	}
#endif
	if (mask) {
		dma_disable(DMA2, DMA_STREAM3);
		return false;
	}
	return true;
}
/*---------------------------------------------------------------------------*/
static void trxStart(uint8_t* buf, uint32_t n, uint8_t dir)
{
	m_dir = dir;
	uint32_t flags = (SDIO_BLOCKSIZE_512 | SDIO_DCTRL_DTEN);
	if (dir==TRX_RD) flags |= SDIO_DIR_RX;
	// setup SDIO to transfer n blocks of 512 bytes
	sdio_setup_transfer(0x00FFFFFF, n, flags);
}
/*---------------------------------------------------------------------------*/
static bool trxStop()
{
	if (!cardCommand(CMD12_XFERTYP, 0)) {
		return sdError(SD_CARD_ERROR_CMD12);
	}
	if ( t ) {
		Serial.print(", in "); Serial.println(millis()-t);
		t = 0;
	}
	return true;
}
/*---------------------------------------------------------------------------*/
static bool dmaTrxStart(uint8_t* buf, uint32_t n, uint8_t dir)
{
	m_dir = dir;
	if ((3 & (uint32_t)buf) || n == 0) { // check alignment
		_panic("- transferStart: unaligned buffer address ", (uint32_t)buf);
		return sdError(SD_CARD_ERROR_DMA);
	}
	if (dir==TRX_RD && yieldTimeout(isBusyCMD13)) {
		return sdError(SD_CARD_ERROR_CMD13);
	}
	uint32_t flags = (SDIO_BLOCKSIZE_512 | SDIO_DCTRL_DMAEN | SDIO_DCTRL_DTEN);
	if (dir==TRX_RD) flags |= SDIO_DIR_RX;
	// setup SDIO to transfer n blocks of 512 bytes
	sdio_setup_transfer(0x00FFFFFF, n, flags);
	// setup DMA2 stream 3 channel 4
	dma_init(DMA2);
	flags = (DMA_PERIF_CTRL | DMA_BURST_INCR4 | DMA_MINC_MODE | DMA_PRIO_VERY_HIGH);
	if (dir==TRX_RD) flags |= DMA_FROM_PER; // read
	else             flags |= DMA_FROM_MEM; // write
	dma_setup_transfer(DMA2, DMA_STREAM3, DMA_CH4, DMA_SIZE_32BITS, &SDIO->FIFO, buf, NULL, flags);
	dma_set_num_transfers(DMA2, DMA_STREAM3, n);
	dma_set_fifo_flags(DMA2, DMA_STREAM3, (DMA_FCR_DMDIS | DMA_FCR_FTH_FULL)); // disable direct mode | threshold FULL
	dma_clear_isr_bits(DMA2, DMA_STREAM3);
	dma_enable(DMA2, DMA_STREAM3);
	return true;
}
/*---------------------------------------------------------------------------*/
static bool dmaTrxEnd(bool multi_block)
{
	if ( !waitDmaStatus() ) {
		DBG_PRINT();
		return sdError(SD_CARD_ERROR_DMA);
	}
	if ( waitTimeout(isBusyTransferComplete) ) {
		if (m_dir==TRX_RD)
			return sdError(SD_CARD_ERROR_READ_TIMEOUT);
		else
			return sdError(SD_CARD_ERROR_WRITE_TIMEOUT);
	}
	if (multi_block) {
		return trxStop();
	} else {
		if ( t ) {
			Serial.print(", in "); Serial.println(millis()-t);
			t = 0;
		}
		return true;
	}
}
//-----------------------------------------------------------------------------
// Read 16 byte CID or CSD register.
static bool readReg16(uint32_t xfertyp, void* data)
{
  uint8_t* d = reinterpret_cast<uint8_t*>(data);
  if (!cardCommand(xfertyp, m_rca)) {
    return false;  // Caller will set errorCode.
  }
  *(uint32_t*)(&d[0])  = __builtin_bswap32(SDIO->RESP[0]);
  *(uint32_t*)(&d[4])  = __builtin_bswap32(SDIO->RESP[1]);
  *(uint32_t*)(&d[8])  = __builtin_bswap32(SDIO->RESP[2]);
  *(uint32_t*)(&d[12]) = __builtin_bswap32(SDIO->RESP[3]);
  d[15] = 0;
  return true;
}
/*---------------------------------------------------------------------------*/
// Return true if timeout occurs.
static bool yieldTimeout(bool (*fcn)()) {
  uint32_t m = millis();
  while (fcn()) {
    if ((millis() - m) > BUSY_TIMEOUT_MILLIS) {
      return true;
    }
    yield();
  }
  return false;  // Caller will set errorCode.
}
/*---------------------------------------------------------------------------*/
static bool waitDmaStatus(void)
{
  if (yieldTimeout(isBusyDMA)) {
    return false;  // Caller will set errorCode.
  }
  return true;
}
/*---------------------------------------------------------------------------*/
// Return true if timeout occurs.
static bool waitTimeout(bool (*fcn)(void)) {
  uint32_t m = millis();
  while (fcn()) {
    if ((millis() - m) > BUSY_TIMEOUT_MILLIS) {
      return true;
    }
	delayMicroseconds(1);
  }
  return false;  // Caller will set errorCode.
}

uint32_t aligned[128]; // temporary buffer for misaligned buffers
//=============================================================================
bool SdioCard::begin(void)
{
  m_initDone = false;
  m_errorCode = SD_CARD_ERROR_NONE;
  m_highCapacity = false;
  m_version2 = false;

#if USE_DEBUG_MODE
pinMode(DBG_PIN, OUTPUT);
digitalWrite(DBG_PIN, HIGH);
delay(100);
#endif
  // initialize controller.
  initSDHC();

  if (!cardCommand(CMD0_XFERTYP, 0)) {
    return sdError(SD_CARD_ERROR_CMD0);
  }
  // Try several times for case of reset delay.
  for (uint32_t i = 0; i < CMD8_RETRIES; i++) {
    if (cardCommand(CMD8_XFERTYP, 0X1AA)) {
      if (SDIO->RESP[0] != 0X1AA) {
        return sdError(SD_CARD_ERROR_CMD8);
      }
      m_version2 = true;
      break;
    }
  }
  uint32_t arg = m_version2 ? 0X40300000 : 0x00300000;
  uint32_t m = millis();
  do {
    if (!cardAcmd(0, ACMD41_XFERTYP, arg) ||
       ((millis() - m) > BUSY_TIMEOUT_MILLIS)) {
      return sdError(SD_CARD_ERROR_ACMD41);
    }
  } while ((SDIO->RESP[0] & 0x80000000) == 0);

  m_ocr = SDIO->RESP[0];
  if (m_ocr & 0x40000000) {
    // Is high capacity.
    m_highCapacity = true;
  }
  if (!cardCommand(CMD2_XFERTYP, 0)) {
    return sdError(SD_CARD_ERROR_CMD2);
  }
  if (!cardCommand(CMD3_XFERTYP, 0)) {
    return sdError(SD_CARD_ERROR_CMD3);
  }
  m_rca = SDIO->RESP[0] & 0xFFFF0000;
  if (!readReg16(CMD9_XFERTYP, &m_csd)) {
    return sdError(SD_CARD_ERROR_CMD9);
  }
  if (!readReg16(CMD10_XFERTYP, &m_cid)) {
    return sdError(SD_CARD_ERROR_CMD10);
  }
  if (!cardCommand(CMD7_XFERTYP, m_rca)) {
    return sdError(SD_CARD_ERROR_CMD7);
  }
  // Set card to bus width four.
  if (!cardAcmd(m_rca, ACMD6_XFERTYP, 2)) {
    return sdError(SD_CARD_ERROR_ACMD6);
  }
  sdio_set_dbus_width(SDIO_CLKCR_WIDBUS_4BIT);

  // Determine if High Speed mode is supported and set frequency.
  uint8_t status[64];
  // see "Physical Layer Simplified Specification Version 6.00", chapter 4.3.10, Table 4-13.
  // Support Bits of Functions in Function Group 1: bits 415:400, which are bytes [12][13]
  // Function Selection of Function Group 1: bits 379:376, which is low nibble of byte [16]
  if (cardCMD6(0X00FFFFFF, status) && (2 & status[13]) &&
      cardCMD6(0X80FFFFF1, status) && (status[16] & 0XF) == 1) {
	//Serial.println("\n*** 50MHz clock supported ***");
  } else {
	_panic("*** Only 25MHz clock supported! ***", 0);
  }

  m_sdClkKhz = 24000;
  sdio_set_clock(m_sdClkKhz*1000); // set clock to 24MHz

  m_initDone = true;
  return true;
}
//-----------------------------------------------------------------------------
uint32_t SdioCard::cardSize(void) {
  return sdCardCapacity(&m_csd);
}
/*---------------------------------------------------------------------------*/
bool SdioCard::erase(uint32_t firstBlock, uint32_t lastBlock) {
  // check for single block erase
  if (!m_csd.v1.erase_blk_en) {
    // erase size mask
    uint8_t m = (m_csd.v1.sector_size_high << 1) | m_csd.v1.sector_size_low;
    if ((firstBlock & m) != 0 || ((lastBlock + 1) & m) != 0) {
      // error card can't erase specified area
      return sdError(SD_CARD_ERROR_ERASE_SINGLE_BLOCK);
    }
  }
  if (!m_highCapacity) {
    firstBlock <<= 9;
    lastBlock <<= 9;
  }
  if (!cardCommand(CMD32_XFERTYP, firstBlock)) {
    return sdError(SD_CARD_ERROR_CMD32);
  }
  if (!cardCommand(CMD33_XFERTYP, lastBlock)) {
     return sdError(SD_CARD_ERROR_CMD33);
  }
  if (!cardCommand(CMD38_XFERTYP, 0)) {
    return sdError(SD_CARD_ERROR_CMD38);
  }
  if (waitTimeout(isBusyCMD13)) {
    return sdError(SD_CARD_ERROR_ERASE_TIMEOUT);
  }
  return true;
}
//-----------------------------------------------------------------------------
uint8_t SdioCard::errorCode() {
  return m_errorCode;
}
//-----------------------------------------------------------------------------
uint32_t SdioCard::errorData() {
  return m_irqstat;
}
//-----------------------------------------------------------------------------
uint32_t SdioCard::errorLine() {
  return m_errorLine;
}
//-----------------------------------------------------------------------------
bool SdioCard::isBusy() {
  return m_busyFcn ? m_busyFcn() : m_initDone && isBusyCMD13();
}
//-----------------------------------------------------------------------------
uint32_t SdioCard::kHzSdClk() {
  return m_sdClkKhz;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::readBlock(uint32_t lba, uint8_t* buf)
{
#if USE_DEBUG_MODE
	Serial.print("readBlock: ");  Serial.println(lba); //Serial.print(", buf: "); Serial.println((uint32_t)buf, HEX);
#endif
	// prepare SDIO and DMA for data read transfer
	dmaTrxStart((uint32_t)buf & 3 ? (uint8_t*)aligned : buf, 512, TRX_RD);
	// send command to start data transfer
	if ( !cardCommand(CMD17_XFERTYP, (m_highCapacity ? lba : 512*lba)) ) {
		return sdError(SD_CARD_ERROR_CMD17);
	}
	if ( dmaTrxEnd(0)) {
		if ( (uint32_t)buf & 3 ) {
			//memcpy(buf, aligned, 512);
			register uint8_t * dst = buf;
			register uint8_t * src = (uint8_t *)aligned;
			register uint16_t i = 64;
			while ( i-- ) { // do 8 byte copies, is much faster than single byte copy
				*dst++ = *src++; *dst++ = *src++; *dst++ = *src++; *dst++ = *src++;
				*dst++ = *src++; *dst++ = *src++; *dst++ = *src++; *dst++ = *src++;
			}
		}
		return true;
	}
	return false;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::readBlocks(uint32_t lba, uint8_t* buf, size_t n)
{
#if USE_DEBUG_MODE
	Serial.print("readBlocks: ");  Serial.print(lba);
	//Serial.print(", buf: "); Serial.print((uint32_t)buf, HEX);
	Serial.print(", "); Serial.println(n);
#endif
	if ((uint32_t)buf & 3) {
		for (size_t i = 0; i < n; i++, lba++, buf += 512) {
			if (!readBlock(lba, buf)) {
				return false;  // readBlock will set errorCode.
			}
		}
		return true;
	}
	// prepare SDIO and DMA for data read transfer
	dmaTrxStart(buf, 512*n, TRX_RD);
	// send command to start data transfer
	if ( !cardCommand(CMD18_XFERTYP, (m_highCapacity ? lba : 512*lba)) ) {
		return sdError(SD_CARD_ERROR_CMD18);
	}
	return dmaTrxEnd(1);
}
//-----------------------------------------------------------------------------
bool SdioCard::readCID(void* cid) {
  memcpy(cid, &m_cid, 16);
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCard::readCSD(void* csd) {
  memcpy(csd, &m_csd, 16);
  return true;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::readData(uint8_t *dst)
{
	//Serial.print("readData: "); Serial.print(m_lba); Serial.print(", m_cnt: "); Serial.println(m_cnt);
	if ( m_cnt==0 ) return false;
	if (yieldTimeout(isBusyCMD13)) { // wait for previous transmission end
		return sdError(SD_CARD_ERROR_CMD13);
	}
	// non-DMA block read
	trxStart(dst, 512, TRX_RD);
	// send command to start data transfer
	if ( !cardCommand(CMD17_XFERTYP, (m_highCapacity ? m_lba : 512*m_lba)) ) {
		return sdError(SD_CARD_ERROR_CMD17);
	}
	// Receive a data block from the SDIO
	register uint32_t STA; // to speed up SDIO flags checking
	register uint16_t cnt = 512;
	register uint32_t * ptr = (uint32_t *)dst;
	// ----> TIME CRITICAL SECTION BEGIN <----
	do {
		STA = SDIO->STA;
		if (STA & SDIO_STA_RXFIFOHF) {
			// Receive FIFO half full, there are at least 8 words in it
			noInterrupts();
			*ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO;
			*ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO; *ptr++ = SDIO->FIFO;
			interrupts();
			cnt -= 8;
		}
	} while ( !(STA & (SDIO_STA_DATAEND | SDIO_STA_TRX_ERROR_FLAGS)) );
	// <---- TIME CRITICAL SECTION END ---->

	// read data still available in FIFO
	while ( (SDIO->STA & SDIO_STA_RXDAVL) && (cnt--) ) {
		*ptr++ = SDIO->FIFO;
	}
	// check error, temporary stuff, remove for final version
	if ( SDIO->STA & SDIO_STA_TRX_ERROR_FLAGS ) {
		_panic("ERROR: non-DMA read error ", SDIO->STA);
		return false;
	}
	m_lba++;
	m_cnt--;
	return !(SDIO->STA&SDIO_STA_TRX_ERROR_FLAGS);
}
//-----------------------------------------------------------------------------
bool SdioCard::readOCR(uint32_t* ocr) {
  *ocr = m_ocr;
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCard::readStart(uint32_t lba)
{
  m_lba = lba;
  m_cnt = 1024;
  return true;
}
/*---------------------------------------------------------------------------*/
// SDHC will do Auto CMD12 after count blocks.
bool SdioCard::readStart(uint32_t lba, uint32_t count)
{
	//Serial.print("readStart: "); Serial.print(lba); Serial.print(", cnt: "); Serial.println(count);
  m_lba = lba;
  m_cnt = count;
  return true;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::readStop()
{
	//Serial.println("readStop.");
  m_lba = 0;
  m_cnt = 0;
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCard::syncBlocks() {
  return true;
}
//-----------------------------------------------------------------------------
uint8_t SdioCard::type() {
  return  m_version2 ? m_highCapacity ?
          SD_CARD_TYPE_SDHC : SD_CARD_TYPE_SD2 : SD_CARD_TYPE_SD1;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::writeBlock(uint32_t lba, const uint8_t* buf)
{
#if USE_DEBUG_MODE
	Serial.print("writeBlock: ");  Serial.println(lba); //Serial.print(", buf: "); Serial.println((uint32_t)buf, HEX);
#endif
	uint8_t * ptr = (uint8_t *)buf;
	if (3 & (uint32_t)ptr) {
		Serial.print("writeBlock: "); Serial.print(lba);
		Serial.print(", buf: "); Serial.print((uint32_t)ptr, HEX);
		//memcpy(aligned, buf, 512);
		register uint8_t * src = (uint8_t *)ptr;
		ptr = (uint8_t *)aligned;
		register uint8_t * dst = ptr;
		register uint16_t i = 64;
		while ( i-- ) { // do 8 byte copies, is much faster than single byte copy
			*dst++ = *src++; *dst++ = *src++; *dst++ = *src++; *dst++ = *src++;
			*dst++ = *src++; *dst++ = *src++; *dst++ = *src++; *dst++ = *src++;
		}
	}
	if (yieldTimeout(isBusyCMD13)) { // wait for previous transmission end
		return sdError(SD_CARD_ERROR_CMD13);
	}
	// send command to start data transfer
	if ( !cardCommand(CMD24_XFERTYP, (m_highCapacity ? lba : 512*lba)) ) {
		return sdError(SD_CARD_ERROR_CMD24);
	}
	// prepare SDIO and DMA for data transfer
	dmaTrxStart(ptr, 512, TRX_WR); // 1 block, write transfer

	return dmaTrxEnd(0);
}
/*---------------------------------------------------------------------------*/
bool SdioCard::writeBlocks(uint32_t lba, const uint8_t* buf, size_t n)
{
#if USE_DEBUG_MODE
	Serial.print("writeBlocks: ");  Serial.print(lba);
	//Serial.print(", buf: "); Serial.print((uint32_t)buf, HEX);
	Serial.print(", "); Serial.println(n);
#endif
	if (3 & (uint32_t)buf) { // misaligned buffer address, write single blocks
		for (size_t i = 0; i < n; i++, lba++, buf += 512) {
			if (!writeBlock(lba, buf)) {
				return false;  // writeBlock will set errorCode.
			}
		}
		return true;
	}
	if (yieldTimeout(isBusyCMD13)) {
		return sdError(SD_CARD_ERROR_CMD13);
	}
#if 0
	// set number of blocks to write - this can speed up block write
	if ( !cardAcmd(m_rca, ACMD23_XFERTYP, n) ) {
		return sdError(SD_CARD_ERROR_ACMD23);
	}
#endif
	// send command to start data transfer
	if ( !cardCommand(CMD25_XFERTYP, (m_highCapacity ? lba : 512*lba)) ) {
		return sdError(SD_CARD_ERROR_CMD25);
	}
	// prepare SDIO and DMA for data transfer
	dmaTrxStart((uint8_t *)buf, 512*n, TRX_WR); // n blocks, write transfer

	return dmaTrxEnd(1);
}
/*---------------------------------------------------------------------------*/
bool SdioCard::writeData(const uint8_t* src)
{
	//Serial.print("writeData: "); Serial.print(m_lba); Serial.print(", cnt: "); Serial.println(m_cnt);
	if ( !m_cnt ) return false;
	if (yieldTimeout(isBusyCMD13)) { // wait for previous transmission end
		return sdError(SD_CARD_ERROR_CMD13);
	}
	// send command to start block data transfer
	if ( !cardCommand(CMD24_XFERTYP, (m_highCapacity ? m_lba : 512*m_lba)) ) {
		return sdError(SD_CARD_ERROR_CMD24);
	}
	// non-DMA block write
	trxStart((uint8_t*)src, 512, TRX_WR);
	// Receive a data block from the SDIO
	register uint32_t STA; // to speed up SDIO flags checking
	register uint16_t cnt = 512;
	register uint32_t * ptr = (uint32_t*)src;
	// pre-fill up the FIFO with 32 words
	noInterrupts();
	while ( (cnt--)>(512-32) ) SDIO->FIFO = *ptr++;
	interrupts();
	// ----> TIME CRITICAL SECTION BEGIN <----
	do {
		STA = SDIO->STA;
		if (STA & SDIO_STA_TXFIFOHE) {
			// Transmit FIFO half empty, fill up the remaining 16 words
			noInterrupts();
			SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++;
			SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++;
			SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++;
			SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++; SDIO->FIFO = *ptr++;
			interrupts();
			cnt -= 16;
		}
	} while ( !(STA & (SDIO_STA_DATAEND | SDIO_STA_TRX_ERROR_FLAGS)) && cnt);
	// <---- TIME CRITICAL SECTION END ---->
	if ( waitTimeout(isBusyTransferComplete) ) {
		return sdError(SD_CARD_ERROR_WRITE_TIMEOUT);
	}
	m_lba++;
	m_cnt--;
	if (SDIO->STA&SDIO_STA_TRX_ERROR_FLAGS) {
		_panic("writeData error: ", SDIO->STA);
	}
	return !(SDIO->STA&SDIO_STA_TRX_ERROR_FLAGS);
}
//-----------------------------------------------------------------------------
bool SdioCard::writeStart(uint32_t lba)
{
  m_lba = lba;
  m_cnt = 1024;
  return true;
}
/*---------------------------------------------------------------------------*/
// SDHC will do Auto CMD12 after count blocks.
bool SdioCard::writeStart(uint32_t lba, uint32_t count)
{
	//Serial.print("writeStart: "); Serial.print(lba); Serial.print(", cnt: "); Serial.println(count);
  m_lba = lba;
  m_cnt = count;
  return true;
}
/*---------------------------------------------------------------------------*/
bool SdioCard::writeStop()
{
	//Serial.println("writeStop.");
  m_lba = 0;
  m_cnt = 0;
  return true;
}
