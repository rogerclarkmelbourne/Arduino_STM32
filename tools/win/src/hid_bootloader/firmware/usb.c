/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stm32f10x.h>
#include <stdlib.h>

#include "usb.h"
#include "bitwise.h"

USB_RxTxBuf_t RxTxBuffer[MAX_EP_NUM];

volatile uint8_t DeviceAddress = 0;
volatile uint16_t DeviceConfigured = 0, DeviceStatus = 0;
void (*_EPHandler)(uint16_t) = NULL;
void (*_USBResetHandler)(void) = NULL;

/* USB String Descriptors */
const uint8_t sdVendor[] = {
	0x2A, // Size,
	0x03, // Descriptor type
	'w', 0, 'w', 0, 'w', 0, '.', 0, 'b', 0, 'r', 0, 'u', 0, 'n', 0, 'o', 0,
	'f', 0, 'r', 0, 'e', 0, 'i', 0, 't', 0, 'a', 0, 's', 0, '.', 0, 'c', 0,
	'o', 0, 'm', 0
};

const uint8_t sdProduct[] = {
	0x2C, // Size,
	0x03, // Descriptor type
	'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, 'F', 0, ' ', 0, 'H', 0, 'I', 0,
	'D', 0, ' ', 0, 'B', 0, 'o', 0, 'o', 0, 't', 0, 'l', 0, 'o', 0, 'a', 0,
	'd', 0, 'e', 0, 'r', 0
};

const uint8_t sdSerial[] = {
	0x16, // Size,
	0x03, // Descriptor type
	'1',0,'2',0,'3',0,'4',0,'5',0,'6',0,'7',0,'8',0,'9',0,'0',0
};

const uint8_t sdLangID[] = {
		0x04, // Size,
		0x03, // Descriptor type
		0x09, 0x04
};

void USB_PMA2Buffer(uint8_t EPn) {
	uint8_t Count = RxTxBuffer[EPn].RXL = (_GetEPRxCount(EPn) & 0x3FF);
	uint32_t *Address = (uint32_t *) (PMAAddr + _GetEPRxAddr(EPn) * 2);
	uint16_t *Destination = (uint16_t *) RxTxBuffer[EPn].RXB;

	for (uint8_t i = 0; i < Count; i++) {
		*(uint16_t *) Destination = *(uint16_t *) Address;
		Destination++;
		Address++;
	}
}

void USB_Buffer2PMA(uint8_t EPn) {
	uint32_t *Destination;
	uint8_t Count;

	Count = RxTxBuffer[EPn].TXL <= RxTxBuffer[EPn].MaxPacketSize ? RxTxBuffer[EPn].TXL : RxTxBuffer[EPn].MaxPacketSize;
	_SetEPTxCount(EPn, Count);

	Destination = (uint32_t *) (PMAAddr + _GetEPTxAddr(EPn) * 2);

	for (uint8_t i = 0; i < (Count + 1) / 2; i++) {
		*(uint32_t *) Destination = *(uint16_t *) RxTxBuffer[EPn].TXB;
		Destination++;
		RxTxBuffer[EPn].TXB++;
	}

	RxTxBuffer[EPn].TXL -= Count;
}

void USB_SendData(uint8_t EPn, uint16_t *Data, uint16_t Length) {

	if (EPn > 0 && !DeviceConfigured) {
		return;
	}

	RxTxBuffer[EPn].TXL = Length;

	RxTxBuffer[EPn].TXB = Data;

	if (Length > 0) {
		USB_Buffer2PMA(EPn);
	} else {
		_SetEPTxCount(EPn, 0);
	}

	_SetEPTxValid(EPn);
}

