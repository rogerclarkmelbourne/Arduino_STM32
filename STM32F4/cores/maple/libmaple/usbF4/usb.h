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

inline RESULT usbPowerOn(void) { return USB_SUCCESS; }
RESULT usbPowerOff(void);

void   usbBlockingSendByte(char ch);
uint32_t usbSendBytes(const uint8_t* sendBuf,uint32_t len);
extern uint32_t VCPBytesAvailable(void);
inline uint32_t usbBytesAvailable(void) { return VCPBytesAvailable(); }
uint32_t usbReceiveBytes(uint8_t* recvBuf, uint32_t len);
extern uint8_t VCPGetDTR(void);
extern uint8_t VCPGetRTS(void);
inline uint8_t usbGetDTR(void) { return VCPGetDTR(); };
inline uint8_t usbGetRTS(void) { return VCPGetRTS(); };

extern uint8_t usb_isConnected(void);
extern uint8_t usb_isConfigured(void);
inline uint8_t usbIsConnected(void) { return usb_isConnected(); }
inline uint8_t usbIsConfigured(void) { return usb_isConfigured(); }

uint16_t usbGetPending(void);
extern void VCP_SetUSBTxBlocking(const uint8_t mode);
inline void usbEnableBlockingTx(void) { VCP_SetUSBTxBlocking(1); }
inline void usbDisableBlockingTx(void) { VCP_SetUSBTxBlocking(0); }


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _USB_H_
