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


/**
 *  @file usb_descriptor.c
 *
 *  @brief aka application descriptor; big static struct and callbacks for sending
 *  the descriptor.
 *
 */


#include "usb_descriptor.h"

u8 u8_usbDeviceDescriptorDFU[18] = {
    0x12,   /* bLength */
    0x01,   /* bDescriptorType */
    0x00,   /* bcdUSB, version 1.00 */
    0x01,
    0x00,   /* bDeviceClass : See interface */
    0x00,   /* bDeviceSubClass : See interface*/
    0x00,   /* bDeviceProtocol : See interface */
    bMaxPacketSize, /* bMaxPacketSize0 0x40 = 64 */
    VEND_ID0,   /* idVendor     (0110) */
    VEND_ID1,

    PROD_ID0,   /* idProduct (0x1001 or 1002) */
    PROD_ID1,

    0x01,   /* bcdDevice*/
    0x02,
    0x01,   /* iManufacturer : index of string Manufacturer  */
    0x02,   /* iProduct      : index of string descriptor of product*/
    0x03,   /* iSerialNumber : index of string serial number*/
    0x01    /*bNumConfigurations */
};

ONE_DESCRIPTOR usbDeviceDescriptorDFU = {
    u8_usbDeviceDescriptorDFU,
    0x12
};

u8 u8_usbFunctionalDescriptor[9] = {
    /******************** DFU Functional Descriptor********************/
    0x09,   /*blength = 7 Bytes*/
    0x21,   /* DFU Functional Descriptor*/
    0x03,   /*bmAttributes, bitCanDnload | bitCanUpload */
    0xFF,   /*DetachTimeOut= 255 ms*/
    0x00,
    (wTransferSize & 0x00FF),
    (wTransferSize & 0xFF00) >> 8, /* TransferSize = 1024 Byte*/
    0x10,                          /* bcdDFUVersion = 1.1 */
    0x01
};

ONE_DESCRIPTOR usbFunctionalDescriptor = {
    u8_usbFunctionalDescriptor,
    0x09
};

#define u8_usbConfigDescriptorDFU_LENGTH 45
u8 u8_usbConfigDescriptorDFU[u8_usbConfigDescriptorDFU_LENGTH] = {
    0x09,   /* bLength: Configuation Descriptor size */
    0x02,   /* bDescriptorType: Configuration */
    u8_usbConfigDescriptorDFU_LENGTH,   /* wTotalLength: Bytes returned */
    0x00,
    0x01,   /* bNumInterfaces: 1 interface */
    0x01,   /* bConfigurationValue: */
    0x00,   /* iConfiguration: */
    0x80,   /* bmAttributes: */
    0x32,   /* MaxPower 100 mA */
    /* 09 */

    /************ Descriptor of DFU interface 0 Alternate setting 0 *********/
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x00,   /* bNumEndpoints*/
    0xFE,   /* bInterfaceClass: DFU */
    0x01,   /* bInterfaceSubClass */

    0x02,   /* nInterfaceProtocol, switched to 0x02 while in dfu_mode */

    0x04,   /* iInterface: */

    /************ Descriptor of DFU interface 0 Alternate setting 1 *********/
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x01,   /* bAlternateSetting: Alternate setting */
    0x00,   /* bNumEndpoints*/
    0xFE,   /* bInterfaceClass: DFU */
    0x01,   /* bInterfaceSubClass */

    0x02,   /* nInterfaceProtocol, switched to 0x02 while in dfu_mode */

    0x05,   /* iInterface: */
	
    /************ Descriptor of DFU interface 0 Alternate setting 2 *********/
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x02,   /* bAlternateSetting: Alternate setting */
    0x00,   /* bNumEndpoints*/
    0xFE,   /* bInterfaceClass: DFU */
    0x01,   /* bInterfaceSubClass */

    0x02,   /* nInterfaceProtocol, switched to 0x02 while in dfu_mode */

    0x06,   /* iInterface: */	
	

    /******************** DFU Functional Descriptor********************/
    0x09,   /*blength = 7 Bytes*/
    0x21,   /* DFU Functional Descriptor*/
    0x03,   /*bmAttributes, bitCanDnload | bitCanUpload */
    0xFF,   /*DetachTimeOut= 255 ms*/
    0x00,
    (wTransferSize & 0x00FF),
    (wTransferSize & 0xFF00) >> 8, /* TransferSize = 1024 Byte*/
    0x10,                          /* bcdDFUVersion = 1.1 */
    0x01
    /***********************************************************/
    /*36*/
};

ONE_DESCRIPTOR usbConfigDescriptorDFU = {
    u8_usbConfigDescriptorDFU,
    u8_usbConfigDescriptorDFU_LENGTH
};

