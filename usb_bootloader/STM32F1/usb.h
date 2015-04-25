/* *****************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ****************************************************************************/
#ifndef __USB_H
#define __USB_H

#include "common.h"
#include "usb_lib.h"
#include "usb_descriptor.h"

/* USB Disc Pin Setup.  On the Mini, USB DISC is PB9 */
#define USB_DISC_BANK         GPIOB
#define USB_DISC              9
#define USB_DISC_CR           GPIO_CRH(USB_DISC_BANK)
#define USB_DISC_CR_MASK      0xFFFFFF0F
#define USB_DISC_CR_OUTPUT_OD 0x00000050
#define RCC_APB2ENR_USB       0x00000008
#define RCC_APB1ENR_USB_CLK   0x00800000

/* USB configuration params */
#define BTABLE_ADDRESS  0x00
#define ENDP0_RXADDR    0x40
#define ENDP0_TXADDR    0x80    /* gives 64 bytes i/o buflen */
#define ENDP1_TXADDR    0xC0
#define ENDP2_TXADDR    0x100
#define ENDP3_RXADDR    0x110

#define bMaxPacketSize  0x40    /* 64B,  maximum for usb FS devices */
#define wTransferSize   0x0400  /* 1024B, want: maxpacket < wtransfer < 10KB (to ensure everything can live in ram */

#define NUM_ENDPTS      0x01

/* do we gracefully implement usb suspend? */
#define F_SUSPEND_ENABLED 1

/* defines which interrupts are handled */
#define ISR_MSK (CNTR_CTRM   |			\
                 CNTR_WKUPM  |			\
                 CNTR_SUSPM  |			\
                 CNTR_ERRM   |			\
                 CNTR_SOFM   |			\
                 CNTR_ESOFM  |			\
                 CNTR_RESETM			\
                )

typedef enum _RESUME_STATE {
    RESUME_EXTERNAL,
    RESUME_INTERNAL,
    RESUME_LATER,
    RESUME_WAIT,
    RESUME_START,
    RESUME_ON,
    RESUME_OFF,
    RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE {
    UNCONNECTED,
    ATTACHED,
    POWERED,
    SUSPENDED,
    ADDRESSED,
    CONFIGURED
} DEVICE_STATE;

void setupUSB(void);
void usbDsbBus(void);
void usbAppInit(void); /* singleton usb initializer */

void usbSuspend(void);
void usbResumeInit(void);
void usbResume(RESUME_STATE state);
RESULT usbPowerOn(void);
RESULT usbPowerOff(void);

/* internal functions (as per the usb_core pProperty structure) */
void usbInit(void);
void usbReset(void);
void usbStatusIn(void);
void usbStatusOut(void);

RESULT usbDataSetup(u8 request);
RESULT usbNoDataSetup(u8 request);
RESULT usbGetInterfaceSetting(u8, u8);

u8 *usbGetDeviceDescriptor(u16 length);
u8 *usbGetConfigDescriptor(u16 length);
u8 *usbGetStringDescriptor(u16 length);
u8 *usbGetFunctionalDescriptor(u16 length);

/* internal callbacks to respond to standard requests */
void usbGetConfiguration(void);
void usbSetConfiguration(void);
void usbGetInterface(void);
void usbSetInterface(void);
void usbGetStatus(void);
void usbClearFeature(void);
void usbSetEndpointFeature(void);
void usbSetDeviceFeature(void);
void usbSetDeviceAddress(void);

/* the small number of comm emulator functions to
   eventually be migrated into their own usart sources
*/
u8 *vcomGetLineCoding(u16 length);
u8 *vcomSetLineCoding(u16 length);
void vcomEp1In(void);
void vcomEp3Out(void);

/* Interrupt setup/handling exposed only so that
   its obvious from main what interrupts are overloaded
   from c_only_startup.s (see the top of main.c) */
void usbDsbISR(void);
void usbEnbISR(void);

/* override the weakly defined isr in linker */
void USB_LP_CAN1_RX0_IRQHandler(void);


void nothingProc(void);

#endif
