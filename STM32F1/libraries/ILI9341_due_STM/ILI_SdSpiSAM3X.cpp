/* Arduino SdSpi Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino SdSpi Library
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
 * along with the Arduino SdSpi Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "ILI_SdSpi.h"
#if ILI_USE_NATIVE_SAM3X_SPI
/** Use SAM3X DMAC if nonzero */
#define ILI_USE_SAM3X_DMAC 1
/** Use extra Bus Matrix arbitration fix if nonzero */
#define ILI_USE_SAM3X_BUS_MATRIX_FIX 0
/** Time in ms for DMA receive timeout */
#define ILI_SAM3X_DMA_TIMEOUT 100
/** chip select register number */
#define ILI_SPI_CHIP_SEL 3
/** DMAC receive channel */
#define ILI_SPI_DMAC_RX_CH  1
/** DMAC transmit channel */
#define ILI_SPI_DMAC_TX_CH  0
/** DMAC Channel HW Interface Number for SPI TX. */
#define ILI_SPI_TX_IDX  1
/** DMAC Channel HW Interface Number for SPI RX. */
#define ILI_SPI_RX_IDX  2
//------------------------------------------------------------------------------
/** Disable DMA Controller. */
static void ILI_dmac_disable() {
  DMAC->DMAC_EN &= (~DMAC_EN_ENABLE);
}
/** Enable DMA Controller. */
static void ILI_dmac_enable() {
  DMAC->DMAC_EN = DMAC_EN_ENABLE;
}
/** Disable DMA Channel. */
static void ILI_dmac_channel_disable(uint32_t ul_num) {
  DMAC->DMAC_CHDR = DMAC_CHDR_DIS0 << ul_num;
}
/** Enable DMA Channel. */
static void ILI_dmac_channel_enable(uint32_t ul_num) {
  DMAC->DMAC_CHER = DMAC_CHER_ENA0 << ul_num;
}
/** Poll for transfer complete. */
static bool ILI_dmac_channel_transfer_done(uint32_t ul_num) {
  return (DMAC->DMAC_CHSR & (DMAC_CHSR_ENA0 << ul_num)) ? false : true;
}
//------------------------------------------------------------------------------
void ILI_SdSpi::begin() {
  PIO_Configure(
    g_APinDescription[PIN_SPI_MOSI].pPort,
    g_APinDescription[PIN_SPI_MOSI].ulPinType,
    g_APinDescription[PIN_SPI_MOSI].ulPin,
    g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PIN_SPI_MISO].pPort,
    g_APinDescription[PIN_SPI_MISO].ulPinType,
    g_APinDescription[PIN_SPI_MISO].ulPin,
    g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PIN_SPI_SCK].pPort,
    g_APinDescription[PIN_SPI_SCK].ulPinType,
    g_APinDescription[PIN_SPI_SCK].ulPin,
    g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
  pmc_enable_periph_clk(ID_SPI0);
#if ILI_USE_SAM3X_DMAC
  pmc_enable_periph_clk(ID_DMAC);
  ILI_dmac_disable();
  DMAC->DMAC_GCFG = DMAC_GCFG_ARB_CFG_FIXED;
  ILI_dmac_enable();
#if ILI_USE_SAM3X_BUS_MATRIX_FIX
  MATRIX->MATRIX_WPMR = 0x4d415400;
  MATRIX->MATRIX_MCFG[1] = 1;
  MATRIX->MATRIX_MCFG[2] = 1;
  MATRIX->MATRIX_SCFG[0] = 0x01000010;
  MATRIX->MATRIX_SCFG[1] = 0x01000010;
  MATRIX->MATRIX_SCFG[7] = 0x01000010;
