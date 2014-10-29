MCU := STM32F103C8
PRODUCT_ID := 0003
ERROR_LED_PORT := GPIOB
ERROR_LED_PIN  := 2
MCU_SERIES := stm32f1
MCU_F1_LINE := performance
# This crap is due to ld-script limitations. If you know of a better
# way to go about this (like some magic ld switches to specify MEMORY
# at the command line), please tell us!
ifeq ($(BOOTLOADER),maple)
LD_MEM_DIR := sram_20k_flash_128k
endif
ifeq ($(BOOTLOADER),robotis)
LD_MEM_DIR := sram_20k_flash_128k_robotis
endif
