#ifndef _USBF4_USB_H_
#define _USBF4_USB_H_


#include <STM32_USB_Device_Library/Class/cdc/inc/usbd_cdc_core.h>
#include <STM32_USB_Device_Library/Core/inc/usbd_usr.h>
#include <VCP/usbd_desc.h>
#include "usb.h"
#include <libmaple/gpio.h>
#include <libmaple/rccF4.h>
#include <VCP/usbd_cdc_vcp.h>
#include <boards.h>

USB_OTG_CORE_HANDLE  USB_OTG_dev;

void setupUSB (void)
{
	gpio_set_mode(BOARD_USB_DP_PIN, GPIO_OUTPUT_OD); // ala42
#ifdef USB_DISC_OD
  //gpio_set_mode(USB_DISC_DEV, USB_DISC_PIN, GPIO_OUTPUT_OD); // ala42
#else
  //gpio_set_mode(USB_DISC_DEV, USB_DISC_PIN, GPIO_OUTPUT_PP); // ala42 for active pull-up on disconnect pin
#endif

  gpio_clear_pin(BOARD_USB_DP_PIN); // ala42
  delay_us(50000);

  /* initialize the usb application */
  gpio_set_pin(BOARD_USB_DP_PIN); // ala42 // presents us to the host
  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_CDC_cb,
            &USR_cb);
}

extern uint16_t VCP_DataTx (const uint8_t* Buf, uint32_t Len);
extern uint8_t  VCPGetByte(void);
extern uint32_t VCPGetBytes(uint8_t * rxBuf, uint32_t len);

uint32_t usbSendBytes(const uint8_t* sendBuf, uint32_t len)
{
	return VCP_DataTx(sendBuf, len);
}

uint32_t usbReceiveBytes(uint8_t* recvBuf, uint32_t len)
{
	return VCPGetBytes(recvBuf, len);
}

RESULT usbPowerOff(void)
{
	USBD_DeInitFull(&USB_OTG_dev);
	return USB_SUCCESS;
}

#include <STM32_USB_OTG_Driver/inc/usb_dcd_int.h>
void __irq_usb_fs(void)
{
	USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void x__irq_usbwakeup(void)
{
}

#endif
