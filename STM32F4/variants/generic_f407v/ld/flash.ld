/*
 * Discovery F4 (STM32F407VGT6, high density) linker script for
 * Flash builds.
 */

MEMORY
{
  ram (rwx) : ORIGIN = 0x20000C00, LENGTH = 125K
  rom (rx)  : ORIGIN = 0x08010000, LENGTH = 960K  /* ala42 */
}

/* GROUP(libcs4_stm32_high_density.a) */

REGION_ALIAS("REGION_TEXT", rom);
REGION_ALIAS("REGION_DATA", ram);
REGION_ALIAS("REGION_BSS", ram);
REGION_ALIAS("REGION_RODATA", rom);

_FLASH_BUILD = 1;
INCLUDE common.inc
