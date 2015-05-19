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
 *  @file hardware.c
 *
 *  @brief init routines to setup clocks, interrupts, also destructor functions.
 *  does not include USB stuff. EEPROM read/write functions.
 *
 */

#include "hardware.h"

void setPin(u32 bank, u8 pin) {
    u32 pinMask = 0x1 << (pin);
    SET_REG(GPIO_BSRR(bank), pinMask);
}

void resetPin(u32 bank, u8 pin) {
    u32 pinMask = 0x1 << (16 + pin);
    SET_REG(GPIO_BSRR(bank), pinMask);
}

bool readPin(u32 bank, u8 pin) {
    // todo, implement read
    if (GET_REG(GPIO_IDR(bank)) & (0x01 << pin)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void strobePin(u32 bank, u8 pin, u8 count, u32 rate) {
    resetPin(bank, pin);

    u32 c;
    while (count-- > 0) {
        for (c = rate; c > 0; c--) {
            asm volatile("nop");
        }
        setPin(bank, pin);
        for (c = rate; c > 0; c--) {
            asm volatile("nop");
        }
        resetPin(bank, pin);
    }
}

void systemReset(void) {
    SET_REG(RCC_CR, GET_REG(RCC_CR)     | 0x00000001);
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xF8FF0000);
    SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFEF6FFFF);
    SET_REG(RCC_CR, GET_REG(RCC_CR)     & 0xFFFBFFFF);
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) & 0xFF80FFFF);

    SET_REG(RCC_CIR, 0x00000000);  /* disable all RCC interrupts */
}

void setupCLK(void) {
    /* enable HSE */
    SET_REG(RCC_CR, GET_REG(RCC_CR) | 0x00010001);
    while ((GET_REG(RCC_CR) & 0x00020000) == 0); /* for it to come on */

    /* enable flash prefetch buffer */
    SET_REG(FLASH_ACR, 0x00000012);

    /* Configure PLL */
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) | 0x001D0400); /* pll=72Mhz,APB1=36Mhz,AHB=72Mhz */
    SET_REG(RCC_CR, GET_REG(RCC_CR)     | 0x01000000); /* enable the pll */
    while ((GET_REG(RCC_CR) & 0x03000000) == 0);         /* wait for it to come on */

    /* Set SYSCLK as PLL */
    SET_REG(RCC_CFGR, GET_REG(RCC_CFGR) | 0x00000002);
    while ((GET_REG(RCC_CFGR) & 0x00000008) == 0); /* wait for it to come on */
}

void setupLED (void) {
  /* enable LED pin */
  pRCC->APB2ENR |= RCC_APB2ENR_LED;

  /* Setup LED pin as output open drain */
  SET_REG(LED_BANK_CR,(GET_REG(LED_BANK_CR) & LED_CR_MASK) | LED_CR_MODE);
  setPin(LED_BANK, LED);
}

void setupBUTTON (void) {
  /* enable button pin */
  pRCC->APB2ENR |= RCC_APB2ENR_BUT;

  /* Setup button pin as floating input */
  SET_REG(BUT_BANK_CR,(GET_REG(BUT_BANK_CR) & BUT_CR_MASK) | BUT_CR_OUTPUT_IN);
  setPin(BUTTON_BANK, BUTTON);
}

void setupFLASH() {
    /* configure the HSI oscillator */
    if ((pRCC->CR & 0x01) == 0x00) {
        u32 rwmVal = pRCC->CR;
        rwmVal |= 0x01;
        pRCC->CR = rwmVal;
    }

    /* wait for it to come on */
    while ((pRCC->CR & 0x02) == 0x00) {}
}

bool checkUserCode(u32 usrAddr) {
    u32 sp = *(vu32 *) usrAddr;

    if ((sp & 0x2FFE0000) == 0x20000000) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}

void jumpToUser(u32 usrAddr) {
    typedef void (*funcPtr)(void);

    u32 jumpAddr = *(vu32 *)(usrAddr + 0x04); /* reset ptr in vector table */
    funcPtr usrMain = (funcPtr) jumpAddr;

    /* tear down all the dfu related setup */
    // disable usb interrupts, clear them, turn off usb, set the disc pin
    // todo pick exactly what we want to do here, now its just a conservative
    flashLock();
    usbDsbISR();
    nvicDisableInterrupts();
    setPin(GPIOC, 12); // disconnect usb from host. todo, macroize pin
    systemReset(); // resets clocks and periphs, not core regs


    __MSR_MSP(*(vu32 *) usrAddr);             /* set the users stack ptr */

    usrMain();                                /* go! */
}