#define USB_STR_LANG_ID_LEN 0x04
u8 u8_usbStringLangId[USB_STR_LANG_ID_LEN] = {
    USB_STR_LANG_ID_LEN,
    0x03,
    0x09,
    0x04    /* LangID = 0x0409: U.S. English */
};
#define USB_VENDOR_STR_LEN 0x12
u8 u8_usbStringVendor[USB_VENDOR_STR_LEN] = {
    USB_VENDOR_STR_LEN,
    0x03,
    'L', 0, 'e', 0, 'a', 0, 'f', 0, 'L', 0, 'a', 0, 'b', 0, 's', 0
};
#define USB_PRODUCT_STR_LEN 0x14
u8 u8_usbStringProduct[USB_PRODUCT_STR_LEN] = {
    USB_PRODUCT_STR_LEN,
    0x03,
    'M', 0, 'a', 0, 'p', 0, 'l', 0, 'e', 0, ' ', 0, '0', 0, '0', 0, '3', 0
};
#define USB_SERIAL_STR_LEN 0x10
u8 u8_usbStringSerial[USB_SERIAL_STR_LEN] = {
    USB_SERIAL_STR_LEN,
    0x03,
    'L', 0, 'L', 0, 'M', 0, ' ', 0, '0', 0, '0', 0, '3', 0
};

#define ALT0_STR_LEN 0x6A
u8 u8_usbStringAlt0[ALT0_STR_LEN] = {
ALT0_STR_LEN,
0x03,
'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,' ',0,'E',0,'R',0,'R',0,'O',0,'R',0,'.',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'R',0,'A',0,'M',0,' ',0,'i',0,'s',0,' ',0,'n',0,'o',0,'t',0,' ',0,'s',0,'u',0,'p',0,'p',0,'o',0,'r',0,'t',0,'e',0,'d',0
};

#define ALT1_STR_LEN 0x62
u8 u8_usbStringAlt1[ALT1_STR_LEN] = {
ALT1_STR_LEN,
0x03,
'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'5',0,'0',0,'0',0,'0',0
};

#define ALT2_STR_LEN 0x62
u8 u8_usbStringAlt2[ALT2_STR_LEN] = {
ALT2_STR_LEN,
0x03,
'B',0,'o',0,'o',0,'t',0,'l',0,'o',0,'a',0,'d',0,'e',0,'r',0,' ',0,'2',0,'.',0,'0',0,' ',0,'U',0,'p',0,'l',0,'o',0,'a',0,'d',0,' ',0,'t',0,'o',0,' ',0,'F',0,'l',0,'a',0,'s',0,'h',0,' ',0,'a',0,'d',0,'d',0,'r',0,'e',0,'s',0,'s',0,' ',0,'0',0,'x',0,'8',0,'0',0,'0',0,'2',0,'0',0,'0',0,'0',0
};

u8 u8_usbStringInterface = NULL;

ONE_DESCRIPTOR usbStringDescriptor[STR_DESC_LEN] = {
    { (u8 *)u8_usbStringLangId,  USB_STR_LANG_ID_LEN },
    { (u8 *)u8_usbStringVendor,  USB_VENDOR_STR_LEN },
    { (u8 *)u8_usbStringProduct, USB_PRODUCT_STR_LEN },
    { (u8 *)u8_usbStringSerial,  USB_SERIAL_STR_LEN },
    { (u8 *)u8_usbStringAlt0,    ALT0_STR_LEN },
    { (u8 *)u8_usbStringAlt1,    ALT1_STR_LEN },
	{ (u8 *)u8_usbStringAlt2,    ALT2_STR_LEN }
};
/*
 Roger.
 Javascript utility to make new ALT ID text structs
 
<html>
<script>
function convertText(txt,idNum)
{
	
	var txt2 ="#define ALT"+idNum+"_STR_LEN 0x"+(txt.length*2  + 2).toString(16).toUpperCase()+"<br/>u8 u8_usbStringAlt"+idNum+"[ALT"+idNum+"_STR_LEN] = {<br/>ALT"+idNum+"_STR_LEN,<br/>0x03,<br/>";
	for (var i=0;i<txt.length;i++)
	{
		txt2+="'"+txt[i]+"',0,";
	}
	return txt2.substring(0,txt2.length-1)+"<br/>};<br/>";
}
document.write("<pre>");
document.write(convertText("Bootloader 2.0 ERROR. Upload to RAM is not supported",0)+"<br/>");
document.write(convertText("Bootloader 2.0 Upload to Flash address 0x8005000",1)+"<br/>");
document.write(convertText("Bootloader 2.0 Upload to Flash address 0x8002000",2)+"<br/>");
document.write("</pre>");
</script>
</html>
*/
