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
/*
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
*/
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
//static void ILI_dmac_channel_disable(dma_channel ul_num) {
//  dma_disable(DMA1, ul_num);
//}
/** Enable DMA Channel. */
//static void ILI_dmac_channel_enable(dma_channel ul_num) {
//  dma_enable(DMA1, ul_num);
//}
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
/*  dma_init(DMA1);
  dma_attach_interrupt(DMA1, DMA_CH3, SPI_DMA_TX_Event);
  dma_attach_interrupt(DMA1, DMA_CH2, SPI_DMA_RX_Event);
  spi_tx_dma_enable(SPI1);
  spi_rx_dma_enable(SPI1);
*/

#endif  // ILI_USE_STM32F1_DMAC
}
//------------------------------------------------------------------------------
// start RX DMA
/*
static void ILI_spiDmaRX(uint8_t* dst, uint16_t count) {
//  spi_rx_dma_enable(SPI1);
  if (count < 1) return;
  dma_setup_transfer(DMA1, DMA_CH2, &SPI1->regs->DR, DMA_SIZE_8BITS,
                     dst, DMA_SIZE_8BITS, (DMA_MINC_MODE | DMA_TRNS_CMPLT));
  dma_set_num_transfers(DMA1, DMA_CH2, count); // 2 bytes per pixel
  SPI_DMA_RX_Active = true;
  dma_enable(DMA1, DMA_CH2);


}
*/
//------------------------------------------------------------------------------
// start TX DMA
/*
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
*/
//------------------------------------------------------------------------------
//  initialize SPI controller STM32F1
void ILI_SdSpi::init(uint8_t sckDivisor) {
  SPI.setClockDivider(sckDivisor);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
/*  spi_tx_dma_enable(SPI1);
  spi_rx_dma_enable(SPI1);
*/
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
	SPI.DMAtransfer (0, buf, n);
  /*  spiDmaRX(buf, n);
  spiDmaTX(0, n);

  uint32_t m = millis();
  while (SPI_DMA_RX_Active) {
    if ((millis() - m) > STM32F1_DMA_TIMEOUT)  {
      dmac_channel_disable(SPI_DMAC_RX_CH);
      dmac_channel_disable(SPI_DMAC_TX_CH);
      rtn = 2;
      break;
    }
  }
*/

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
  SPI.DMAsend (buf, n);
/*
  spiDmaTX(buf, n);
  while (SPI_DMA_TX_Active) {}
*/
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
    uint8_t b = spi_rx_reg(SPI1);
}
#endif  // ILI_USE_NATIVE_STM32F1_SPI
