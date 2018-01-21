/*
 * STM32F4xx high density linker script for
 * JTAG (bare metal, no bootloader) builds.
 */

MEMORY
{
  ccmram (rw): ORIGIN = 0x10000000, LENGTH = 64K
  ram (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
  rom (rx)   : ORIGIN = 0x08000000, LENGTH = 512K
}

/* GROUP(libcs3_stm32_high_density.a) */

REGION_ALIAS("REGION_TEXT", rom);
REGION_ALIAS("REGION_DATA", ram);
REGION_ALIAS("REGION_BSS", ram);
REGION_ALIAS("REGION_RODATA", rom);

_FLASH_BUILD = 1;
INCLUDE common.inc
