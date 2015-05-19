# 1 "./stm32_lib/cortexm3_macro.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "./stm32_lib/cortexm3_macro.S"
# 16 "./stm32_lib/cortexm3_macro.S"
 .cpu cortex-m3
.fpu softvfp
.syntax unified
.thumb
.text


  .globl __WFI
  .globl __WFE
  .globl __SEV
  .globl __ISB
  .globl __DSB
  .globl __DMB
  .globl __SVC
  .globl __MRS_CONTROL
  .globl __MSR_CONTROL
  .globl __MRS_PSP
  .globl __MSR_PSP
  .globl __MRS_MSP
  .globl __MSR_MSP
  .globl __RESETPRIMASK
  .globl __SETPRIMASK
  .globl __READ_PRIMASK
  .globl __RESETFAULTMASK
  .globl __SETFAULTMASK
  .globl __READ_FAULTMASK
  .globl __BASEPRICONFIG
  .globl __GetBASEPRI
  .globl __REV_HalfWord
  .globl __REV_Word







.thumb_func
__WFI:

    WFI
    BX r14







.thumb_func
__WFE:

    WFE
    BX r14







.thumb_func
__SEV:

    SEV
    BX r14







.thumb_func
__ISB:

    ISB
    BX r14







.thumb_func
__DSB:

    DSB
    BX r14







.thumb_func
__DMB:

    DMB
    BX r14







.thumb_func
__SVC:

    SVC 0x01
    BX r14







.thumb_func
__MRS_CONTROL:

  MRS r0,control
  BX r14







.thumb_func
__MSR_CONTROL:

  MSR control, r0
  ISB
  BX r14






.thumb_func
__MRS_PSP:

  MRS r0, psp
  BX r14







.thumb_func
__MSR_PSP:

    MSR psp, r0
    BX r14







.thumb_func
__MRS_MSP:

  MRS r0, msp
  BX r14







.thumb_func
__MSR_MSP:

    MSR msp, r0
    BX r14







.thumb_func
__RESETPRIMASK:

  CPSIE i
  BX r14







.thumb_func
__SETPRIMASK:

  CPSID i
  BX r14







.thumb_func
__READ_PRIMASK:

  MRS r0, PRIMASK
  BX r14







.thumb_func
__RESETFAULTMASK:

  CPSIE f
  BX r14







.thumb_func
__SETFAULTMASK:

  CPSID f
  BX r14







.thumb_func
__READ_FAULTMASK:

  MRS r0, FAULTMASK
  BX r14







.thumb_func
__BASEPRICONFIG:

  MSR basepri, r0
  BX r14







.thumb_func
__GetBASEPRI:

  MRS r0, basepri_max
  BX r14






.thumb_func
__REV_HalfWord:

  REV16 r0, r0
  BX r14







.thumb_func
__REV_Word:

  REV r0, r0
  BX r14

.end
