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

#ifndef __DFU_H
#define __DFU_H

#include "common.h"
/*
#define DFU_UPLOAD_NONE 0
#define DFU_UPLOAD_RAM 1
#define DFU_UPLOAD_FLASH_0X8005000 2
#define DFU_UPLOAD_FLASH_0X8002000 3
*/
typedef enum {DFU_UPLOAD_NONE, DFU_UPLOAD_RAM, DFU_UPLOAD_FLASH_0X8005000,DFU_UPLOAD_FLASH_0X8002000} dfuUploadTypes_t;

/* exposed types */
typedef u8 *(*ClassReqCB)(u16);

/* exposed structs */
typedef struct _DFUStatus {
    u8 bStatus;
    u8 bwPollTimeout0;
    u8 bwPollTimeout1;
    u8 bwPollTimeout2;
    u8 bState;  /* state of device at the time the host receives the message! */
    u8 iString;
} DFUStatus;

typedef enum _PLOT {
    BEGINNING,
    MIDDLE,
    END,
    WAIT
} PLOT;


/*** DFU bRequest Values ******/
/* bmRequestType, wValue,    wIndex,    wLength, Data */
#define  DFU_DETACH    0x00 /* 0x21,          wTimeout,  Interface, Zero,    None */
#define  DFU_DNLOAD    0x01 /* 0x21,          wBlockNum, Interface, Length,  Firmware */
#define  DFU_UPLOAD    0x02 /* 0xA1,          Zero,      Interface, Length,  Firmware */
#define  DFU_GETSTATUS 0x03 /* 0xA1,          Zero,      Interface, 6,       Status */
#define  DFU_CLRSTATUS 0x04 /* 0x21,          Zero,      Interface, Zero,    None */
#define  DFU_GETSTATE  0x05 /* 0xA1,          Zero,      Interface, 1,       State */
#define  DFU_ABORT     0x06 /* 0x21,          Zero,      Interface, Zero,    None */

/*** DFU Status Values ******/
#define  OK              0x00 /* No error */
#define  errTARGET       0x01 /* File is not appropriate for this device */
#define  errFILE         0x02 /* File fails some vendor tests */
#define  errWRITE        0x03 /* Device is unable to write memory */
#define  errERASE        0x04 /* Memory erase failed */
#define  errCHECK_ERASED 0x05 /* Memory erase check failed */
#define  errPROG         0x06 /* Program memory function failed */
#define  errVERIFY       0x07 /* Written program failed verification */
#define  errADDRESS      0x08 /* address out of range */
#define  errNOTDONE      0x09 /* received DNLOAD with wLength=0, but firmware seems incomplete */
#define  errFIRMWARE     0x0A /* Runtime firmware corrupt, cannot return to non-dfu operations! */
#define  errVENDOR       0x0B /* vendor specific error */
#define  errUSBR         0x0C /* Unexpected usb reset! */
#define  errPOR          0x0D /* Unexpected power on reset */
#define  errUNKNOWN      0x0E /* Unknown error */
#define  errSTALLEDPKT   0x0F /* device stalled unexpected request */
/***************************/

/*** DFU State Values **************/
#define  appIDLE                0x00
#define  appDETACH              0x01
#define  dfuIDLE                0x02
#define  dfuDNLOAD_SYNC         0x03
#define  dfuDNBUSY              0x04
#define  dfuDNLOAD_IDLE         0x05
#define  dfuMANIFEST_SYNC       0x06
#define  dfuMANIFEST            0x07
#define  dfuMANIFEST_WAIT_RESET 0x08
#define  dfuUPLOAD_IDLE         0x09
#define  dfuERROR               0x0A
/***********************************/



extern volatile bool dfuBusy;

/* exposed functions */
void dfuInit(void);  /* singleton dfu initializer */

/* should consume dfuEvent type, but for now we can use pInfo (see comment above) */
bool dfuUpdateByRequest(void);  /* returns if new status is OK */
void dfuUpdateByReset(void);
void dfuUpdateByTimeout(void);

/* usb callbacks */
u8 *dfuCopyState(u16);
u8 *dfuCopyStatus(u16);
u8 *dfuCopyDNLOAD(u16);
u8 *dfuCopyUPLOAD(u16);

void dfuCopyBufferToExec(void);
bool checkTestFile(void);

u8 dfuGetState(void);
void dfuSetState(u8);
bool dfuUploadStarted();
void dfuFinishUpload();


#endif