#endif  // ILI_USE_SAM3X_BUS_MATRIX_FIX
#endif  // ILI_USE_SAM3X_DMAC
}
//------------------------------------------------------------------------------
// start RX DMA
static void ILI_spiDmaRX(uint8_t* dst, uint16_t count) {
  ILI_dmac_channel_disable(ILI_SPI_DMAC_RX_CH);
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_SADDR = (uint32_t)&SPI0->SPI_RDR;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_DADDR = (uint32_t)dst;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_DSCR =  0;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CTRLA = count |
      DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CTRLB = DMAC_CTRLB_SRC_DSCR |
      DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_PER2MEM_DMA_FC |
      DMAC_CTRLB_SRC_INCR_FIXED | DMAC_CTRLB_DST_INCR_INCREMENTING;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CFG = DMAC_CFG_SRC_PER(ILI_SPI_RX_IDX) |
      DMAC_CFG_SRC_H2SEL | DMAC_CFG_SOD | DMAC_CFG_FIFOCFG_ASAP_CFG;
  ILI_dmac_channel_enable(ILI_SPI_DMAC_RX_CH);
}
//------------------------------------------------------------------------------
// start TX DMA
static void ILI_spiDmaTX(const uint8_t* src, uint16_t count) {
  static uint8_t ff = 0XFF;
  uint32_t src_incr = DMAC_CTRLB_SRC_INCR_INCREMENTING;
  if (!src) {
    src = &ff;
    src_incr = DMAC_CTRLB_SRC_INCR_FIXED;
  }
  ILI_dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_SADDR = (uint32_t)src;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DADDR = (uint32_t)&SPI0->SPI_TDR;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DSCR =  0;
  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLA = count |
      DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;

  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLB =  DMAC_CTRLB_SRC_DSCR |
      DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_MEM2PER_DMA_FC |
      src_incr | DMAC_CTRLB_DST_INCR_FIXED;

  DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CFG = DMAC_CFG_DST_PER(ILI_SPI_TX_IDX) |
      DMAC_CFG_DST_H2SEL | DMAC_CFG_SOD | DMAC_CFG_FIFOCFG_ALAP_CFG;

  ILI_dmac_channel_enable(ILI_SPI_DMAC_TX_CH);
}
//------------------------------------------------------------------------------
//  initialize SPI controller
void ILI_SdSpi::init(uint8_t sckDivisor) {
  uint8_t scbr = sckDivisor;
  Spi* pSpi = SPI0;
  //  disable SPI
  pSpi->SPI_CR = SPI_CR_SPIDIS;
  // reset SPI
  pSpi->SPI_CR = SPI_CR_SWRST;
  // no mode fault detection, set master mode
  pSpi->SPI_MR = SPI_PCS(ILI_SPI_CHIP_SEL) | SPI_MR_MODFDIS | SPI_MR_MSTR;
  // mode 0, 8-bit,
  pSpi->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI_CSR_SCBR(scbr) | SPI_CSR_NCPHA;
  // enable SPI
  pSpi->SPI_CR |= SPI_CR_SPIEN;
}
//------------------------------------------------------------------------------
static inline uint8_t ILI_spiTransfer(uint8_t b) {
  Spi* pSpi = SPI0;

  pSpi->SPI_TDR = b;
  while ((pSpi->SPI_SR & SPI_SR_RDRF) == 0) {}
  b = pSpi->SPI_RDR;
  return b;
}
//------------------------------------------------------------------------------
/** SPI receive a byte */
uint8_t ILI_SdSpi::receive() {
  return ILI_spiTransfer(0XFF);
}
//------------------------------------------------------------------------------
/** SPI receive multiple bytes */
uint8_t ILI_SdSpi::receive(uint8_t* buf, size_t n) {
  Spi* pSpi = SPI0;
  int rtn = 0;
#if ILI_USE_SAM3X_DMAC
  // clear overrun error
  uint32_t s = pSpi->SPI_SR;

  ILI_spiDmaRX(buf, n);
  ILI_spiDmaTX(0, n);

  uint32_t m = millis();
  while (!ILI_dmac_channel_transfer_done(ILI_SPI_DMAC_RX_CH)) {
    if ((millis() - m) > ILI_SAM3X_DMA_TIMEOUT)  {
      ILI_dmac_channel_disable(ILI_SPI_DMAC_RX_CH);
      ILI_dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
      rtn = 2;
      break;
    }
  }
  if (pSpi->SPI_SR & SPI_SR_OVRES) rtn |= 1;
#else  // ILI_USE_SAM3X_DMAC
  for (size_t i = 0; i < n; i++) {
    pSpi->SPI_TDR = 0XFF;
    while ((pSpi->SPI_SR & SPI_SR_RDRF) == 0) {}
    buf[i] = pSpi->SPI_RDR;
  }
#endif  // ILI_USE_SAM3X_DMAC
  return rtn;
}
//------------------------------------------------------------------------------
/** SPI send a byte */
void ILI_SdSpi::send(uint8_t b) {
  ILI_spiTransfer(b);
}
//------------------------------------------------------------------------------
void ILI_SdSpi::send(const uint8_t* buf , size_t n) {
  Spi* pSpi = SPI0;
#if ILI_USE_SAM3X_DMAC
  ILI_spiDmaTX(buf, n);
  while (!ILI_dmac_channel_transfer_done(ILI_SPI_DMAC_TX_CH)) {}
#else  // #if ILI_USE_SAM3X_DMAC
  while ((pSpi->SPI_SR & SPI_SR_TXEMPTY) == 0) {}
  for (size_t i = 0; i < n; i++) {
    pSpi->SPI_TDR = buf[i];
    while ((pSpi->SPI_SR & SPI_SR_TDRE) == 0) {}
  }
#endif  // #if ILI_USE_SAM3X_DMAC
  while ((pSpi->SPI_SR & SPI_SR_TXEMPTY) == 0) {}
  // leave RDR empty
  uint8_t b = pSpi->SPI_RDR;
}
#endif  // ILI_USE_NATIVE_SAM3X_SPI