void nvicInit(NVIC_InitTypeDef *NVIC_InitStruct) {
    u32 tmppriority = 0x00;
    u32 tmpreg      = 0x00;
    u32 tmpmask     = 0x00;
    u32 tmppre      = 0;
    u32 tmpsub      = 0x0F;

    SCB_TypeDef *rSCB = (SCB_TypeDef *) SCB_BASE;
    NVIC_TypeDef *rNVIC = (NVIC_TypeDef *) NVIC_BASE;


    /* Compute the Corresponding IRQ Priority --------------------------------*/
    tmppriority = (0x700 - (rSCB->AIRCR & (u32)0x700)) >> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (u32)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;

    tmppriority = tmppriority << 0x04;
    tmppriority = ((u32)tmppriority) << ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);

    tmpreg = rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)];
    tmpmask = (u32)0xFF << ((NVIC_InitStruct->NVIC_IRQChannel & (u8)0x03) * 0x08);
    tmpreg &= ~tmpmask;
    tmppriority &= tmpmask;
    tmpreg |= tmppriority;

    rNVIC->IPR[(NVIC_InitStruct->NVIC_IRQChannel >> 0x02)] = tmpreg;

    /* Enable the Selected IRQ Channels --------------------------------------*/
    rNVIC->ISER[(NVIC_InitStruct->NVIC_IRQChannel >> 0x05)] =
        (u32)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (u8)0x1F);
}

void nvicDisableInterrupts() {
    NVIC_TypeDef *rNVIC = (NVIC_TypeDef *) NVIC_BASE;
    rNVIC->ICER[0] = 0xFFFFFFFF;
    rNVIC->ICER[1] = 0xFFFFFFFF;
    rNVIC->ICPR[0] = 0xFFFFFFFF;
    rNVIC->ICPR[1] = 0xFFFFFFFF;

    SET_REG(STK_CTRL, 0x04); /* disable the systick, which operates separately from nvic */
}

void systemHardReset(void) {
    SCB_TypeDef *rSCB = (SCB_TypeDef *) SCB_BASE;

    /* Reset  */
    rSCB->AIRCR = (u32)AIRCR_RESET_REQ;

    /*  should never get here */
    while (1) {
        asm volatile("nop");
    }
}

bool flashErasePage(u32 pageAddr) {
    u32 rwmVal = GET_REG(FLASH_CR);
    rwmVal = FLASH_CR_PER;
    SET_REG(FLASH_CR, rwmVal);

    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    SET_REG(FLASH_AR, pageAddr);
    SET_REG(FLASH_CR, FLASH_CR_START | FLASH_CR_PER);
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    /* todo: verify the page was erased */

    rwmVal = 0x00;
    SET_REG(FLASH_CR, rwmVal);

    return TRUE;
}
bool flashErasePages(u32 pageAddr, u16 n) {
    while (n-- > 0) {
        if (!flashErasePage(pageAddr + 0x400 * n)) {
            return FALSE;
        }
    }

    return TRUE;
}

bool flashWriteWord(u32 addr, u32 word) {
    vu16 *flashAddr = (vu16 *)addr;
    vu32 lhWord = (vu32)word & 0x0000FFFF;
    vu32 hhWord = ((vu32)word & 0xFFFF0000) >> 16;

    u32 rwmVal = GET_REG(FLASH_CR);
    SET_REG(FLASH_CR, FLASH_CR_PG);

    /* apparently we need not write to FLASH_AR and can
       simply do a native write of a half word */
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr + 0x01) = (vu16)hhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr) = (vu16)lhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    rwmVal &= 0xFFFFFFFE;
    SET_REG(FLASH_CR, rwmVal);

    /* verify the write */
    if (*(vu32 *)addr != word) {
        return FALSE;
    }

    return TRUE;
}

void flashLock() {
    /* take down the HSI oscillator? it may be in use elsewhere */

    /* ensure all FPEC functions disabled and lock the FPEC */
    SET_REG(FLASH_CR, 0x00000080);
}

void flashUnlock() {
    /* unlock the flash */
    SET_REG(FLASH_KEYR, FLASH_KEY1);
    SET_REG(FLASH_KEYR, FLASH_KEY2);
}




