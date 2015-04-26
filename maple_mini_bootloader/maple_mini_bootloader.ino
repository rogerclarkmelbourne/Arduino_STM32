/**
 *
 * Maple Mini bootloader transition sketch. Loads 7KB bootloader provided by Madias.
 * Based on sketch from Gregwar for Robotis OpenCM9.04, which is based on Maple bootloader code.
 *
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
 * WARNING                                                                 WARNING 
 * WARNING  This comes with NO WARRANTY, you have to be perfectly sure     WARNING
 * WARNING  and aware of what you are doing.                               WARNING
 * WARNING                                                                 WARNING
 * WARNING  Running this sketch will erase your bootloader to put the      WARNING
 * WARNING  New Maple Mini one. This is still a BETA version               WARNING
 * WARNING                                                                 WARNING
 * WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
 *
 */
#include "types.h"

/**
 * This is the Maple Mini bootloader v 2.0
 */
 
#include "maple_mini_boot20.h"
#define bootloader maple_mini_boot20

//#if !defined(__STM32F1__)
//This should not be used on a different MCU
//#endif 

#define BOOTLOADER_FLASH   ((uint32)0x08000000)
#define PAGE_SIZE          1024

#define SET_REG(addr,val) do { *(volatile uint32*)(addr)=val; } while(0)
#define GET_REG(addr)     (*(volatile uint32*)(addr))


#define RCC   ((uint32)0x40021000)
#define FLASH ((uint32)0x40022000)

#define RCC_CR      RCC
#define RCC_CFGR    (RCC + 0x04)
#define RCC_CIR     (RCC + 0x08)
#define RCC_AHBENR  (RCC + 0x14)
#define RCC_APB2ENR (RCC + 0x18)
#define RCC_APB1ENR (RCC + 0x1C)

#define FLASH_ACR     (FLASH + 0x00)
#define FLASH_KEYR    (FLASH + 0x04)
#define FLASH_OPTKEYR (FLASH + 0x08)
#define FLASH_SR      (FLASH + 0x0C)
#define FLASH_CR      (FLASH + 0x10)
#define FLASH_AR      (FLASH + 0x14)
#define FLASH_OBR     (FLASH + 0x1C)
#define FLASH_WRPR    (FLASH + 0x20)

#define FLASH_KEY1     0x45670123
#define FLASH_KEY2     0xCDEF89AB
#define FLASH_RDPRT    0x00A5
#define FLASH_SR_BSY   0x01
#define FLASH_CR_PER   0x02
#define FLASH_CR_PG    0x01
#define FLASH_CR_START 0x40

typedef struct {
    volatile uint32 CR; 
    volatile uint32 CFGR;
    volatile uint32 CIR;
    volatile uint32 APB2RSTR;
    volatile uint32 APB1RSTR;
    volatile uint32 AHBENR;
    volatile uint32 APB2ENR;
    volatile uint32 APB1ENR;
    volatile uint32 BDCR;
    volatile uint32 CSR;
} RCC_RegStruct;
#define pRCC ((RCC_RegStruct *) RCC)


bool flashErasePage(uint32 pageAddr) {
    uint32 rwmVal = GET_REG(FLASH_CR);
    rwmVal = FLASH_CR_PER;
    SET_REG(FLASH_CR, rwmVal);

    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    SET_REG(FLASH_AR, pageAddr);
    SET_REG(FLASH_CR, FLASH_CR_START | FLASH_CR_PER);
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    /* todo: verify the page was erased */

    rwmVal = 0x00;
    SET_REG(FLASH_CR, rwmVal);

    return true;
}
bool flashErasePages(uint32 pageAddr, uint16 n) {
    while (n-- > 0) {
        if (!flashErasePage(pageAddr + 0x400 * n)) {
            return false;
        }
    }

    return true;
}

bool flashWriteWord(uint32 addr, uint32 word) {
    volatile uint16 *flashAddr = (volatile uint16 *)addr;
    volatile uint32 lhWord = (volatile uint32)word & 0x0000FFFF;
    volatile uint32 hhWord = ((volatile uint32)word & 0xFFFF0000) >> 16;

    uint32 rwmVal = GET_REG(FLASH_CR);
    SET_REG(FLASH_CR, FLASH_CR_PG);

    /* apparently we need not write to FLASH_AR and can
       simply do a native write of a half word */
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr + 0x01) = (volatile uint16)hhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}
    *(flashAddr) = (volatile uint16)lhWord;
    while (GET_REG(FLASH_SR) & FLASH_SR_BSY) {}

    rwmVal &= 0xFFFFFFFE;
    SET_REG(FLASH_CR, rwmVal);

    /* verify the write */
    if (*(volatile uint32 *)addr != word) {
        return false;
    }

    return true;
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

/* Minimum_Source*/

void setupFLASH() {
    /* configure the HSI oscillator */
    if ((pRCC->CR & 0x01) == 0x00) {
        uint32 rwmVal = pRCC->CR;
        rwmVal |= 0x01;
        pRCC->CR = rwmVal;
    }   

    /* wait for it to come on */
    while ((pRCC->CR & 0x02) == 0x00) {}
}

bool writeChunk(uint32 *ptr, int size, char *data)
{
     flashErasePage((uint32)(ptr));
     
     for (int i = 0; i<size; i = i + 4) {
       if (!flashWriteWord((uint32)(ptr++), *((uint32 *)(data + i)))) {
        return false; 
       }
     }
     
     return true;
}

void setup() {
  pinMode(BOARD_LED_PIN, OUTPUT);
  digitalWrite(BOARD_LED_PIN, LOW);
  Serial.begin(9600);
  delay (5000);
}

void loop() {
  Serial.println ("*** BETA version 4/26/2015. Bootloader compiled by Roger.                              ****");
  Serial.println ("*** Sketches can starts at 0x08005000 or 0x8002000 by using a different upload ID      ****");
  Serial.println ("*** This sketch will update the bootloader in the Maple Mini.                          ****");
  Serial.println ("*** If you are running this on a different board, please do not continue               ****");
  Serial.println ();
  Serial.println ("*** When using this bootloader you can use up to 120KB of Flash and 20KB of RAM for a Sketch");
  Serial.print ("Bootloader Size: ");
  Serial.println (sizeof(bootloader), DEC);
    Serial.println ();
  Serial.println ("-To proceed with the update, enter Y");
  while (Serial.read() != 'Y')
  {
    delay(1);
  }
  Serial.println ("Proceeding with update, do not remove power.");
  
  setupFLASH();
  flashUnlock();
  
  int n = sizeof(bootloader);
  int success = 1;
  for (int i=0; i<n; i+=PAGE_SIZE) {
     int size = 0;
     uint32* chunk = (uint32 *)(BOOTLOADER_FLASH + i);
     
     size = n-i;
     if (size > PAGE_SIZE) size = PAGE_SIZE;
     
     if (!writeChunk(chunk, size, &bootloader[i])) {
      Serial.println ();
      Serial.println ("WARNING, Update Failed!!");
      success = 0;
      break;
     }
  }
  
  if (success){
  flashLock();
  Serial.println ();
  Serial.println ("Update completed successfully. Reboot now and replace this sketch");
  while (1){
  digitalWrite(BOARD_LED_PIN, LOW);
  delay(500);
  digitalWrite(BOARD_LED_PIN, HIGH);  
  delay(500);
  }
  }
}