//*********************************************************
// STM32F1 section
//*********************************************************

#if ILI_USE_NATIVE_STM32F1_SPI
#include <SPI.h>
#include <libmaple/dma.h>
/** Use SAM3X DMAC if nonzero */
#define ILI_USE_STM32F1_DMAC 1
/** Time in ms for DMA receive timeout */
#define ILI_STM32F1_DMA_TIMEOUT 100
/** chip select register number */
#define ILI_SPI_CHIP_SEL 3
/** DMAC receive channel */
#define ILI_SPI_DMAC_RX_CH  DMA_CH2
/** DMAC transmit channel */
#define ILI_SPI_DMAC_TX_CH  DMA_CH3
/** DMAC Channel HW Interface Number for SPI TX. */
#define ILI_SPI_TX_IDX  1
/** DMAC Channel HW Interface Number for SPI RX. */
#define ILI_SPI_RX_IDX  2

volatile bool SPI_DMA_TX_Active = false;
volatile bool SPI_DMA_RX_Active = false;

inline void SPI_DMA_TX_Event() {
  SPI_DMA_TX_Active = false;
  dma_disable(DMA1, DMA_CH3);
}

inline void SPI_DMA_RX_Event() {
  SPI_DMA_RX_Active = false;
  dma_disable(DMA1, DMA_CH2);
}
//------------------------------------------------------------------------------
/** Disable DMA Controller. */
//static void ILI_dmac_disable() {
//  DMAC->DMAC_EN &= (~DMAC_EN_ENABLE);
//}
/** Enable DMA Controller. */
//static void ILI_dmac_enable() {
//  DMAC->DMAC_EN = DMAC_EN_ENABLE;
//}
/** Disable DMA Channel. */
static void ILI_dmac_channel_disable(dma_channel ul_num) {
  dma_disable(DMA1, ul_num);
}
/** Enable DMA Channel. */
static void ILI_dmac_channel_enable(dma_channel ul_num) {
  dma_enable(DMA1, ul_num);
}
/** Poll for transfer complete. */
//static bool ILI_dmac_channel_transfer_done(dma_tube tube) {
//    uint8 shift = (tube - 1) * 4;
//    return ((DMA1->regs->ISR >> shift) & 0x02) ? false : true;

//  return (DMAC->DMAC_CHSR & (DMAC_CHSR_ENA0 << ul_num)) ? false : true;
//}
//------------------------------------------------------------------------------
void ILI_SdSpi::begin() {
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  // DMA setup stuff. We use a line buffer and usa DMA for filling lines and blocks.



#if ILI_USE_STM32F1_DMAC
  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH3, SPI_DMA_TX_Event);
  dma_attach_interrupt(DMA1, DMA_CH2, SPI_DMA_RX_Event);
  spi_tx_dma_enable(SPI1);
  spi_rx_dma_enable(SPI1);


#endif  // ILI_USE_STM32F1_DMAC
}
//------------------------------------------------------------------------------
// start RX DMA