void USB_Shutdown() {
	bit_set(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

	// Disable USB IRQ
	NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
	_SetISTR(0);

	DeviceConfigured = DeviceStatus = 0;

	_EPHandler = NULL;

	_USBResetHandler = NULL;

	// Turn USB Macrocell off (FRES + PWDN)
	_SetCNTR(0x03);

	// PA_12 output mode: General purpose output open drain (b01)
	bit_set(GPIOA->CRH, GPIO_CRH_CNF12_0);
	bit_clear(GPIOA->CRH, GPIO_CRH_CNF12_1);

	// Set PA_12 to output
	bit_set(GPIOA->CRH, GPIO_CRH_MODE12);// PA_12 set as: Output mode, max speed 50 MHz.

	// Sinks A12 to GND
	GPIOA->BRR = GPIO_BRR_BR12;

	// Disable USB Clock on APB1
	bit_clear(RCC->APB1ENR, RCC_APB1ENR_USBEN);
}

static void USB_TurnOn() {
	bit_set(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);

	// PA_12 output mode: General purpose Input Float (b01)
	bit_set(GPIOA->CRH, GPIO_CRH_CNF12_0);
	bit_clear(GPIOA->CRH, GPIO_CRH_CNF12_1);

	// Set PA_12 to Input mode
	bit_clear(GPIOA->CRH, GPIO_CRH_MODE12);
}

void USB_Init(void (*EPHandlerPtr)(uint16_t), void (*ResetHandlerPtr)(void)) {

	// Reset RX and TX lengths inside RxTxBuffer struct for all endpoints
	for(int i = 0; i < MAX_EP_NUM; i++) {
		RxTxBuffer[i].RXL = RxTxBuffer[i].TXL = 0;
	}

	_EPHandler = EPHandlerPtr;
	_USBResetHandler = ResetHandlerPtr;

	USB_TurnOn();

	DeviceConfigured = DeviceStatus = 0;

	bit_set(RCC->APB1ENR, RCC_APB1ENR_USBEN);
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	/*** CNTR_PWDN = 0 ***/
	_SetCNTR(CNTR_FRES);

	/* The following sequence is recommended:
	 1- FRES = 0
	 2- Wait until RESET flag = 1 (polling)
	 3- clear ISTR register */

	/*** CNTR_FRES = 0 ***/
	_SetCNTR(0);

	/* Wait until RESET flag = 1 (polling) */
	while ((_GetISTR() & ISTR_RESET) == 1);

	/*** Clear pending interrupts ***/
	_SetISTR(0);

	/*** Set interrupt mask ***/
	_SetCNTR(CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM);
}

uint16_t USB_IsDeviceConfigured() {
	return DeviceConfigured;
}

void USB_LP_CAN1_RX0_IRQHandler() {
	// Handle Reset
	if (_GetISTR() & ISTR_RESET) {
		_SetISTR(_GetISTR() & CLR_RESET);
		if(_USBResetHandler) {
			_USBResetHandler();
		}
		return;
	}

	// Handle EP data
	if (_GetISTR() & ISTR_CTR) { // Handle data on EP
		if(_EPHandler) {
			_EPHandler(_GetISTR());
		}
		_SetISTR(_GetISTR() & CLR_CTR);
		return;
	}

	// Handle DOVR
	if (_GetISTR() & ISTR_DOVR) {
		_SetISTR(_GetISTR() & CLR_DOVR);
		return;
	}

	// Handle Suspend
	if (_GetISTR() & ISTR_SUSP) {
		_SetISTR(_GetISTR() & CLR_SUSP);

		// If device address is assigned, then reset it
		if (_GetDADDR() & 0x007f) {
			_SetDADDR(0);
			_SetCNTR(_GetCNTR() & ~CNTR_SUSPM);
		}

		return;
	}

	// Handle Error
	if (_GetISTR() & ISTR_ERR) {
		_SetISTR(_GetISTR() & CLR_ERR);
		return;
	}

	// Handle Wakeup
	if (_GetISTR() & ISTR_WKUP) {
		_SetISTR(_GetISTR() & CLR_WKUP);
		return;
	}

	// Handle SOF
	if (_GetISTR() & ISTR_SOF) {
		_SetISTR(_GetISTR() & CLR_SOF);
		return;
	}

	// Handle ESOF
	if (_GetISTR() & ISTR_ESOF) {
		_SetISTR(_GetISTR() & CLR_ESOF);
		return;
	}

	// Default to clear all interrupt flags
	_SetISTR(0);
}

