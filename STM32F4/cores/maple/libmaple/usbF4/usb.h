#ifndef _USBF4_H_
#define _USBF4_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <VCP/usb_conf.h>

typedef enum _RESULT
{
  USB_SUCCESS = 0,    /* Process successfully */
  USB_ERROR,
  USB_UNSUPPORT,
  USB_NOT_READY       /* The process has not been finished, endpoint will be
                         NAK to further request */
} RESULT;

void setupUSB(void);
void disableUSB(void);
void usbSuspend(void);
void usbResumeInit(void);
//void usbResume(RESUME_STATE);

RESULT usbPowerOn(void);
RESULT usbPowerOff(void);

void usbDsbISR(void);
void usbEnbISR(void);

void   usbBlockingSendByte(char ch);
uint32_t usbSendBytes(const uint8_t* sendBuf,uint32_t len);
uint32_t usbBytesAvailable(void);
uint32_t usbReceiveBytes(uint8_t* recvBuf, uint32_t len);
uint8_t usbGetDTR(void);
uint8_t usbGetRTS(void);
uint8_t usbIsConnected(void);
uint8_t usbIsConfigured(void);
uint16_t usbGetPending(void);
void usbEnableBlockingTx(void);
void usbDisableBlockingTx(void);



void __irq_OTG_FS_IRQHandler(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _USB_H_