static void ILI_spiDmaRX(uint8_t* dst, uint16_t count) {
//  spi_rx_dma_enable(SPI1);
  if (count < 1) return;
  dma_setup_transfer(DMA1, DMA_CH2, &SPI1->regs->DR, DMA_SIZE_8BITS,
                     dst, DMA_SIZE_8BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));
  dma_set_num_transfers(DMA1, DMA_CH2, count); // 2 bytes per pixel
  SPI_DMA_RX_Active = true;
  dma_enable(DMA1, DMA_CH2);


}
//------------------------------------------------------------------------------
// start TX DMA
static void ILI_spiDmaTX(uint8_t* src, uint16_t count) {
  if (count < 1) return;
  static uint8_t ff = 0XFF;
//  spi_tx_dma_enable(SPI1);
  //    dma_init(DMA1);
  //    dma_attach_interrupt(DMA1, DMA_CH3, SPI_DMA_TX_Event);


  if (!src) {
    src = &ff;
    dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_8BITS,
                       src, DMA_SIZE_8BITS, (DMA_FROM_MEM | DMA_TRNS_CMPLT));
  }
  else {
    dma_setup_transfer(DMA1, DMA_CH3, &SPI1->regs->DR, DMA_SIZE_8BITS,
                       src, DMA_SIZE_8BITS, (DMA_MINC_MODE  |  DMA_FROM_MEM | DMA_TRNS_CMPLT));
  }
  dma_set_num_transfers(DMA1, DMA_CH3, count); // 2 bytes per pixel
  SPI_DMA_TX_Active = true;
  dma_enable(DMA1, DMA_CH3);

}
//------------------------------------------------------------------------------
//  initialize SPI controller STM32F1
void ILI_SdSpi::init(uint8_t sckDivisor) {
  SPI.setClockDivider(sckDivisor);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  spi_tx_dma_enable(SPI1);
  spi_rx_dma_enable(SPI1);

}
//------------------------------------------------------------------------------
// STM32
static inline uint8_t ILI_spiTransfer(uint8_t b) {
  return SPI.transfer(b);
}
//------------------------------------------------------------------------------
// should be valid for STM32
/** SPI receive a byte */
uint8_t ILI_SdSpi::receive() {
  return ILI_spiTransfer(0xFF);
}
//------------------------------------------------------------------------------
/** SPI receive multiple bytes */
// check and finish.

uint8_t ILI_SdSpi::receive(uint8_t* buf, size_t n) {
  int rtn = 0;

#if ILI_USE_STM32F1_DMAC
  // clear overrun error
  //  uint32_t s = pSpi->SPI_SR;
  ILI_spiDmaRX(buf, n);
  ILI_spiDmaTX(0, n);


  uint32_t m = millis();
  while (SPI_DMA_RX_Active) {
    if ((millis() - m) > ILI_STM32F1_DMA_TIMEOUT)  {
      ILI_dmac_channel_disable(ILI_SPI_DMAC_RX_CH);
      ILI_dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
      rtn = 2;
      break;
    }
  }
  //  if (pSpi->SPI_SR & SPI_SR_OVRES) rtn |= 1;
#else  // ILI_USE_STM32F1_DMAC
  for (size_t i = 0; i < n; i++) {
    buf[i] = SPI.transfer (0xFF);
  }
#endif  // ILI_USE_STM32F1_DMAC
  return rtn;
}
//------------------------------------------------------------------------------
/** SPI send a byte */
void ILI_SdSpi::send(uint8_t b) {
  ILI_spiTransfer(b);
}
//------------------------------------------------------------------------------
void ILI_SdSpi::send(uint8_t* buf , size_t n) {

#if ILI_USE_STM32F1_DMAC
  ILI_spiDmaTX(buf, n);
  while (SPI_DMA_TX_Active) {}

  //  uint32_t m = millis();
  //  while (SPI_DMA_TX_Active) {
  //    if ((millis() - m) > ILI_STM32F1_DMA_TIMEOUT)  {
  //      ILI_dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
  //      break;
  //    }
  //  }
#else  // #if ILI_USE_STM32F1_DMAC
  SPI.write (buf, n)
#endif  // #if ILI_USE_STM32F1_DMAC
  // leave RDR empty
  //  uint8_t b = pSpi->SPI_RDR;
}
#endif  // ILI_USE_NATIVE_STM32F1_SPI
