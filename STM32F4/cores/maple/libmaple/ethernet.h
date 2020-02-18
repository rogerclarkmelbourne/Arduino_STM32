// ethernet.h

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include "gpio.h"
#include <boards.h>

#ifdef __cplusplus
 extern "C" {
#endif // __cplusplus


typedef struct
{
  __IO uint32_t MACCR;
  __IO uint32_t MACFFR;
  __IO uint32_t MACHTHR;
  __IO uint32_t MACHTLR;
  __IO uint32_t MACMIIAR;
  __IO uint32_t MACMIIDR;
  __IO uint32_t MACFCR;
  __IO uint32_t MACVLANTR;             /*    8 */
  uint32_t      RESERVED0[2];
  __IO uint32_t MACRWUFFR;             /*   11 */
  __IO uint32_t MACPMTCSR;
  uint32_t      RESERVED1;
  __IO uint32_t MACDBGR;
  __IO uint32_t MACSR;                 /*   15 */
  __IO uint32_t MACIMR;
  __IO uint32_t MACA0HR;
  __IO uint32_t MACA0LR;
  __IO uint32_t MACA1HR;
  __IO uint32_t MACA1LR;
  __IO uint32_t MACA2HR;
  __IO uint32_t MACA2LR;
  __IO uint32_t MACA3HR;
  __IO uint32_t MACA3LR;               /*   24 */
  uint32_t      RESERVED2[40];
  __IO uint32_t MMCCR;                 /*   65 */
  __IO uint32_t MMCRIR;
  __IO uint32_t MMCTIR;
  __IO uint32_t MMCRIMR;
  __IO uint32_t MMCTIMR;               /*   69 */
  uint32_t      RESERVED3[14];
  __IO uint32_t MMCTGFSCCR;            /*   84 */
  __IO uint32_t MMCTGFMSCCR;
  uint32_t      RESERVED4[5];
  __IO uint32_t MMCTGFCR;
  uint32_t      RESERVED5[10];
  __IO uint32_t MMCRFCECR;
  __IO uint32_t MMCRFAECR;
  uint32_t      RESERVED6[10];
  __IO uint32_t MMCRGUFCR;
  uint32_t      RESERVED7[334];
  __IO uint32_t PTPTSCR;
  __IO uint32_t PTPSSIR;
  __IO uint32_t PTPTSHR;
  __IO uint32_t PTPTSLR;
  __IO uint32_t PTPTSHUR;
  __IO uint32_t PTPTSLUR;
  __IO uint32_t PTPTSAR;
  __IO uint32_t PTPTTHR;
  __IO uint32_t PTPTTLR;
  __IO uint32_t RESERVED8;
  __IO uint32_t PTPTSSR;
  uint32_t      RESERVED9[565];
  __IO uint32_t DMABMR;
  __IO uint32_t DMATPDR;
  __IO uint32_t DMARPDR;
  __IO uint32_t DMARDLAR;
  __IO uint32_t DMATDLAR;
  __IO uint32_t DMASR;
  __IO uint32_t DMAOMR;
  __IO uint32_t DMAIER;
  __IO uint32_t DMAMFBOCR;
  __IO uint32_t DMARSWTR;
  uint32_t      RESERVED10[8];
  __IO uint32_t DMACHTDR;
  __IO uint32_t DMACHRDR;
  __IO uint32_t DMACHTBAR;
  __IO uint32_t DMACHRBAR;
} eth_reg_map;

//-----------------------------------------------------------------------------
#define ETH_BASE              (0x40028000U)
#define ETH_MAC_BASE          (ETH_BASE)
#define ETH_MMC_BASE          (ETH_BASE + 0x0100U)
#define ETH_PTP_BASE          (ETH_BASE + 0x0700U)
#define ETH_DMA_BASE          (ETH_BASE + 0x1000U)

#define ETH                 ((eth_reg_map*) ETH_BASE)  


/******************************************************************************/
/*                                                                            */
/*                Ethernet MAC Registers bits definitions                     */
/*                                                                            */
/******************************************************************************/
/* Bit definition for Ethernet MAC Control Register register */
#define ETH_MACCR_WD_Pos                              (23U)                    
#define ETH_MACCR_WD_Msk                              (0x1U << ETH_MACCR_WD_Pos) /*!< 0x00800000 */
#define ETH_MACCR_WD                                  ETH_MACCR_WD_Msk         /* Watchdog disable */
#define ETH_MACCR_JD_Pos                              (22U)                    
#define ETH_MACCR_JD_Msk                              (0x1U << ETH_MACCR_JD_Pos) /*!< 0x00400000 */
#define ETH_MACCR_JD                                  ETH_MACCR_JD_Msk         /* Jabber disable */
#define ETH_MACCR_IFG_Pos                             (17U)                    
#define ETH_MACCR_IFG_Msk                             (0x7U << ETH_MACCR_IFG_Pos) /*!< 0x000E0000 */
#define ETH_MACCR_IFG                                 ETH_MACCR_IFG_Msk        /* Inter-frame gap */
#define ETH_MACCR_IFG_96Bit                           0x00000000U              /* Minimum IFG between frames during transmission is 96Bit */
#define ETH_MACCR_IFG_88Bit                           0x00020000U              /* Minimum IFG between frames during transmission is 88Bit */
#define ETH_MACCR_IFG_80Bit                           0x00040000U              /* Minimum IFG between frames during transmission is 80Bit */
#define ETH_MACCR_IFG_72Bit                           0x00060000U              /* Minimum IFG between frames during transmission is 72Bit */
#define ETH_MACCR_IFG_64Bit                           0x00080000U              /* Minimum IFG between frames during transmission is 64Bit */
#define ETH_MACCR_IFG_56Bit                           0x000A0000U              /* Minimum IFG between frames during transmission is 56Bit */
#define ETH_MACCR_IFG_48Bit                           0x000C0000U              /* Minimum IFG between frames during transmission is 48Bit */
#define ETH_MACCR_IFG_40Bit                           0x000E0000U              /* Minimum IFG between frames during transmission is 40Bit */
#define ETH_MACCR_CSD_Pos                             (16U)                    
#define ETH_MACCR_CSD_Msk                             (0x1U << ETH_MACCR_CSD_Pos) /*!< 0x00010000 */
#define ETH_MACCR_CSD                                 ETH_MACCR_CSD_Msk        /* Carrier sense disable (during transmission) */
#define ETH_MACCR_FES_Pos                             (14U)                    
#define ETH_MACCR_FES_Msk                             (0x1U << ETH_MACCR_FES_Pos) /*!< 0x00004000 */
#define ETH_MACCR_FES                                 ETH_MACCR_FES_Msk        /* Fast ethernet speed */
#define ETH_MACCR_ROD_Pos                             (13U)                    
#define ETH_MACCR_ROD_Msk                             (0x1U << ETH_MACCR_ROD_Pos) /*!< 0x00002000 */
#define ETH_MACCR_ROD                                 ETH_MACCR_ROD_Msk        /* Receive own disable */
#define ETH_MACCR_LM_Pos                              (12U)                    
#define ETH_MACCR_LM_Msk                              (0x1U << ETH_MACCR_LM_Pos) /*!< 0x00001000 */
#define ETH_MACCR_LM                                  ETH_MACCR_LM_Msk         /* loopback mode */
#define ETH_MACCR_DM_Pos                              (11U)                    
#define ETH_MACCR_DM_Msk                              (0x1U << ETH_MACCR_DM_Pos) /*!< 0x00000800 */
#define ETH_MACCR_DM                                  ETH_MACCR_DM_Msk         /* Duplex mode */
#define ETH_MACCR_IPCO_Pos                            (10U)                    
#define ETH_MACCR_IPCO_Msk                            (0x1U << ETH_MACCR_IPCO_Pos) /*!< 0x00000400 */
#define ETH_MACCR_IPCO                                ETH_MACCR_IPCO_Msk       /* IP Checksum offload */
#define ETH_MACCR_RD_Pos                              (9U)                     
#define ETH_MACCR_RD_Msk                              (0x1U << ETH_MACCR_RD_Pos) /*!< 0x00000200 */
#define ETH_MACCR_RD                                  ETH_MACCR_RD_Msk         /* Retry disable */
#define ETH_MACCR_APCS_Pos                            (7U)                     
#define ETH_MACCR_APCS_Msk                            (0x1U << ETH_MACCR_APCS_Pos) /*!< 0x00000080 */
#define ETH_MACCR_APCS                                ETH_MACCR_APCS_Msk       /* Automatic Pad/CRC stripping */
#define ETH_MACCR_BL_Pos                              (5U)                     
#define ETH_MACCR_BL_Msk                              (0x3U << ETH_MACCR_BL_Pos) /*!< 0x00000060 */
#define ETH_MACCR_BL                                  ETH_MACCR_BL_Msk         /* Back-off limit: random integer number (r) of slot time delays before rescheduling
                                                       a transmission attempt during retries after a collision: 0 =< r <2^k */
#define ETH_MACCR_BL_10                               0x00000000U              /* k = min (n, 10) */
#define ETH_MACCR_BL_8                                0x00000020U              /* k = min (n, 8) */
#define ETH_MACCR_BL_4                                0x00000040U              /* k = min (n, 4) */
#define ETH_MACCR_BL_1                                0x00000060U              /* k = min (n, 1) */ 
#define ETH_MACCR_DC_Pos                              (4U)                     
#define ETH_MACCR_DC_Msk                              (0x1U << ETH_MACCR_DC_Pos) /*!< 0x00000010 */
#define ETH_MACCR_DC                                  ETH_MACCR_DC_Msk         /* Defferal check */
#define ETH_MACCR_TE_Pos                              (3U)                     
#define ETH_MACCR_TE_Msk                              (0x1U << ETH_MACCR_TE_Pos) /*!< 0x00000008 */
#define ETH_MACCR_TE                                  ETH_MACCR_TE_Msk         /* Transmitter enable */
#define ETH_MACCR_RE_Pos                              (2U)                     
#define ETH_MACCR_RE_Msk                              (0x1U << ETH_MACCR_RE_Pos) /*!< 0x00000004 */
#define ETH_MACCR_RE                                  ETH_MACCR_RE_Msk         /* Receiver enable */

/* Bit definition for Ethernet MAC Frame Filter Register */
#define ETH_MACFFR_RA_Pos                             (31U)                    
#define ETH_MACFFR_RA_Msk                             (0x1U << ETH_MACFFR_RA_Pos) /*!< 0x80000000 */
#define ETH_MACFFR_RA                                 ETH_MACFFR_RA_Msk        /* Receive all */
#define ETH_MACFFR_HPF_Pos                            (10U)                    
#define ETH_MACFFR_HPF_Msk                            (0x1U << ETH_MACFFR_HPF_Pos) /*!< 0x00000400 */
#define ETH_MACFFR_HPF                                ETH_MACFFR_HPF_Msk       /* Hash or perfect filter */
#define ETH_MACFFR_SAF_Pos                            (9U)                     
#define ETH_MACFFR_SAF_Msk                            (0x1U << ETH_MACFFR_SAF_Pos) /*!< 0x00000200 */
#define ETH_MACFFR_SAF                                ETH_MACFFR_SAF_Msk       /* Source address filter enable */
#define ETH_MACFFR_SAIF_Pos                           (8U)                     
#define ETH_MACFFR_SAIF_Msk                           (0x1U << ETH_MACFFR_SAIF_Pos) /*!< 0x00000100 */
#define ETH_MACFFR_SAIF                               ETH_MACFFR_SAIF_Msk      /* SA inverse filtering */ 
#define ETH_MACFFR_PCF_Pos                            (6U)                     
#define ETH_MACFFR_PCF_Msk                            (0x3U << ETH_MACFFR_PCF_Pos) /*!< 0x000000C0 */
#define ETH_MACFFR_PCF                                ETH_MACFFR_PCF_Msk       /* Pass control frames: 3 cases */
#define ETH_MACFFR_PCF_BlockAll_Pos                   (6U)                     
#define ETH_MACFFR_PCF_BlockAll_Msk                   (0x1U << ETH_MACFFR_PCF_BlockAll_Pos) /*!< 0x00000040 */
#define ETH_MACFFR_PCF_BlockAll                       ETH_MACFFR_PCF_BlockAll_Msk /* MAC filters all control frames from reaching the application */
#define ETH_MACFFR_PCF_ForwardAll_Pos                 (7U)                     
#define ETH_MACFFR_PCF_ForwardAll_Msk                 (0x1U << ETH_MACFFR_PCF_ForwardAll_Pos) /*!< 0x00000080 */
#define ETH_MACFFR_PCF_ForwardAll                     ETH_MACFFR_PCF_ForwardAll_Msk /* MAC forwards all control frames to application even if they fail the Address Filter */
#define ETH_MACFFR_PCF_ForwardPassedAddrFilter_Pos    (6U)                     
#define ETH_MACFFR_PCF_ForwardPassedAddrFilter_Msk    (0x3U << ETH_MACFFR_PCF_ForwardPassedAddrFilter_Pos) /*!< 0x000000C0 */
#define ETH_MACFFR_PCF_ForwardPassedAddrFilter        ETH_MACFFR_PCF_ForwardPassedAddrFilter_Msk /* MAC forwards control frames that pass the Address Filter. */ 
#define ETH_MACFFR_BFD_Pos                            (5U)                     
#define ETH_MACFFR_BFD_Msk                            (0x1U << ETH_MACFFR_BFD_Pos) /*!< 0x00000020 */
#define ETH_MACFFR_BFD                                ETH_MACFFR_BFD_Msk       /* Broadcast frame disable */ 
#define ETH_MACFFR_PAM_Pos                            (4U)                     
#define ETH_MACFFR_PAM_Msk                            (0x1U << ETH_MACFFR_PAM_Pos) /*!< 0x00000010 */
#define ETH_MACFFR_PAM                                ETH_MACFFR_PAM_Msk       /* Pass all mutlicast */
#define ETH_MACFFR_DAIF_Pos                           (3U)                     
#define ETH_MACFFR_DAIF_Msk                           (0x1U << ETH_MACFFR_DAIF_Pos) /*!< 0x00000008 */
#define ETH_MACFFR_DAIF                               ETH_MACFFR_DAIF_Msk      /* DA Inverse filtering */
#define ETH_MACFFR_HM_Pos                             (2U)                     
#define ETH_MACFFR_HM_Msk                             (0x1U << ETH_MACFFR_HM_Pos) /*!< 0x00000004 */
#define ETH_MACFFR_HM                                 ETH_MACFFR_HM_Msk        /* Hash multicast */ 
#define ETH_MACFFR_HU_Pos                             (1U)                     
#define ETH_MACFFR_HU_Msk                             (0x1U << ETH_MACFFR_HU_Pos) /*!< 0x00000002 */
#define ETH_MACFFR_HU                                 ETH_MACFFR_HU_Msk        /* Hash unicast */
#define ETH_MACFFR_PM_Pos                             (0U)                     
#define ETH_MACFFR_PM_Msk                             (0x1U << ETH_MACFFR_PM_Pos) /*!< 0x00000001 */
#define ETH_MACFFR_PM                                 ETH_MACFFR_PM_Msk        /* Promiscuous mode */

/* Bit definition for Ethernet MAC Hash Table High Register */
#define ETH_MACHTHR_HTH_Pos                           (0U)                     
#define ETH_MACHTHR_HTH_Msk                           (0xFFFFFFFFU << ETH_MACHTHR_HTH_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACHTHR_HTH                               ETH_MACHTHR_HTH_Msk      /* Hash table high */

/* Bit definition for Ethernet MAC Hash Table Low Register */
#define ETH_MACHTLR_HTL_Pos                           (0U)                     
#define ETH_MACHTLR_HTL_Msk                           (0xFFFFFFFFU << ETH_MACHTLR_HTL_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACHTLR_HTL                               ETH_MACHTLR_HTL_Msk      /* Hash table low */

/* Bit definition for Ethernet MAC MII Address Register */
#define ETH_MACMIIAR_PA_Pos                           (11U)                    
#define ETH_MACMIIAR_PA_Msk                           (0x1FU << ETH_MACMIIAR_PA_Pos) /*!< 0x0000F800 */
#define ETH_MACMIIAR_PA                               ETH_MACMIIAR_PA_Msk      /* Physical layer address */
#define ETH_MACMIIAR_MR_Pos                           (6U)                     
#define ETH_MACMIIAR_MR_Msk                           (0x1FU << ETH_MACMIIAR_MR_Pos) /*!< 0x000007C0 */
#define ETH_MACMIIAR_MR                               ETH_MACMIIAR_MR_Msk      /* MII register in the selected PHY */
#define ETH_MACMIIAR_CR_Pos                           (2U)                     
#define ETH_MACMIIAR_CR_Msk                           (0x7U << ETH_MACMIIAR_CR_Pos) /*!< 0x0000001C */
#define ETH_MACMIIAR_CR                               ETH_MACMIIAR_CR_Msk      /* CR clock range: 6 cases */
#define ETH_MACMIIAR_CR_Div42                         0x00000000U              /* HCLK:60-100 MHz; MDC clock= HCLK/42   */
#define ETH_MACMIIAR_CR_Div62_Pos                     (2U)                     
#define ETH_MACMIIAR_CR_Div62_Msk                     (0x1U << ETH_MACMIIAR_CR_Div62_Pos) /*!< 0x00000004 */
#define ETH_MACMIIAR_CR_Div62                         ETH_MACMIIAR_CR_Div62_Msk /* HCLK:100-150 MHz; MDC clock= HCLK/62  */
#define ETH_MACMIIAR_CR_Div16_Pos                     (3U)                     
#define ETH_MACMIIAR_CR_Div16_Msk                     (0x1U << ETH_MACMIIAR_CR_Div16_Pos) /*!< 0x00000008 */
#define ETH_MACMIIAR_CR_Div16                         ETH_MACMIIAR_CR_Div16_Msk /* HCLK:20-35 MHz; MDC clock= HCLK/16    */
#define ETH_MACMIIAR_CR_Div26_Pos                     (2U)                     
#define ETH_MACMIIAR_CR_Div26_Msk                     (0x3U << ETH_MACMIIAR_CR_Div26_Pos) /*!< 0x0000000C */
#define ETH_MACMIIAR_CR_Div26                         ETH_MACMIIAR_CR_Div26_Msk /* HCLK:35-60 MHz; MDC clock= HCLK/26    */
#define ETH_MACMIIAR_CR_Div102_Pos                    (4U)                     
#define ETH_MACMIIAR_CR_Div102_Msk                    (0x1U << ETH_MACMIIAR_CR_Div102_Pos) /*!< 0x00000010 */
#define ETH_MACMIIAR_CR_Div102                        ETH_MACMIIAR_CR_Div102_Msk /* HCLK:150-168 MHz; MDC clock= HCLK/102 */
#define ETH_MACMIIAR_MW_Pos                           (1U)                     
#define ETH_MACMIIAR_MW_Msk                           (0x1U << ETH_MACMIIAR_MW_Pos) /*!< 0x00000002 */
#define ETH_MACMIIAR_MW                               ETH_MACMIIAR_MW_Msk      /* MII write */
#define ETH_MACMIIAR_MB_Pos                           (0U)                     
#define ETH_MACMIIAR_MB_Msk                           (0x1U << ETH_MACMIIAR_MB_Pos) /*!< 0x00000001 */
#define ETH_MACMIIAR_MB                               ETH_MACMIIAR_MB_Msk      /* MII busy  */

/* Bit definition for Ethernet MAC MII Data Register */
#define ETH_MACMIIDR_MD_Pos                           (0U)                     
#define ETH_MACMIIDR_MD_Msk                           (0xFFFFU << ETH_MACMIIDR_MD_Pos) /*!< 0x0000FFFF */
#define ETH_MACMIIDR_MD                               ETH_MACMIIDR_MD_Msk      /* MII data: read/write data from/to PHY */

/* Bit definition for Ethernet MAC Flow Control Register */
#define ETH_MACFCR_PT_Pos                             (16U)                    
#define ETH_MACFCR_PT_Msk                             (0xFFFFU << ETH_MACFCR_PT_Pos) /*!< 0xFFFF0000 */
#define ETH_MACFCR_PT                                 ETH_MACFCR_PT_Msk        /* Pause time */
#define ETH_MACFCR_ZQPD_Pos                           (7U)                     
#define ETH_MACFCR_ZQPD_Msk                           (0x1U << ETH_MACFCR_ZQPD_Pos) /*!< 0x00000080 */
#define ETH_MACFCR_ZQPD                               ETH_MACFCR_ZQPD_Msk      /* Zero-quanta pause disable */
#define ETH_MACFCR_PLT_Pos                            (4U)                     
#define ETH_MACFCR_PLT_Msk                            (0x3U << ETH_MACFCR_PLT_Pos) /*!< 0x00000030 */
#define ETH_MACFCR_PLT                                ETH_MACFCR_PLT_Msk       /* Pause low threshold: 4 cases */
#define ETH_MACFCR_PLT_Minus4                         0x00000000U              /* Pause time minus 4 slot times   */
#define ETH_MACFCR_PLT_Minus28_Pos                    (4U)                     
#define ETH_MACFCR_PLT_Minus28_Msk                    (0x1U << ETH_MACFCR_PLT_Minus28_Pos) /*!< 0x00000010 */
#define ETH_MACFCR_PLT_Minus28                        ETH_MACFCR_PLT_Minus28_Msk /* Pause time minus 28 slot times  */
#define ETH_MACFCR_PLT_Minus144_Pos                   (5U)                     
#define ETH_MACFCR_PLT_Minus144_Msk                   (0x1U << ETH_MACFCR_PLT_Minus144_Pos) /*!< 0x00000020 */
#define ETH_MACFCR_PLT_Minus144                       ETH_MACFCR_PLT_Minus144_Msk /* Pause time minus 144 slot times */
#define ETH_MACFCR_PLT_Minus256_Pos                   (4U)                     
#define ETH_MACFCR_PLT_Minus256_Msk                   (0x3U << ETH_MACFCR_PLT_Minus256_Pos) /*!< 0x00000030 */
#define ETH_MACFCR_PLT_Minus256                       ETH_MACFCR_PLT_Minus256_Msk /* Pause time minus 256 slot times */
#define ETH_MACFCR_UPFD_Pos                           (3U)                     
#define ETH_MACFCR_UPFD_Msk                           (0x1U << ETH_MACFCR_UPFD_Pos) /*!< 0x00000008 */
#define ETH_MACFCR_UPFD                               ETH_MACFCR_UPFD_Msk      /* Unicast pause frame detect */
#define ETH_MACFCR_RFCE_Pos                           (2U)                     
#define ETH_MACFCR_RFCE_Msk                           (0x1U << ETH_MACFCR_RFCE_Pos) /*!< 0x00000004 */
#define ETH_MACFCR_RFCE                               ETH_MACFCR_RFCE_Msk      /* Receive flow control enable */
#define ETH_MACFCR_TFCE_Pos                           (1U)                     
#define ETH_MACFCR_TFCE_Msk                           (0x1U << ETH_MACFCR_TFCE_Pos) /*!< 0x00000002 */
#define ETH_MACFCR_TFCE                               ETH_MACFCR_TFCE_Msk      /* Transmit flow control enable */
#define ETH_MACFCR_FCBBPA_Pos                         (0U)                     
#define ETH_MACFCR_FCBBPA_Msk                         (0x1U << ETH_MACFCR_FCBBPA_Pos) /*!< 0x00000001 */
#define ETH_MACFCR_FCBBPA                             ETH_MACFCR_FCBBPA_Msk    /* Flow control busy/backpressure activate */

/* Bit definition for Ethernet MAC VLAN Tag Register */
#define ETH_MACVLANTR_VLANTC_Pos                      (16U)                    
#define ETH_MACVLANTR_VLANTC_Msk                      (0x1U << ETH_MACVLANTR_VLANTC_Pos) /*!< 0x00010000 */
#define ETH_MACVLANTR_VLANTC                          ETH_MACVLANTR_VLANTC_Msk /* 12-bit VLAN tag comparison */
#define ETH_MACVLANTR_VLANTI_Pos                      (0U)                     
#define ETH_MACVLANTR_VLANTI_Msk                      (0xFFFFU << ETH_MACVLANTR_VLANTI_Pos) /*!< 0x0000FFFF */
#define ETH_MACVLANTR_VLANTI                          ETH_MACVLANTR_VLANTI_Msk /* VLAN tag identifier (for receive frames) */

/* Bit definition for Ethernet MAC Remote Wake-UpFrame Filter Register */ 
#define ETH_MACRWUFFR_D_Pos                           (0U)                     
#define ETH_MACRWUFFR_D_Msk                           (0xFFFFFFFFU << ETH_MACRWUFFR_D_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACRWUFFR_D                               ETH_MACRWUFFR_D_Msk      /* Wake-up frame filter register data */
/* Eight sequential Writes to this address (offset 0x28) will write all Wake-UpFrame Filter Registers.
   Eight sequential Reads from this address (offset 0x28) will read all Wake-UpFrame Filter Registers. */
/* Wake-UpFrame Filter Reg0 : Filter 0 Byte Mask
   Wake-UpFrame Filter Reg1 : Filter 1 Byte Mask
   Wake-UpFrame Filter Reg2 : Filter 2 Byte Mask
   Wake-UpFrame Filter Reg3 : Filter 3 Byte Mask
   Wake-UpFrame Filter Reg4 : RSVD - Filter3 Command - RSVD - Filter2 Command - 
                              RSVD - Filter1 Command - RSVD - Filter0 Command
   Wake-UpFrame Filter Re5 : Filter3 Offset - Filter2 Offset - Filter1 Offset - Filter0 Offset
   Wake-UpFrame Filter Re6 : Filter1 CRC16 - Filter0 CRC16
   Wake-UpFrame Filter Re7 : Filter3 CRC16 - Filter2 CRC16 */

/* Bit definition for Ethernet MAC PMT Control and Status Register */ 
#define ETH_MACPMTCSR_WFFRPR_Pos                      (31U)                    
#define ETH_MACPMTCSR_WFFRPR_Msk                      (0x1U << ETH_MACPMTCSR_WFFRPR_Pos) /*!< 0x80000000 */
#define ETH_MACPMTCSR_WFFRPR                          ETH_MACPMTCSR_WFFRPR_Msk /* Wake-Up Frame Filter Register Pointer Reset */
#define ETH_MACPMTCSR_GU_Pos                          (9U)                     
#define ETH_MACPMTCSR_GU_Msk                          (0x1U << ETH_MACPMTCSR_GU_Pos) /*!< 0x00000200 */
#define ETH_MACPMTCSR_GU                              ETH_MACPMTCSR_GU_Msk     /* Global Unicast                              */
#define ETH_MACPMTCSR_WFR_Pos                         (6U)                     
#define ETH_MACPMTCSR_WFR_Msk                         (0x1U << ETH_MACPMTCSR_WFR_Pos) /*!< 0x00000040 */
#define ETH_MACPMTCSR_WFR                             ETH_MACPMTCSR_WFR_Msk    /* Wake-Up Frame Received                      */
#define ETH_MACPMTCSR_MPR_Pos                         (5U)                     
#define ETH_MACPMTCSR_MPR_Msk                         (0x1U << ETH_MACPMTCSR_MPR_Pos) /*!< 0x00000020 */
#define ETH_MACPMTCSR_MPR                             ETH_MACPMTCSR_MPR_Msk    /* Magic Packet Received                       */
#define ETH_MACPMTCSR_WFE_Pos                         (2U)                     
#define ETH_MACPMTCSR_WFE_Msk                         (0x1U << ETH_MACPMTCSR_WFE_Pos) /*!< 0x00000004 */
#define ETH_MACPMTCSR_WFE                             ETH_MACPMTCSR_WFE_Msk    /* Wake-Up Frame Enable                        */
#define ETH_MACPMTCSR_MPE_Pos                         (1U)                     
#define ETH_MACPMTCSR_MPE_Msk                         (0x1U << ETH_MACPMTCSR_MPE_Pos) /*!< 0x00000002 */
#define ETH_MACPMTCSR_MPE                             ETH_MACPMTCSR_MPE_Msk    /* Magic Packet Enable                         */
#define ETH_MACPMTCSR_PD_Pos                          (0U)                     
#define ETH_MACPMTCSR_PD_Msk                          (0x1U << ETH_MACPMTCSR_PD_Pos) /*!< 0x00000001 */
#define ETH_MACPMTCSR_PD                              ETH_MACPMTCSR_PD_Msk     /* Power Down                                  */

/* Bit definition for Ethernet MAC debug Register */
#define ETH_MACDBGR_TFF_Pos                           (25U)                    
#define ETH_MACDBGR_TFF_Msk                           (0x1U << ETH_MACDBGR_TFF_Pos) /*!< 0x02000000 */
#define ETH_MACDBGR_TFF                               ETH_MACDBGR_TFF_Msk      /* Tx FIFO full                                                            */
#define ETH_MACDBGR_TFNE_Pos                          (24U)                    
#define ETH_MACDBGR_TFNE_Msk                          (0x1U << ETH_MACDBGR_TFNE_Pos) /*!< 0x01000000 */
#define ETH_MACDBGR_TFNE                              ETH_MACDBGR_TFNE_Msk     /* Tx FIFO not empty                                                       */
#define ETH_MACDBGR_TFWA_Pos                          (22U)                    
#define ETH_MACDBGR_TFWA_Msk                          (0x1U << ETH_MACDBGR_TFWA_Pos) /*!< 0x00400000 */
#define ETH_MACDBGR_TFWA                              ETH_MACDBGR_TFWA_Msk     /* Tx FIFO write active                                                    */
#define ETH_MACDBGR_TFRS_Pos                          (20U)                    
#define ETH_MACDBGR_TFRS_Msk                          (0x3U << ETH_MACDBGR_TFRS_Pos) /*!< 0x00300000 */
#define ETH_MACDBGR_TFRS                              ETH_MACDBGR_TFRS_Msk     /* Tx FIFO read status mask                                                */
#define ETH_MACDBGR_TFRS_WRITING_Pos                  (20U)                    
#define ETH_MACDBGR_TFRS_WRITING_Msk                  (0x3U << ETH_MACDBGR_TFRS_WRITING_Pos) /*!< 0x00300000 */
#define ETH_MACDBGR_TFRS_WRITING                      ETH_MACDBGR_TFRS_WRITING_Msk /* Writing the received TxStatus or flushing the TxFIFO                    */
#define ETH_MACDBGR_TFRS_WAITING_Pos                  (21U)                    
#define ETH_MACDBGR_TFRS_WAITING_Msk                  (0x1U << ETH_MACDBGR_TFRS_WAITING_Pos) /*!< 0x00200000 */
#define ETH_MACDBGR_TFRS_WAITING                      ETH_MACDBGR_TFRS_WAITING_Msk /* Waiting for TxStatus from MAC transmitter                               */
#define ETH_MACDBGR_TFRS_READ_Pos                     (20U)                    
#define ETH_MACDBGR_TFRS_READ_Msk                     (0x1U << ETH_MACDBGR_TFRS_READ_Pos) /*!< 0x00100000 */
#define ETH_MACDBGR_TFRS_READ                         ETH_MACDBGR_TFRS_READ_Msk /* Read state (transferring data to the MAC transmitter)                   */
#define ETH_MACDBGR_TFRS_IDLE                         0x00000000U              /* Idle state                                                              */
#define ETH_MACDBGR_MTP_Pos                           (19U)                    
#define ETH_MACDBGR_MTP_Msk                           (0x1U << ETH_MACDBGR_MTP_Pos) /*!< 0x00080000 */
#define ETH_MACDBGR_MTP                               ETH_MACDBGR_MTP_Msk      /* MAC transmitter in pause                                                */
#define ETH_MACDBGR_MTFCS_Pos                         (17U)                    
#define ETH_MACDBGR_MTFCS_Msk                         (0x3U << ETH_MACDBGR_MTFCS_Pos) /*!< 0x00060000 */
#define ETH_MACDBGR_MTFCS                             ETH_MACDBGR_MTFCS_Msk    /* MAC transmit frame controller status mask                               */
#define ETH_MACDBGR_MTFCS_TRANSFERRING_Pos            (17U)                    
#define ETH_MACDBGR_MTFCS_TRANSFERRING_Msk            (0x3U << ETH_MACDBGR_MTFCS_TRANSFERRING_Pos) /*!< 0x00060000 */
#define ETH_MACDBGR_MTFCS_TRANSFERRING                ETH_MACDBGR_MTFCS_TRANSFERRING_Msk /* Transferring input frame for transmission                               */
#define ETH_MACDBGR_MTFCS_GENERATINGPCF_Pos           (18U)                    
#define ETH_MACDBGR_MTFCS_GENERATINGPCF_Msk           (0x1U << ETH_MACDBGR_MTFCS_GENERATINGPCF_Pos) /*!< 0x00040000 */
#define ETH_MACDBGR_MTFCS_GENERATINGPCF               ETH_MACDBGR_MTFCS_GENERATINGPCF_Msk /* Generating and transmitting a Pause control frame (in full duplex mode) */
#define ETH_MACDBGR_MTFCS_WAITING_Pos                 (17U)                    
#define ETH_MACDBGR_MTFCS_WAITING_Msk                 (0x1U << ETH_MACDBGR_MTFCS_WAITING_Pos) /*!< 0x00020000 */
#define ETH_MACDBGR_MTFCS_WAITING                     ETH_MACDBGR_MTFCS_WAITING_Msk /* Waiting for Status of previous frame or IFG/backoff period to be over   */
#define ETH_MACDBGR_MTFCS_IDLE                        0x00000000U              /* Idle                                                                    */
#define ETH_MACDBGR_MMTEA_Pos                         (16U)                    
#define ETH_MACDBGR_MMTEA_Msk                         (0x1U << ETH_MACDBGR_MMTEA_Pos) /*!< 0x00010000 */
#define ETH_MACDBGR_MMTEA                             ETH_MACDBGR_MMTEA_Msk    /* MAC MII transmit engine active                                          */
#define ETH_MACDBGR_RFFL_Pos                          (8U)                     
#define ETH_MACDBGR_RFFL_Msk                          (0x3U << ETH_MACDBGR_RFFL_Pos) /*!< 0x00000300 */
#define ETH_MACDBGR_RFFL                              ETH_MACDBGR_RFFL_Msk     /* Rx FIFO fill level mask                                                 */
#define ETH_MACDBGR_RFFL_FULL_Pos                     (8U)                     
#define ETH_MACDBGR_RFFL_FULL_Msk                     (0x3U << ETH_MACDBGR_RFFL_FULL_Pos) /*!< 0x00000300 */
#define ETH_MACDBGR_RFFL_FULL                         ETH_MACDBGR_RFFL_FULL_Msk /* RxFIFO full                                                             */
#define ETH_MACDBGR_RFFL_ABOVEFCT_Pos                 (9U)                     
#define ETH_MACDBGR_RFFL_ABOVEFCT_Msk                 (0x1U << ETH_MACDBGR_RFFL_ABOVEFCT_Pos) /*!< 0x00000200 */
#define ETH_MACDBGR_RFFL_ABOVEFCT                     ETH_MACDBGR_RFFL_ABOVEFCT_Msk /* RxFIFO fill-level above flow-control activate threshold                 */
#define ETH_MACDBGR_RFFL_BELOWFCT_Pos                 (8U)                     
#define ETH_MACDBGR_RFFL_BELOWFCT_Msk                 (0x1U << ETH_MACDBGR_RFFL_BELOWFCT_Pos) /*!< 0x00000100 */
#define ETH_MACDBGR_RFFL_BELOWFCT                     ETH_MACDBGR_RFFL_BELOWFCT_Msk /* RxFIFO fill-level below flow-control de-activate threshold              */
#define ETH_MACDBGR_RFFL_EMPTY                        0x00000000U              /* RxFIFO empty                                                            */
#define ETH_MACDBGR_RFRCS_Pos                         (5U)                     
#define ETH_MACDBGR_RFRCS_Msk                         (0x3U << ETH_MACDBGR_RFRCS_Pos) /*!< 0x00000060 */
#define ETH_MACDBGR_RFRCS                             ETH_MACDBGR_RFRCS_Msk    /* Rx FIFO read controller status mask                                     */
#define ETH_MACDBGR_RFRCS_FLUSHING_Pos                (5U)                     
#define ETH_MACDBGR_RFRCS_FLUSHING_Msk                (0x3U << ETH_MACDBGR_RFRCS_FLUSHING_Pos) /*!< 0x00000060 */
#define ETH_MACDBGR_RFRCS_FLUSHING                    ETH_MACDBGR_RFRCS_FLUSHING_Msk /* Flushing the frame data and status                                      */
#define ETH_MACDBGR_RFRCS_STATUSREADING_Pos           (6U)                     
#define ETH_MACDBGR_RFRCS_STATUSREADING_Msk           (0x1U << ETH_MACDBGR_RFRCS_STATUSREADING_Pos) /*!< 0x00000040 */
#define ETH_MACDBGR_RFRCS_STATUSREADING               ETH_MACDBGR_RFRCS_STATUSREADING_Msk /* Reading frame status (or time-stamp)                                    */
#define ETH_MACDBGR_RFRCS_DATAREADING_Pos             (5U)                     
#define ETH_MACDBGR_RFRCS_DATAREADING_Msk             (0x1U << ETH_MACDBGR_RFRCS_DATAREADING_Pos) /*!< 0x00000020 */
#define ETH_MACDBGR_RFRCS_DATAREADING                 ETH_MACDBGR_RFRCS_DATAREADING_Msk /* Reading frame data                                                      */
#define ETH_MACDBGR_RFRCS_IDLE                        0x00000000U              /* IDLE state                                                              */
#define ETH_MACDBGR_RFWRA_Pos                         (4U)                     
#define ETH_MACDBGR_RFWRA_Msk                         (0x1U << ETH_MACDBGR_RFWRA_Pos) /*!< 0x00000010 */
#define ETH_MACDBGR_RFWRA                             ETH_MACDBGR_RFWRA_Msk    /* Rx FIFO write controller active                                         */
#define ETH_MACDBGR_MSFRWCS_Pos                       (1U)                     
#define ETH_MACDBGR_MSFRWCS_Msk                       (0x3U << ETH_MACDBGR_MSFRWCS_Pos) /*!< 0x00000006 */
#define ETH_MACDBGR_MSFRWCS                           ETH_MACDBGR_MSFRWCS_Msk  /* MAC small FIFO read / write controllers status  mask                    */
#define ETH_MACDBGR_MSFRWCS_1                         (0x2U << ETH_MACDBGR_MSFRWCS_Pos) /*!< 0x00000004 */
#define ETH_MACDBGR_MSFRWCS_0                         (0x1U << ETH_MACDBGR_MSFRWCS_Pos) /*!< 0x00000002 */
#define ETH_MACDBGR_MMRPEA_Pos                        (0U)                     
#define ETH_MACDBGR_MMRPEA_Msk                        (0x1U << ETH_MACDBGR_MMRPEA_Pos) /*!< 0x00000001 */
#define ETH_MACDBGR_MMRPEA                            ETH_MACDBGR_MMRPEA_Msk   /* MAC MII receive protocol engine active                                  */

/* Bit definition for Ethernet MAC Status Register */
#define ETH_MACSR_TSTS_Pos                            (9U)                     
#define ETH_MACSR_TSTS_Msk                            (0x1U << ETH_MACSR_TSTS_Pos) /*!< 0x00000200 */
#define ETH_MACSR_TSTS                                ETH_MACSR_TSTS_Msk       /* Time stamp trigger status */
#define ETH_MACSR_MMCTS_Pos                           (6U)                     
#define ETH_MACSR_MMCTS_Msk                           (0x1U << ETH_MACSR_MMCTS_Pos) /*!< 0x00000040 */
#define ETH_MACSR_MMCTS                               ETH_MACSR_MMCTS_Msk      /* MMC transmit status       */
#define ETH_MACSR_MMMCRS_Pos                          (5U)                     
#define ETH_MACSR_MMMCRS_Msk                          (0x1U << ETH_MACSR_MMMCRS_Pos) /*!< 0x00000020 */
#define ETH_MACSR_MMMCRS                              ETH_MACSR_MMMCRS_Msk     /* MMC receive status        */
#define ETH_MACSR_MMCS_Pos                            (4U)                     
#define ETH_MACSR_MMCS_Msk                            (0x1U << ETH_MACSR_MMCS_Pos) /*!< 0x00000010 */
#define ETH_MACSR_MMCS                                ETH_MACSR_MMCS_Msk       /* MMC status                */
#define ETH_MACSR_PMTS_Pos                            (3U)                     
#define ETH_MACSR_PMTS_Msk                            (0x1U << ETH_MACSR_PMTS_Pos) /*!< 0x00000008 */
#define ETH_MACSR_PMTS                                ETH_MACSR_PMTS_Msk       /* PMT status                */

/* Bit definition for Ethernet MAC Interrupt Mask Register */
#define ETH_MACIMR_TSTIM_Pos                          (9U)                     
#define ETH_MACIMR_TSTIM_Msk                          (0x1U << ETH_MACIMR_TSTIM_Pos) /*!< 0x00000200 */
#define ETH_MACIMR_TSTIM                              ETH_MACIMR_TSTIM_Msk     /* Time stamp trigger interrupt mask */
#define ETH_MACIMR_PMTIM_Pos                          (3U)                     
#define ETH_MACIMR_PMTIM_Msk                          (0x1U << ETH_MACIMR_PMTIM_Pos) /*!< 0x00000008 */
#define ETH_MACIMR_PMTIM                              ETH_MACIMR_PMTIM_Msk     /* PMT interrupt mask                */

/* Bit definition for Ethernet MAC Address0 High Register */
#define ETH_MACA0HR_MACA0H_Pos                        (0U)                     
#define ETH_MACA0HR_MACA0H_Msk                        (0xFFFFU << ETH_MACA0HR_MACA0H_Pos) /*!< 0x0000FFFF */
#define ETH_MACA0HR_MACA0H                            ETH_MACA0HR_MACA0H_Msk   /* MAC address0 high */

/* Bit definition for Ethernet MAC Address0 Low Register */
#define ETH_MACA0LR_MACA0L_Pos                        (0U)                     
#define ETH_MACA0LR_MACA0L_Msk                        (0xFFFFFFFFU << ETH_MACA0LR_MACA0L_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACA0LR_MACA0L                            ETH_MACA0LR_MACA0L_Msk   /* MAC address0 low */

/* Bit definition for Ethernet MAC Address1 High Register */
#define ETH_MACA1HR_AE_Pos                            (31U)                    
#define ETH_MACA1HR_AE_Msk                            (0x1U << ETH_MACA1HR_AE_Pos) /*!< 0x80000000 */
#define ETH_MACA1HR_AE                                ETH_MACA1HR_AE_Msk       /* Address enable */
#define ETH_MACA1HR_SA_Pos                            (30U)                    
#define ETH_MACA1HR_SA_Msk                            (0x1U << ETH_MACA1HR_SA_Pos) /*!< 0x40000000 */
#define ETH_MACA1HR_SA                                ETH_MACA1HR_SA_Msk       /* Source address */
#define ETH_MACA1HR_MBC_Pos                           (24U)                    
#define ETH_MACA1HR_MBC_Msk                           (0x3FU << ETH_MACA1HR_MBC_Pos) /*!< 0x3F000000 */
#define ETH_MACA1HR_MBC                               ETH_MACA1HR_MBC_Msk      /* Mask byte control: bits to mask for comparison of the MAC Address bytes */
#define ETH_MACA1HR_MBC_HBits15_8                     0x20000000U              /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA1HR_MBC_HBits7_0                      0x10000000U              /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA1HR_MBC_LBits31_24                    0x08000000U              /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA1HR_MBC_LBits23_16                    0x04000000U              /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA1HR_MBC_LBits15_8                     0x02000000U              /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA1HR_MBC_LBits7_0                      0x01000000U              /* Mask MAC Address low reg bits [7:0]   */
#define ETH_MACA1HR_MACA1H_Pos                        (0U)                     
#define ETH_MACA1HR_MACA1H_Msk                        (0xFFFFU << ETH_MACA1HR_MACA1H_Pos) /*!< 0x0000FFFF */
#define ETH_MACA1HR_MACA1H                            ETH_MACA1HR_MACA1H_Msk   /* MAC address1 high */

/* Bit definition for Ethernet MAC Address1 Low Register */
#define ETH_MACA1LR_MACA1L_Pos                        (0U)                     
#define ETH_MACA1LR_MACA1L_Msk                        (0xFFFFFFFFU << ETH_MACA1LR_MACA1L_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACA1LR_MACA1L                            ETH_MACA1LR_MACA1L_Msk   /* MAC address1 low */

/* Bit definition for Ethernet MAC Address2 High Register */
#define ETH_MACA2HR_AE_Pos                            (31U)                    
#define ETH_MACA2HR_AE_Msk                            (0x1U << ETH_MACA2HR_AE_Pos) /*!< 0x80000000 */
#define ETH_MACA2HR_AE                                ETH_MACA2HR_AE_Msk       /* Address enable */
#define ETH_MACA2HR_SA_Pos                            (30U)                    
#define ETH_MACA2HR_SA_Msk                            (0x1U << ETH_MACA2HR_SA_Pos) /*!< 0x40000000 */
#define ETH_MACA2HR_SA                                ETH_MACA2HR_SA_Msk       /* Source address */
#define ETH_MACA2HR_MBC_Pos                           (24U)                    
#define ETH_MACA2HR_MBC_Msk                           (0x3FU << ETH_MACA2HR_MBC_Pos) /*!< 0x3F000000 */
#define ETH_MACA2HR_MBC                               ETH_MACA2HR_MBC_Msk      /* Mask byte control */
#define ETH_MACA2HR_MBC_HBits15_8                     0x20000000U              /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA2HR_MBC_HBits7_0                      0x10000000U              /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA2HR_MBC_LBits31_24                    0x08000000U              /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA2HR_MBC_LBits23_16                    0x04000000U              /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA2HR_MBC_LBits15_8                     0x02000000U              /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA2HR_MBC_LBits7_0                      0x01000000U              /* Mask MAC Address low reg bits [70]    */
#define ETH_MACA2HR_MACA2H_Pos                        (0U)                     
#define ETH_MACA2HR_MACA2H_Msk                        (0xFFFFU << ETH_MACA2HR_MACA2H_Pos) /*!< 0x0000FFFF */
#define ETH_MACA2HR_MACA2H                            ETH_MACA2HR_MACA2H_Msk   /* MAC address1 high */

/* Bit definition for Ethernet MAC Address2 Low Register */
#define ETH_MACA2LR_MACA2L_Pos                        (0U)                     
#define ETH_MACA2LR_MACA2L_Msk                        (0xFFFFFFFFU << ETH_MACA2LR_MACA2L_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACA2LR_MACA2L                            ETH_MACA2LR_MACA2L_Msk   /* MAC address2 low */

/* Bit definition for Ethernet MAC Address3 High Register */
#define ETH_MACA3HR_AE_Pos                            (31U)                    
#define ETH_MACA3HR_AE_Msk                            (0x1U << ETH_MACA3HR_AE_Pos) /*!< 0x80000000 */
#define ETH_MACA3HR_AE                                ETH_MACA3HR_AE_Msk       /* Address enable */
#define ETH_MACA3HR_SA_Pos                            (30U)                    
#define ETH_MACA3HR_SA_Msk                            (0x1U << ETH_MACA3HR_SA_Pos) /*!< 0x40000000 */
#define ETH_MACA3HR_SA                                ETH_MACA3HR_SA_Msk       /* Source address */
#define ETH_MACA3HR_MBC_Pos                           (24U)                    
#define ETH_MACA3HR_MBC_Msk                           (0x3FU << ETH_MACA3HR_MBC_Pos) /*!< 0x3F000000 */
#define ETH_MACA3HR_MBC                               ETH_MACA3HR_MBC_Msk      /* Mask byte control */
#define ETH_MACA3HR_MBC_HBits15_8                     0x20000000U              /* Mask MAC Address high reg bits [15:8] */
#define ETH_MACA3HR_MBC_HBits7_0                      0x10000000U              /* Mask MAC Address high reg bits [7:0]  */
#define ETH_MACA3HR_MBC_LBits31_24                    0x08000000U              /* Mask MAC Address low reg bits [31:24] */
#define ETH_MACA3HR_MBC_LBits23_16                    0x04000000U              /* Mask MAC Address low reg bits [23:16] */
#define ETH_MACA3HR_MBC_LBits15_8                     0x02000000U              /* Mask MAC Address low reg bits [15:8]  */
#define ETH_MACA3HR_MBC_LBits7_0                      0x01000000U              /* Mask MAC Address low reg bits [70]    */
#define ETH_MACA3HR_MACA3H_Pos                        (0U)                     
#define ETH_MACA3HR_MACA3H_Msk                        (0xFFFFU << ETH_MACA3HR_MACA3H_Pos) /*!< 0x0000FFFF */
#define ETH_MACA3HR_MACA3H                            ETH_MACA3HR_MACA3H_Msk   /* MAC address3 high */

/* Bit definition for Ethernet MAC Address3 Low Register */
#define ETH_MACA3LR_MACA3L_Pos                        (0U)                     
#define ETH_MACA3LR_MACA3L_Msk                        (0xFFFFFFFFU << ETH_MACA3LR_MACA3L_Pos) /*!< 0xFFFFFFFF */
#define ETH_MACA3LR_MACA3L                            ETH_MACA3LR_MACA3L_Msk   /* MAC address3 low */

/******************************************************************************/
/*                Ethernet MMC Registers bits definition                      */
/******************************************************************************/

/* Bit definition for Ethernet MMC Contol Register */
#define ETH_MMCCR_MCFHP_Pos                           (5U)                     
#define ETH_MMCCR_MCFHP_Msk                           (0x1U << ETH_MMCCR_MCFHP_Pos) /*!< 0x00000020 */
#define ETH_MMCCR_MCFHP                               ETH_MMCCR_MCFHP_Msk      /* MMC counter Full-Half preset */
#define ETH_MMCCR_MCP_Pos                             (4U)                     
#define ETH_MMCCR_MCP_Msk                             (0x1U << ETH_MMCCR_MCP_Pos) /*!< 0x00000010 */
#define ETH_MMCCR_MCP                                 ETH_MMCCR_MCP_Msk        /* MMC counter preset           */
#define ETH_MMCCR_MCF_Pos                             (3U)                     
#define ETH_MMCCR_MCF_Msk                             (0x1U << ETH_MMCCR_MCF_Pos) /*!< 0x00000008 */
#define ETH_MMCCR_MCF                                 ETH_MMCCR_MCF_Msk        /* MMC Counter Freeze           */
#define ETH_MMCCR_ROR_Pos                             (2U)                     
#define ETH_MMCCR_ROR_Msk                             (0x1U << ETH_MMCCR_ROR_Pos) /*!< 0x00000004 */
#define ETH_MMCCR_ROR                                 ETH_MMCCR_ROR_Msk        /* Reset on Read                */
#define ETH_MMCCR_CSR_Pos                             (1U)                     
#define ETH_MMCCR_CSR_Msk                             (0x1U << ETH_MMCCR_CSR_Pos) /*!< 0x00000002 */
#define ETH_MMCCR_CSR                                 ETH_MMCCR_CSR_Msk        /* Counter Stop Rollover        */
#define ETH_MMCCR_CR_Pos                              (0U)                     
#define ETH_MMCCR_CR_Msk                              (0x1U << ETH_MMCCR_CR_Pos) /*!< 0x00000001 */
#define ETH_MMCCR_CR                                  ETH_MMCCR_CR_Msk         /* Counters Reset               */

/* Bit definition for Ethernet MMC Receive Interrupt Register */
#define ETH_MMCRIR_RGUFS_Pos                          (17U)                    
#define ETH_MMCRIR_RGUFS_Msk                          (0x1U << ETH_MMCRIR_RGUFS_Pos) /*!< 0x00020000 */
#define ETH_MMCRIR_RGUFS                              ETH_MMCRIR_RGUFS_Msk     /* Set when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIR_RFAES_Pos                          (6U)                     
#define ETH_MMCRIR_RFAES_Msk                          (0x1U << ETH_MMCRIR_RFAES_Pos) /*!< 0x00000040 */
#define ETH_MMCRIR_RFAES                              ETH_MMCRIR_RFAES_Msk     /* Set when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIR_RFCES_Pos                          (5U)                     
#define ETH_MMCRIR_RFCES_Msk                          (0x1U << ETH_MMCRIR_RFCES_Pos) /*!< 0x00000020 */
#define ETH_MMCRIR_RFCES                              ETH_MMCRIR_RFCES_Msk     /* Set when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Register */
#define ETH_MMCTIR_TGFS_Pos                           (21U)                    
#define ETH_MMCTIR_TGFS_Msk                           (0x1U << ETH_MMCTIR_TGFS_Pos) /*!< 0x00200000 */
#define ETH_MMCTIR_TGFS                               ETH_MMCTIR_TGFS_Msk      /* Set when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIR_TGFMSCS_Pos                        (15U)                    
#define ETH_MMCTIR_TGFMSCS_Msk                        (0x1U << ETH_MMCTIR_TGFMSCS_Pos) /*!< 0x00008000 */
#define ETH_MMCTIR_TGFMSCS                            ETH_MMCTIR_TGFMSCS_Msk   /* Set when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIR_TGFSCS_Pos                         (14U)                    
#define ETH_MMCTIR_TGFSCS_Msk                         (0x1U << ETH_MMCTIR_TGFSCS_Pos) /*!< 0x00004000 */
#define ETH_MMCTIR_TGFSCS                             ETH_MMCTIR_TGFSCS_Msk    /* Set when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Receive Interrupt Mask Register */
#define ETH_MMCRIMR_RGUFM_Pos                         (17U)                    
#define ETH_MMCRIMR_RGUFM_Msk                         (0x1U << ETH_MMCRIMR_RGUFM_Pos) /*!< 0x00020000 */
#define ETH_MMCRIMR_RGUFM                             ETH_MMCRIMR_RGUFM_Msk    /* Mask the interrupt when Rx good unicast frames counter reaches half the maximum value */
#define ETH_MMCRIMR_RFAEM_Pos                         (6U)                     
#define ETH_MMCRIMR_RFAEM_Msk                         (0x1U << ETH_MMCRIMR_RFAEM_Pos) /*!< 0x00000040 */
#define ETH_MMCRIMR_RFAEM                             ETH_MMCRIMR_RFAEM_Msk    /* Mask the interrupt when when Rx alignment error counter reaches half the maximum value */
#define ETH_MMCRIMR_RFCEM_Pos                         (5U)                     
#define ETH_MMCRIMR_RFCEM_Msk                         (0x1U << ETH_MMCRIMR_RFCEM_Pos) /*!< 0x00000020 */
#define ETH_MMCRIMR_RFCEM                             ETH_MMCRIMR_RFCEM_Msk    /* Mask the interrupt when Rx crc error counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmit Interrupt Mask Register */
#define ETH_MMCTIMR_TGFM_Pos                          (21U)                    
#define ETH_MMCTIMR_TGFM_Msk                          (0x1U << ETH_MMCTIMR_TGFM_Pos) /*!< 0x00200000 */
#define ETH_MMCTIMR_TGFM                              ETH_MMCTIMR_TGFM_Msk     /* Mask the interrupt when Tx good frame count counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFMSCM_Pos                       (15U)                    
#define ETH_MMCTIMR_TGFMSCM_Msk                       (0x1U << ETH_MMCTIMR_TGFMSCM_Pos) /*!< 0x00008000 */
#define ETH_MMCTIMR_TGFMSCM                           ETH_MMCTIMR_TGFMSCM_Msk  /* Mask the interrupt when Tx good multi col counter reaches half the maximum value */
#define ETH_MMCTIMR_TGFSCM_Pos                        (14U)                    
#define ETH_MMCTIMR_TGFSCM_Msk                        (0x1U << ETH_MMCTIMR_TGFSCM_Pos) /*!< 0x00004000 */
#define ETH_MMCTIMR_TGFSCM                            ETH_MMCTIMR_TGFSCM_Msk   /* Mask the interrupt when Tx good single col counter reaches half the maximum value */

/* Bit definition for Ethernet MMC Transmitted Good Frames after Single Collision Counter Register */
#define ETH_MMCTGFSCCR_TGFSCC_Pos                     (0U)                     
#define ETH_MMCTGFSCCR_TGFSCC_Msk                     (0xFFFFFFFFU << ETH_MMCTGFSCCR_TGFSCC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCTGFSCCR_TGFSCC                         ETH_MMCTGFSCCR_TGFSCC_Msk /* Number of successfully transmitted frames after a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames after More than a Single Collision Counter Register */
#define ETH_MMCTGFMSCCR_TGFMSCC_Pos                   (0U)                     
#define ETH_MMCTGFMSCCR_TGFMSCC_Msk                   (0xFFFFFFFFU << ETH_MMCTGFMSCCR_TGFMSCC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCTGFMSCCR_TGFMSCC                       ETH_MMCTGFMSCCR_TGFMSCC_Msk /* Number of successfully transmitted frames after more than a single collision in Half-duplex mode. */

/* Bit definition for Ethernet MMC Transmitted Good Frames Counter Register */
#define ETH_MMCTGFCR_TGFC_Pos                         (0U)                     
#define ETH_MMCTGFCR_TGFC_Msk                         (0xFFFFFFFFU << ETH_MMCTGFCR_TGFC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCTGFCR_TGFC                             ETH_MMCTGFCR_TGFC_Msk    /* Number of good frames transmitted. */

/* Bit definition for Ethernet MMC Received Frames with CRC Error Counter Register */
#define ETH_MMCRFCECR_RFCEC_Pos                       (0U)                     
#define ETH_MMCRFCECR_RFCEC_Msk                       (0xFFFFFFFFU << ETH_MMCRFCECR_RFCEC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCRFCECR_RFCEC                           ETH_MMCRFCECR_RFCEC_Msk  /* Number of frames received with CRC error. */

/* Bit definition for Ethernet MMC Received Frames with Alignement Error Counter Register */
#define ETH_MMCRFAECR_RFAEC_Pos                       (0U)                     
#define ETH_MMCRFAECR_RFAEC_Msk                       (0xFFFFFFFFU << ETH_MMCRFAECR_RFAEC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCRFAECR_RFAEC                           ETH_MMCRFAECR_RFAEC_Msk  /* Number of frames received with alignment (dribble) error */

/* Bit definition for Ethernet MMC Received Good Unicast Frames Counter Register */
#define ETH_MMCRGUFCR_RGUFC_Pos                       (0U)                     
#define ETH_MMCRGUFCR_RGUFC_Msk                       (0xFFFFFFFFU << ETH_MMCRGUFCR_RGUFC_Pos) /*!< 0xFFFFFFFF */
#define ETH_MMCRGUFCR_RGUFC                           ETH_MMCRGUFCR_RGUFC_Msk  /* Number of good unicast frames received. */

/******************************************************************************/
/*               Ethernet PTP Registers bits definition                       */
/******************************************************************************/

/* Bit definition for Ethernet PTP Time Stamp Contol Register */
#define ETH_PTPTSCR_TSCNT_Pos                         (16U)                    
#define ETH_PTPTSCR_TSCNT_Msk                         (0x3U << ETH_PTPTSCR_TSCNT_Pos) /*!< 0x00030000 */
#define ETH_PTPTSCR_TSCNT                             ETH_PTPTSCR_TSCNT_Msk    /* Time stamp clock node type */
#define ETH_PTPTSSR_TSSMRME_Pos                       (15U)                    
#define ETH_PTPTSSR_TSSMRME_Msk                       (0x1U << ETH_PTPTSSR_TSSMRME_Pos) /*!< 0x00008000 */
#define ETH_PTPTSSR_TSSMRME                           ETH_PTPTSSR_TSSMRME_Msk  /* Time stamp snapshot for message relevant to master enable */
#define ETH_PTPTSSR_TSSEME_Pos                        (14U)                    
#define ETH_PTPTSSR_TSSEME_Msk                        (0x1U << ETH_PTPTSSR_TSSEME_Pos) /*!< 0x00004000 */
#define ETH_PTPTSSR_TSSEME                            ETH_PTPTSSR_TSSEME_Msk   /* Time stamp snapshot for event message enable */
#define ETH_PTPTSSR_TSSIPV4FE_Pos                     (13U)                    
#define ETH_PTPTSSR_TSSIPV4FE_Msk                     (0x1U << ETH_PTPTSSR_TSSIPV4FE_Pos) /*!< 0x00002000 */
#define ETH_PTPTSSR_TSSIPV4FE                         ETH_PTPTSSR_TSSIPV4FE_Msk /* Time stamp snapshot for IPv4 frames enable */
#define ETH_PTPTSSR_TSSIPV6FE_Pos                     (12U)                    
#define ETH_PTPTSSR_TSSIPV6FE_Msk                     (0x1U << ETH_PTPTSSR_TSSIPV6FE_Pos) /*!< 0x00001000 */
#define ETH_PTPTSSR_TSSIPV6FE                         ETH_PTPTSSR_TSSIPV6FE_Msk /* Time stamp snapshot for IPv6 frames enable */
#define ETH_PTPTSSR_TSSPTPOEFE_Pos                    (11U)                    
#define ETH_PTPTSSR_TSSPTPOEFE_Msk                    (0x1U << ETH_PTPTSSR_TSSPTPOEFE_Pos) /*!< 0x00000800 */
#define ETH_PTPTSSR_TSSPTPOEFE                        ETH_PTPTSSR_TSSPTPOEFE_Msk /* Time stamp snapshot for PTP over ethernet frames enable */
#define ETH_PTPTSSR_TSPTPPSV2E_Pos                    (10U)                    
#define ETH_PTPTSSR_TSPTPPSV2E_Msk                    (0x1U << ETH_PTPTSSR_TSPTPPSV2E_Pos) /*!< 0x00000400 */
#define ETH_PTPTSSR_TSPTPPSV2E                        ETH_PTPTSSR_TSPTPPSV2E_Msk /* Time stamp PTP packet snooping for version2 format enable */
#define ETH_PTPTSSR_TSSSR_Pos                         (9U)                     
#define ETH_PTPTSSR_TSSSR_Msk                         (0x1U << ETH_PTPTSSR_TSSSR_Pos) /*!< 0x00000200 */
#define ETH_PTPTSSR_TSSSR                             ETH_PTPTSSR_TSSSR_Msk    /* Time stamp Sub-seconds rollover */
#define ETH_PTPTSSR_TSSARFE_Pos                       (8U)                     
#define ETH_PTPTSSR_TSSARFE_Msk                       (0x1U << ETH_PTPTSSR_TSSARFE_Pos) /*!< 0x00000100 */
#define ETH_PTPTSSR_TSSARFE                           ETH_PTPTSSR_TSSARFE_Msk  /* Time stamp snapshot for all received frames enable */

#define ETH_PTPTSCR_TSARU_Pos                         (5U)                     
#define ETH_PTPTSCR_TSARU_Msk                         (0x1U << ETH_PTPTSCR_TSARU_Pos) /*!< 0x00000020 */
#define ETH_PTPTSCR_TSARU                             ETH_PTPTSCR_TSARU_Msk    /* Addend register update */
#define ETH_PTPTSCR_TSITE_Pos                         (4U)                     
#define ETH_PTPTSCR_TSITE_Msk                         (0x1U << ETH_PTPTSCR_TSITE_Pos) /*!< 0x00000010 */
#define ETH_PTPTSCR_TSITE                             ETH_PTPTSCR_TSITE_Msk    /* Time stamp interrupt trigger enable */
#define ETH_PTPTSCR_TSSTU_Pos                         (3U)                     
#define ETH_PTPTSCR_TSSTU_Msk                         (0x1U << ETH_PTPTSCR_TSSTU_Pos) /*!< 0x00000008 */
#define ETH_PTPTSCR_TSSTU                             ETH_PTPTSCR_TSSTU_Msk    /* Time stamp update */
#define ETH_PTPTSCR_TSSTI_Pos                         (2U)                     
#define ETH_PTPTSCR_TSSTI_Msk                         (0x1U << ETH_PTPTSCR_TSSTI_Pos) /*!< 0x00000004 */
#define ETH_PTPTSCR_TSSTI                             ETH_PTPTSCR_TSSTI_Msk    /* Time stamp initialize */
#define ETH_PTPTSCR_TSFCU_Pos                         (1U)                     
#define ETH_PTPTSCR_TSFCU_Msk                         (0x1U << ETH_PTPTSCR_TSFCU_Pos) /*!< 0x00000002 */
#define ETH_PTPTSCR_TSFCU                             ETH_PTPTSCR_TSFCU_Msk    /* Time stamp fine or coarse update */
#define ETH_PTPTSCR_TSE_Pos                           (0U)                     
#define ETH_PTPTSCR_TSE_Msk                           (0x1U << ETH_PTPTSCR_TSE_Pos) /*!< 0x00000001 */
#define ETH_PTPTSCR_TSE                               ETH_PTPTSCR_TSE_Msk      /* Time stamp enable */

/* Bit definition for Ethernet PTP Sub-Second Increment Register */
#define ETH_PTPSSIR_STSSI_Pos                         (0U)                     
#define ETH_PTPSSIR_STSSI_Msk                         (0xFFU << ETH_PTPSSIR_STSSI_Pos) /*!< 0x000000FF */
#define ETH_PTPSSIR_STSSI                             ETH_PTPSSIR_STSSI_Msk    /* System time Sub-second increment value */

/* Bit definition for Ethernet PTP Time Stamp High Register */
#define ETH_PTPTSHR_STS_Pos                           (0U)                     
#define ETH_PTPTSHR_STS_Msk                           (0xFFFFFFFFU << ETH_PTPTSHR_STS_Pos) /*!< 0xFFFFFFFF */
#define ETH_PTPTSHR_STS                               ETH_PTPTSHR_STS_Msk      /* System Time second */

/* Bit definition for Ethernet PTP Time Stamp Low Register */
#define ETH_PTPTSLR_STPNS_Pos                         (31U)                    
#define ETH_PTPTSLR_STPNS_Msk                         (0x1U << ETH_PTPTSLR_STPNS_Pos) /*!< 0x80000000 */
#define ETH_PTPTSLR_STPNS                             ETH_PTPTSLR_STPNS_Msk    /* System Time Positive or negative time */
#define ETH_PTPTSLR_STSS_Pos                          (0U)                     
#define ETH_PTPTSLR_STSS_Msk                          (0x7FFFFFFFU << ETH_PTPTSLR_STSS_Pos) /*!< 0x7FFFFFFF */
#define ETH_PTPTSLR_STSS                              ETH_PTPTSLR_STSS_Msk     /* System Time sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp High Update Register */
#define ETH_PTPTSHUR_TSUS_Pos                         (0U)                     
#define ETH_PTPTSHUR_TSUS_Msk                         (0xFFFFFFFFU << ETH_PTPTSHUR_TSUS_Pos) /*!< 0xFFFFFFFF */
#define ETH_PTPTSHUR_TSUS                             ETH_PTPTSHUR_TSUS_Msk    /* Time stamp update seconds */

/* Bit definition for Ethernet PTP Time Stamp Low Update Register */
#define ETH_PTPTSLUR_TSUPNS_Pos                       (31U)                    
#define ETH_PTPTSLUR_TSUPNS_Msk                       (0x1U << ETH_PTPTSLUR_TSUPNS_Pos) /*!< 0x80000000 */
#define ETH_PTPTSLUR_TSUPNS                           ETH_PTPTSLUR_TSUPNS_Msk  /* Time stamp update Positive or negative time */
#define ETH_PTPTSLUR_TSUSS_Pos                        (0U)                     
#define ETH_PTPTSLUR_TSUSS_Msk                        (0x7FFFFFFFU << ETH_PTPTSLUR_TSUSS_Pos) /*!< 0x7FFFFFFF */
#define ETH_PTPTSLUR_TSUSS                            ETH_PTPTSLUR_TSUSS_Msk   /* Time stamp update sub-seconds */

/* Bit definition for Ethernet PTP Time Stamp Addend Register */
#define ETH_PTPTSAR_TSA_Pos                           (0U)                     
#define ETH_PTPTSAR_TSA_Msk                           (0xFFFFFFFFU << ETH_PTPTSAR_TSA_Pos) /*!< 0xFFFFFFFF */
#define ETH_PTPTSAR_TSA                               ETH_PTPTSAR_TSA_Msk      /* Time stamp addend */

/* Bit definition for Ethernet PTP Target Time High Register */
#define ETH_PTPTTHR_TTSH_Pos                          (0U)                     
#define ETH_PTPTTHR_TTSH_Msk                          (0xFFFFFFFFU << ETH_PTPTTHR_TTSH_Pos) /*!< 0xFFFFFFFF */
#define ETH_PTPTTHR_TTSH                              ETH_PTPTTHR_TTSH_Msk     /* Target time stamp high */

/* Bit definition for Ethernet PTP Target Time Low Register */
#define ETH_PTPTTLR_TTSL_Pos                          (0U)                     
#define ETH_PTPTTLR_TTSL_Msk                          (0xFFFFFFFFU << ETH_PTPTTLR_TTSL_Pos) /*!< 0xFFFFFFFF */
#define ETH_PTPTTLR_TTSL                              ETH_PTPTTLR_TTSL_Msk     /* Target time stamp low */

/* Bit definition for Ethernet PTP Time Stamp Status Register */
#define ETH_PTPTSSR_TSTTR_Pos                         (5U)                     
#define ETH_PTPTSSR_TSTTR_Msk                         (0x1U << ETH_PTPTSSR_TSTTR_Pos) /*!< 0x00000020 */
#define ETH_PTPTSSR_TSTTR                             ETH_PTPTSSR_TSTTR_Msk    /* Time stamp target time reached */
#define ETH_PTPTSSR_TSSO_Pos                          (4U)                     
#define ETH_PTPTSSR_TSSO_Msk                          (0x1U << ETH_PTPTSSR_TSSO_Pos) /*!< 0x00000010 */
#define ETH_PTPTSSR_TSSO                              ETH_PTPTSSR_TSSO_Msk     /* Time stamp seconds overflow */

/******************************************************************************/
/*                 Ethernet DMA Registers bits definition                     */
/******************************************************************************/

/* Bit definition for Ethernet DMA Bus Mode Register */
#define ETH_DMABMR_AAB_Pos                            (25U)                    
#define ETH_DMABMR_AAB_Msk                            (0x1U << ETH_DMABMR_AAB_Pos) /*!< 0x02000000 */
#define ETH_DMABMR_AAB                                ETH_DMABMR_AAB_Msk       /* Address-Aligned beats */
#define ETH_DMABMR_FPM_Pos                            (24U)                    
#define ETH_DMABMR_FPM_Msk                            (0x1U << ETH_DMABMR_FPM_Pos) /*!< 0x01000000 */
#define ETH_DMABMR_FPM                                ETH_DMABMR_FPM_Msk       /* 4xPBL mode */
#define ETH_DMABMR_USP_Pos                            (23U)                    
#define ETH_DMABMR_USP_Msk                            (0x1U << ETH_DMABMR_USP_Pos) /*!< 0x00800000 */
#define ETH_DMABMR_USP                                ETH_DMABMR_USP_Msk       /* Use separate PBL */
#define ETH_DMABMR_RDP_Pos                            (17U)                    
#define ETH_DMABMR_RDP_Msk                            (0x3FU << ETH_DMABMR_RDP_Pos) /*!< 0x007E0000 */
#define ETH_DMABMR_RDP                                ETH_DMABMR_RDP_Msk       /* RxDMA PBL */
#define ETH_DMABMR_RDP_1Beat                          0x00020000U              /* maximum number of beats to be transferred in one RxDMA transaction is 1 */
#define ETH_DMABMR_RDP_2Beat                          0x00040000U              /* maximum number of beats to be transferred in one RxDMA transaction is 2 */
#define ETH_DMABMR_RDP_4Beat                          0x00080000U              /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_DMABMR_RDP_8Beat                          0x00100000U              /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_DMABMR_RDP_16Beat                         0x00200000U              /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_DMABMR_RDP_32Beat                         0x00400000U              /* maximum number of beats to be transferred in one RxDMA transaction is 32 */                
#define ETH_DMABMR_RDP_4xPBL_4Beat                    0x01020000U              /* maximum number of beats to be transferred in one RxDMA transaction is 4 */
#define ETH_DMABMR_RDP_4xPBL_8Beat                    0x01040000U              /* maximum number of beats to be transferred in one RxDMA transaction is 8 */
#define ETH_DMABMR_RDP_4xPBL_16Beat                   0x01080000U              /* maximum number of beats to be transferred in one RxDMA transaction is 16 */
#define ETH_DMABMR_RDP_4xPBL_32Beat                   0x01100000U              /* maximum number of beats to be transferred in one RxDMA transaction is 32 */
#define ETH_DMABMR_RDP_4xPBL_64Beat                   0x01200000U              /* maximum number of beats to be transferred in one RxDMA transaction is 64 */
#define ETH_DMABMR_RDP_4xPBL_128Beat                  0x01400000U              /* maximum number of beats to be transferred in one RxDMA transaction is 128 */  
#define ETH_DMABMR_FB_Pos                             (16U)                    
#define ETH_DMABMR_FB_Msk                             (0x1U << ETH_DMABMR_FB_Pos) /*!< 0x00010000 */
#define ETH_DMABMR_FB                                 ETH_DMABMR_FB_Msk        /* Fixed Burst */
#define ETH_DMABMR_RTPR_Pos                           (14U)                    
#define ETH_DMABMR_RTPR_Msk                           (0x3U << ETH_DMABMR_RTPR_Pos) /*!< 0x0000C000 */
#define ETH_DMABMR_RTPR                               ETH_DMABMR_RTPR_Msk      /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_1_1                           0x00000000U              /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_2_1                           0x00004000U              /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_3_1                           0x00008000U              /* Rx Tx priority ratio */
#define ETH_DMABMR_RTPR_4_1                           0x0000C000U              /* Rx Tx priority ratio */  
#define ETH_DMABMR_PBL_Pos                            (8U)                     
#define ETH_DMABMR_PBL_Msk                            (0x3FU << ETH_DMABMR_PBL_Pos) /*!< 0x00003F00 */
#define ETH_DMABMR_PBL                                ETH_DMABMR_PBL_Msk       /* Programmable burst length */
#define ETH_DMABMR_PBL_1Beat                          0x00000100U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 1 */
#define ETH_DMABMR_PBL_2Beat                          0x00000200U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 2 */
#define ETH_DMABMR_PBL_4Beat                          0x00000400U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_DMABMR_PBL_8Beat                          0x00000800U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_DMABMR_PBL_16Beat                         0x00001000U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_DMABMR_PBL_32Beat                         0x00002000U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */                
#define ETH_DMABMR_PBL_4xPBL_4Beat                    0x01000100U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 4 */
#define ETH_DMABMR_PBL_4xPBL_8Beat                    0x01000200U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 8 */
#define ETH_DMABMR_PBL_4xPBL_16Beat                   0x01000400U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 16 */
#define ETH_DMABMR_PBL_4xPBL_32Beat                   0x01000800U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 32 */
#define ETH_DMABMR_PBL_4xPBL_64Beat                   0x01001000U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 64 */
#define ETH_DMABMR_PBL_4xPBL_128Beat                  0x01002000U              /* maximum number of beats to be transferred in one TxDMA (or both) transaction is 128 */
#define ETH_DMABMR_EDE_Pos                            (7U)                     
#define ETH_DMABMR_EDE_Msk                            (0x1U << ETH_DMABMR_EDE_Pos) /*!< 0x00000080 */
#define ETH_DMABMR_EDE                                ETH_DMABMR_EDE_Msk       /* Enhanced Descriptor Enable */
#define ETH_DMABMR_DSL_Pos                            (2U)                     
#define ETH_DMABMR_DSL_Msk                            (0x1FU << ETH_DMABMR_DSL_Pos) /*!< 0x0000007C */
#define ETH_DMABMR_DSL                                ETH_DMABMR_DSL_Msk       /* Descriptor Skip Length */
#define ETH_DMABMR_DA_Pos                             (1U)                     
#define ETH_DMABMR_DA_Msk                             (0x1U << ETH_DMABMR_DA_Pos) /*!< 0x00000002 */
#define ETH_DMABMR_DA                                 ETH_DMABMR_DA_Msk        /* DMA arbitration scheme */
#define ETH_DMABMR_SR_Pos                             (0U)                     
#define ETH_DMABMR_SR_Msk                             (0x1U << ETH_DMABMR_SR_Pos) /*!< 0x00000001 */
#define ETH_DMABMR_SR                                 ETH_DMABMR_SR_Msk        /* Software reset */

/* Bit definition for Ethernet DMA Transmit Poll Demand Register */
#define ETH_DMATPDR_TPD_Pos                           (0U)                     
#define ETH_DMATPDR_TPD_Msk                           (0xFFFFFFFFU << ETH_DMATPDR_TPD_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMATPDR_TPD                               ETH_DMATPDR_TPD_Msk      /* Transmit poll demand */

/* Bit definition for Ethernet DMA Receive Poll Demand Register */
#define ETH_DMARPDR_RPD_Pos                           (0U)                     
#define ETH_DMARPDR_RPD_Msk                           (0xFFFFFFFFU << ETH_DMARPDR_RPD_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMARPDR_RPD                               ETH_DMARPDR_RPD_Msk      /* Receive poll demand  */

/* Bit definition for Ethernet DMA Receive Descriptor List Address Register */
#define ETH_DMARDLAR_SRL_Pos                          (0U)                     
#define ETH_DMARDLAR_SRL_Msk                          (0xFFFFFFFFU << ETH_DMARDLAR_SRL_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMARDLAR_SRL                              ETH_DMARDLAR_SRL_Msk     /* Start of receive list */

/* Bit definition for Ethernet DMA Transmit Descriptor List Address Register */
#define ETH_DMATDLAR_STL_Pos                          (0U)                     
#define ETH_DMATDLAR_STL_Msk                          (0xFFFFFFFFU << ETH_DMATDLAR_STL_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMATDLAR_STL                              ETH_DMATDLAR_STL_Msk     /* Start of transmit list */

/* Bit definition for Ethernet DMA Status Register */
#define ETH_DMASR_TSTS_Pos                            (29U)                    
#define ETH_DMASR_TSTS_Msk                            (0x1U << ETH_DMASR_TSTS_Pos) /*!< 0x20000000 */
#define ETH_DMASR_TSTS                                ETH_DMASR_TSTS_Msk       /* Time-stamp trigger status */
#define ETH_DMASR_PMTS_Pos                            (28U)                    
#define ETH_DMASR_PMTS_Msk                            (0x1U << ETH_DMASR_PMTS_Pos) /*!< 0x10000000 */
#define ETH_DMASR_PMTS                                ETH_DMASR_PMTS_Msk       /* PMT status */
#define ETH_DMASR_MMCS_Pos                            (27U)                    
#define ETH_DMASR_MMCS_Msk                            (0x1U << ETH_DMASR_MMCS_Pos) /*!< 0x08000000 */
#define ETH_DMASR_MMCS                                ETH_DMASR_MMCS_Msk       /* MMC status */
#define ETH_DMASR_EBS_Pos                             (23U)                    
#define ETH_DMASR_EBS_Msk                             (0x7U << ETH_DMASR_EBS_Pos) /*!< 0x03800000 */
#define ETH_DMASR_EBS                                 ETH_DMASR_EBS_Msk        /* Error bits status */
  /* combination with EBS[2:0] for GetFlagStatus function */
#define ETH_DMASR_EBS_DescAccess_Pos                  (25U)                    
#define ETH_DMASR_EBS_DescAccess_Msk                  (0x1U << ETH_DMASR_EBS_DescAccess_Pos) /*!< 0x02000000 */
#define ETH_DMASR_EBS_DescAccess                      ETH_DMASR_EBS_DescAccess_Msk /* Error bits 0-data buffer, 1-desc. access */
#define ETH_DMASR_EBS_ReadTransf_Pos                  (24U)                    
#define ETH_DMASR_EBS_ReadTransf_Msk                  (0x1U << ETH_DMASR_EBS_ReadTransf_Pos) /*!< 0x01000000 */
#define ETH_DMASR_EBS_ReadTransf                      ETH_DMASR_EBS_ReadTransf_Msk /* Error bits 0-write trnsf, 1-read transfr */
#define ETH_DMASR_EBS_DataTransfTx_Pos                (23U)                    
#define ETH_DMASR_EBS_DataTransfTx_Msk                (0x1U << ETH_DMASR_EBS_DataTransfTx_Pos) /*!< 0x00800000 */
#define ETH_DMASR_EBS_DataTransfTx                    ETH_DMASR_EBS_DataTransfTx_Msk /* Error bits 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS_Pos                             (20U)                    
#define ETH_DMASR_TPS_Msk                             (0x7U << ETH_DMASR_TPS_Pos) /*!< 0x00700000 */
#define ETH_DMASR_TPS                                 ETH_DMASR_TPS_Msk        /* Transmit process state */
#define ETH_DMASR_TPS_Stopped                         0x00000000U              /* Stopped - Reset or Stop Tx Command issued  */
#define ETH_DMASR_TPS_Fetching_Pos                    (20U)                    
#define ETH_DMASR_TPS_Fetching_Msk                    (0x1U << ETH_DMASR_TPS_Fetching_Pos) /*!< 0x00100000 */
#define ETH_DMASR_TPS_Fetching                        ETH_DMASR_TPS_Fetching_Msk /* Running - fetching the Tx descriptor */
#define ETH_DMASR_TPS_Waiting_Pos                     (21U)                    
#define ETH_DMASR_TPS_Waiting_Msk                     (0x1U << ETH_DMASR_TPS_Waiting_Pos) /*!< 0x00200000 */
#define ETH_DMASR_TPS_Waiting                         ETH_DMASR_TPS_Waiting_Msk /* Running - waiting for status */
#define ETH_DMASR_TPS_Reading_Pos                     (20U)                    
#define ETH_DMASR_TPS_Reading_Msk                     (0x3U << ETH_DMASR_TPS_Reading_Pos) /*!< 0x00300000 */
#define ETH_DMASR_TPS_Reading                         ETH_DMASR_TPS_Reading_Msk /* Running - reading the data from host memory */
#define ETH_DMASR_TPS_Suspended_Pos                   (21U)                    
#define ETH_DMASR_TPS_Suspended_Msk                   (0x3U << ETH_DMASR_TPS_Suspended_Pos) /*!< 0x00600000 */
#define ETH_DMASR_TPS_Suspended                       ETH_DMASR_TPS_Suspended_Msk /* Suspended - Tx Descriptor unavailabe */
#define ETH_DMASR_TPS_Closing_Pos                     (20U)                    
#define ETH_DMASR_TPS_Closing_Msk                     (0x7U << ETH_DMASR_TPS_Closing_Pos) /*!< 0x00700000 */
#define ETH_DMASR_TPS_Closing                         ETH_DMASR_TPS_Closing_Msk /* Running - closing Rx descriptor */
#define ETH_DMASR_RPS_Pos                             (17U)                    
#define ETH_DMASR_RPS_Msk                             (0x7U << ETH_DMASR_RPS_Pos) /*!< 0x000E0000 */
#define ETH_DMASR_RPS                                 ETH_DMASR_RPS_Msk        /* Receive process state */
#define ETH_DMASR_RPS_Stopped                         0x00000000U              /* Stopped - Reset or Stop Rx Command issued */
#define ETH_DMASR_RPS_Fetching_Pos                    (17U)                    
#define ETH_DMASR_RPS_Fetching_Msk                    (0x1U << ETH_DMASR_RPS_Fetching_Pos) /*!< 0x00020000 */
#define ETH_DMASR_RPS_Fetching                        ETH_DMASR_RPS_Fetching_Msk /* Running - fetching the Rx descriptor */
#define ETH_DMASR_RPS_Waiting_Pos                     (17U)                    
#define ETH_DMASR_RPS_Waiting_Msk                     (0x3U << ETH_DMASR_RPS_Waiting_Pos) /*!< 0x00060000 */
#define ETH_DMASR_RPS_Waiting                         ETH_DMASR_RPS_Waiting_Msk /* Running - waiting for packet */
#define ETH_DMASR_RPS_Suspended_Pos                   (19U)                    
#define ETH_DMASR_RPS_Suspended_Msk                   (0x1U << ETH_DMASR_RPS_Suspended_Pos) /*!< 0x00080000 */
#define ETH_DMASR_RPS_Suspended                       ETH_DMASR_RPS_Suspended_Msk /* Suspended - Rx Descriptor unavailable */
#define ETH_DMASR_RPS_Closing_Pos                     (17U)                    
#define ETH_DMASR_RPS_Closing_Msk                     (0x5U << ETH_DMASR_RPS_Closing_Pos) /*!< 0x000A0000 */
#define ETH_DMASR_RPS_Closing                         ETH_DMASR_RPS_Closing_Msk /* Running - closing descriptor */
#define ETH_DMASR_RPS_Queuing_Pos                     (17U)                    
#define ETH_DMASR_RPS_Queuing_Msk                     (0x7U << ETH_DMASR_RPS_Queuing_Pos) /*!< 0x000E0000 */
#define ETH_DMASR_RPS_Queuing                         ETH_DMASR_RPS_Queuing_Msk /* Running - queuing the recieve frame into host memory */
#define ETH_DMASR_NIS_Pos                             (16U)                    
#define ETH_DMASR_NIS_Msk                             (0x1U << ETH_DMASR_NIS_Pos) /*!< 0x00010000 */
#define ETH_DMASR_NIS                                 ETH_DMASR_NIS_Msk        /* Normal interrupt summary */
#define ETH_DMASR_AIS_Pos                             (15U)                    
#define ETH_DMASR_AIS_Msk                             (0x1U << ETH_DMASR_AIS_Pos) /*!< 0x00008000 */
#define ETH_DMASR_AIS                                 ETH_DMASR_AIS_Msk        /* Abnormal interrupt summary */
#define ETH_DMASR_ERS_Pos                             (14U)                    
#define ETH_DMASR_ERS_Msk                             (0x1U << ETH_DMASR_ERS_Pos) /*!< 0x00004000 */
#define ETH_DMASR_ERS                                 ETH_DMASR_ERS_Msk        /* Early receive status */
#define ETH_DMASR_FBES_Pos                            (13U)                    
#define ETH_DMASR_FBES_Msk                            (0x1U << ETH_DMASR_FBES_Pos) /*!< 0x00002000 */
#define ETH_DMASR_FBES                                ETH_DMASR_FBES_Msk       /* Fatal bus error status */
#define ETH_DMASR_ETS_Pos                             (10U)                    
#define ETH_DMASR_ETS_Msk                             (0x1U << ETH_DMASR_ETS_Pos) /*!< 0x00000400 */
#define ETH_DMASR_ETS                                 ETH_DMASR_ETS_Msk        /* Early transmit status */
#define ETH_DMASR_RWTS_Pos                            (9U)                     
#define ETH_DMASR_RWTS_Msk                            (0x1U << ETH_DMASR_RWTS_Pos) /*!< 0x00000200 */
#define ETH_DMASR_RWTS                                ETH_DMASR_RWTS_Msk       /* Receive watchdog timeout status */
#define ETH_DMASR_RPSS_Pos                            (8U)                     
#define ETH_DMASR_RPSS_Msk                            (0x1U << ETH_DMASR_RPSS_Pos) /*!< 0x00000100 */
#define ETH_DMASR_RPSS                                ETH_DMASR_RPSS_Msk       /* Receive process stopped status */
#define ETH_DMASR_RBUS_Pos                            (7U)                     
#define ETH_DMASR_RBUS_Msk                            (0x1U << ETH_DMASR_RBUS_Pos) /*!< 0x00000080 */
#define ETH_DMASR_RBUS                                ETH_DMASR_RBUS_Msk       /* Receive buffer unavailable status */
#define ETH_DMASR_RS_Pos                              (6U)                     
#define ETH_DMASR_RS_Msk                              (0x1U << ETH_DMASR_RS_Pos) /*!< 0x00000040 */
#define ETH_DMASR_RS                                  ETH_DMASR_RS_Msk         /* Receive status */
#define ETH_DMASR_TUS_Pos                             (5U)                     
#define ETH_DMASR_TUS_Msk                             (0x1U << ETH_DMASR_TUS_Pos) /*!< 0x00000020 */
#define ETH_DMASR_TUS                                 ETH_DMASR_TUS_Msk        /* Transmit underflow status */
#define ETH_DMASR_ROS_Pos                             (4U)                     
#define ETH_DMASR_ROS_Msk                             (0x1U << ETH_DMASR_ROS_Pos) /*!< 0x00000010 */
#define ETH_DMASR_ROS                                 ETH_DMASR_ROS_Msk        /* Receive overflow status */
#define ETH_DMASR_TJTS_Pos                            (3U)                     
#define ETH_DMASR_TJTS_Msk                            (0x1U << ETH_DMASR_TJTS_Pos) /*!< 0x00000008 */
#define ETH_DMASR_TJTS                                ETH_DMASR_TJTS_Msk       /* Transmit jabber timeout status */
#define ETH_DMASR_TBUS_Pos                            (2U)                     
#define ETH_DMASR_TBUS_Msk                            (0x1U << ETH_DMASR_TBUS_Pos) /*!< 0x00000004 */
#define ETH_DMASR_TBUS                                ETH_DMASR_TBUS_Msk       /* Transmit buffer unavailable status */
#define ETH_DMASR_TPSS_Pos                            (1U)                     
#define ETH_DMASR_TPSS_Msk                            (0x1U << ETH_DMASR_TPSS_Pos) /*!< 0x00000002 */
#define ETH_DMASR_TPSS                                ETH_DMASR_TPSS_Msk       /* Transmit process stopped status */
#define ETH_DMASR_TS_Pos                              (0U)                     
#define ETH_DMASR_TS_Msk                              (0x1U << ETH_DMASR_TS_Pos) /*!< 0x00000001 */
#define ETH_DMASR_TS                                  ETH_DMASR_TS_Msk         /* Transmit status */

/* Bit definition for Ethernet DMA Operation Mode Register */
#define ETH_DMAOMR_DTCEFD_Pos                         (26U)                    
#define ETH_DMAOMR_DTCEFD_Msk                         (0x1U << ETH_DMAOMR_DTCEFD_Pos) /*!< 0x04000000 */
#define ETH_DMAOMR_DTCEFD                             ETH_DMAOMR_DTCEFD_Msk    /* Disable Dropping of TCP/IP checksum error frames */
#define ETH_DMAOMR_RSF_Pos                            (25U)                    
#define ETH_DMAOMR_RSF_Msk                            (0x1U << ETH_DMAOMR_RSF_Pos) /*!< 0x02000000 */
#define ETH_DMAOMR_RSF                                ETH_DMAOMR_RSF_Msk       /* Receive store and forward */
#define ETH_DMAOMR_DFRF_Pos                           (24U)                    
#define ETH_DMAOMR_DFRF_Msk                           (0x1U << ETH_DMAOMR_DFRF_Pos) /*!< 0x01000000 */
#define ETH_DMAOMR_DFRF                               ETH_DMAOMR_DFRF_Msk      /* Disable flushing of received frames */
#define ETH_DMAOMR_TSF_Pos                            (21U)                    
#define ETH_DMAOMR_TSF_Msk                            (0x1U << ETH_DMAOMR_TSF_Pos) /*!< 0x00200000 */
#define ETH_DMAOMR_TSF                                ETH_DMAOMR_TSF_Msk       /* Transmit store and forward */
#define ETH_DMAOMR_FTF_Pos                            (20U)                    
#define ETH_DMAOMR_FTF_Msk                            (0x1U << ETH_DMAOMR_FTF_Pos) /*!< 0x00100000 */
#define ETH_DMAOMR_FTF                                ETH_DMAOMR_FTF_Msk       /* Flush transmit FIFO */
#define ETH_DMAOMR_TTC_Pos                            (14U)                    
#define ETH_DMAOMR_TTC_Msk                            (0x7U << ETH_DMAOMR_TTC_Pos) /*!< 0x0001C000 */
#define ETH_DMAOMR_TTC                                ETH_DMAOMR_TTC_Msk       /* Transmit threshold control */
#define ETH_DMAOMR_TTC_64Bytes                        0x00000000U              /* threshold level of the MTL Transmit FIFO is 64 Bytes */
#define ETH_DMAOMR_TTC_128Bytes                       0x00004000U              /* threshold level of the MTL Transmit FIFO is 128 Bytes */
#define ETH_DMAOMR_TTC_192Bytes                       0x00008000U              /* threshold level of the MTL Transmit FIFO is 192 Bytes */
#define ETH_DMAOMR_TTC_256Bytes                       0x0000C000U              /* threshold level of the MTL Transmit FIFO is 256 Bytes */
#define ETH_DMAOMR_TTC_40Bytes                        0x00010000U              /* threshold level of the MTL Transmit FIFO is 40 Bytes */
#define ETH_DMAOMR_TTC_32Bytes                        0x00014000U              /* threshold level of the MTL Transmit FIFO is 32 Bytes */
#define ETH_DMAOMR_TTC_24Bytes                        0x00018000U              /* threshold level of the MTL Transmit FIFO is 24 Bytes */
#define ETH_DMAOMR_TTC_16Bytes                        0x0001C000U              /* threshold level of the MTL Transmit FIFO is 16 Bytes */
#define ETH_DMAOMR_ST_Pos                             (13U)                    
#define ETH_DMAOMR_ST_Msk                             (0x1U << ETH_DMAOMR_ST_Pos) /*!< 0x00002000 */
#define ETH_DMAOMR_ST                                 ETH_DMAOMR_ST_Msk        /* Start/stop transmission command */
#define ETH_DMAOMR_FEF_Pos                            (7U)                     
#define ETH_DMAOMR_FEF_Msk                            (0x1U << ETH_DMAOMR_FEF_Pos) /*!< 0x00000080 */
#define ETH_DMAOMR_FEF                                ETH_DMAOMR_FEF_Msk       /* Forward error frames */
#define ETH_DMAOMR_FUGF_Pos                           (6U)                     
#define ETH_DMAOMR_FUGF_Msk                           (0x1U << ETH_DMAOMR_FUGF_Pos) /*!< 0x00000040 */
#define ETH_DMAOMR_FUGF                               ETH_DMAOMR_FUGF_Msk      /* Forward undersized good frames */
#define ETH_DMAOMR_RTC_Pos                            (3U)                     
#define ETH_DMAOMR_RTC_Msk                            (0x3U << ETH_DMAOMR_RTC_Pos) /*!< 0x00000018 */
#define ETH_DMAOMR_RTC                                ETH_DMAOMR_RTC_Msk       /* receive threshold control */
#define ETH_DMAOMR_RTC_64Bytes                        0x00000000U              /* threshold level of the MTL Receive FIFO is 64 Bytes */
#define ETH_DMAOMR_RTC_32Bytes                        0x00000008U              /* threshold level of the MTL Receive FIFO is 32 Bytes */
#define ETH_DMAOMR_RTC_96Bytes                        0x00000010U              /* threshold level of the MTL Receive FIFO is 96 Bytes */
#define ETH_DMAOMR_RTC_128Bytes                       0x00000018U              /* threshold level of the MTL Receive FIFO is 128 Bytes */
#define ETH_DMAOMR_OSF_Pos                            (2U)                     
#define ETH_DMAOMR_OSF_Msk                            (0x1U << ETH_DMAOMR_OSF_Pos) /*!< 0x00000004 */
#define ETH_DMAOMR_OSF                                ETH_DMAOMR_OSF_Msk       /* operate on second frame */
#define ETH_DMAOMR_SR_Pos                             (1U)                     
#define ETH_DMAOMR_SR_Msk                             (0x1U << ETH_DMAOMR_SR_Pos) /*!< 0x00000002 */
#define ETH_DMAOMR_SR                                 ETH_DMAOMR_SR_Msk        /* Start/stop receive */

/* Bit definition for Ethernet DMA Interrupt Enable Register */
#define ETH_DMAIER_NISE_Pos                           (16U)                    
#define ETH_DMAIER_NISE_Msk                           (0x1U << ETH_DMAIER_NISE_Pos) /*!< 0x00010000 */
#define ETH_DMAIER_NISE                               ETH_DMAIER_NISE_Msk      /* Normal interrupt summary enable */
#define ETH_DMAIER_AISE_Pos                           (15U)                    
#define ETH_DMAIER_AISE_Msk                           (0x1U << ETH_DMAIER_AISE_Pos) /*!< 0x00008000 */
#define ETH_DMAIER_AISE                               ETH_DMAIER_AISE_Msk      /* Abnormal interrupt summary enable */
#define ETH_DMAIER_ERIE_Pos                           (14U)                    
#define ETH_DMAIER_ERIE_Msk                           (0x1U << ETH_DMAIER_ERIE_Pos) /*!< 0x00004000 */
#define ETH_DMAIER_ERIE                               ETH_DMAIER_ERIE_Msk      /* Early receive interrupt enable */
#define ETH_DMAIER_FBEIE_Pos                          (13U)                    
#define ETH_DMAIER_FBEIE_Msk                          (0x1U << ETH_DMAIER_FBEIE_Pos) /*!< 0x00002000 */
#define ETH_DMAIER_FBEIE                              ETH_DMAIER_FBEIE_Msk     /* Fatal bus error interrupt enable */
#define ETH_DMAIER_ETIE_Pos                           (10U)                    
#define ETH_DMAIER_ETIE_Msk                           (0x1U << ETH_DMAIER_ETIE_Pos) /*!< 0x00000400 */
#define ETH_DMAIER_ETIE                               ETH_DMAIER_ETIE_Msk      /* Early transmit interrupt enable */
#define ETH_DMAIER_RWTIE_Pos                          (9U)                     
#define ETH_DMAIER_RWTIE_Msk                          (0x1U << ETH_DMAIER_RWTIE_Pos) /*!< 0x00000200 */
#define ETH_DMAIER_RWTIE                              ETH_DMAIER_RWTIE_Msk     /* Receive watchdog timeout interrupt enable */
#define ETH_DMAIER_RPSIE_Pos                          (8U)                     
#define ETH_DMAIER_RPSIE_Msk                          (0x1U << ETH_DMAIER_RPSIE_Pos) /*!< 0x00000100 */
#define ETH_DMAIER_RPSIE                              ETH_DMAIER_RPSIE_Msk     /* Receive process stopped interrupt enable */
#define ETH_DMAIER_RBUIE_Pos                          (7U)                     
#define ETH_DMAIER_RBUIE_Msk                          (0x1U << ETH_DMAIER_RBUIE_Pos) /*!< 0x00000080 */
#define ETH_DMAIER_RBUIE                              ETH_DMAIER_RBUIE_Msk     /* Receive buffer unavailable interrupt enable */
#define ETH_DMAIER_RIE_Pos                            (6U)                     
#define ETH_DMAIER_RIE_Msk                            (0x1U << ETH_DMAIER_RIE_Pos) /*!< 0x00000040 */
#define ETH_DMAIER_RIE                                ETH_DMAIER_RIE_Msk       /* Receive interrupt enable */
#define ETH_DMAIER_TUIE_Pos                           (5U)                     
#define ETH_DMAIER_TUIE_Msk                           (0x1U << ETH_DMAIER_TUIE_Pos) /*!< 0x00000020 */
#define ETH_DMAIER_TUIE                               ETH_DMAIER_TUIE_Msk      /* Transmit Underflow interrupt enable */
#define ETH_DMAIER_ROIE_Pos                           (4U)                     
#define ETH_DMAIER_ROIE_Msk                           (0x1U << ETH_DMAIER_ROIE_Pos) /*!< 0x00000010 */
#define ETH_DMAIER_ROIE                               ETH_DMAIER_ROIE_Msk      /* Receive Overflow interrupt enable */
#define ETH_DMAIER_TJTIE_Pos                          (3U)                     
#define ETH_DMAIER_TJTIE_Msk                          (0x1U << ETH_DMAIER_TJTIE_Pos) /*!< 0x00000008 */
#define ETH_DMAIER_TJTIE                              ETH_DMAIER_TJTIE_Msk     /* Transmit jabber timeout interrupt enable */
#define ETH_DMAIER_TBUIE_Pos                          (2U)                     
#define ETH_DMAIER_TBUIE_Msk                          (0x1U << ETH_DMAIER_TBUIE_Pos) /*!< 0x00000004 */
#define ETH_DMAIER_TBUIE                              ETH_DMAIER_TBUIE_Msk     /* Transmit buffer unavailable interrupt enable */
#define ETH_DMAIER_TPSIE_Pos                          (1U)                     
#define ETH_DMAIER_TPSIE_Msk                          (0x1U << ETH_DMAIER_TPSIE_Pos) /*!< 0x00000002 */
#define ETH_DMAIER_TPSIE                              ETH_DMAIER_TPSIE_Msk     /* Transmit process stopped interrupt enable */
#define ETH_DMAIER_TIE_Pos                            (0U)                     
#define ETH_DMAIER_TIE_Msk                            (0x1U << ETH_DMAIER_TIE_Pos) /*!< 0x00000001 */
#define ETH_DMAIER_TIE                                ETH_DMAIER_TIE_Msk       /* Transmit interrupt enable */

/* Bit definition for Ethernet DMA Missed Frame and Buffer Overflow Counter Register */
#define ETH_DMAMFBOCR_OFOC_Pos                        (28U)                    
#define ETH_DMAMFBOCR_OFOC_Msk                        (0x1U << ETH_DMAMFBOCR_OFOC_Pos) /*!< 0x10000000 */
#define ETH_DMAMFBOCR_OFOC                            ETH_DMAMFBOCR_OFOC_Msk   /* Overflow bit for FIFO overflow counter */
#define ETH_DMAMFBOCR_MFA_Pos                         (17U)                    
#define ETH_DMAMFBOCR_MFA_Msk                         (0x7FFU << ETH_DMAMFBOCR_MFA_Pos) /*!< 0x0FFE0000 */
#define ETH_DMAMFBOCR_MFA                             ETH_DMAMFBOCR_MFA_Msk    /* Number of frames missed by the application */
#define ETH_DMAMFBOCR_OMFC_Pos                        (16U)                    
#define ETH_DMAMFBOCR_OMFC_Msk                        (0x1U << ETH_DMAMFBOCR_OMFC_Pos) /*!< 0x00010000 */
#define ETH_DMAMFBOCR_OMFC                            ETH_DMAMFBOCR_OMFC_Msk   /* Overflow bit for missed frame counter */
#define ETH_DMAMFBOCR_MFC_Pos                         (0U)                     
#define ETH_DMAMFBOCR_MFC_Msk                         (0xFFFFU << ETH_DMAMFBOCR_MFC_Pos) /*!< 0x0000FFFF */
#define ETH_DMAMFBOCR_MFC                             ETH_DMAMFBOCR_MFC_Msk    /* Number of frames missed by the controller */

/* Bit definition for Ethernet DMA Current Host Transmit Descriptor Register */
#define ETH_DMACHTDR_HTDAP_Pos                        (0U)                     
#define ETH_DMACHTDR_HTDAP_Msk                        (0xFFFFFFFFU << ETH_DMACHTDR_HTDAP_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMACHTDR_HTDAP                            ETH_DMACHTDR_HTDAP_Msk   /* Host transmit descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Descriptor Register */
#define ETH_DMACHRDR_HRDAP_Pos                        (0U)                     
#define ETH_DMACHRDR_HRDAP_Msk                        (0xFFFFFFFFU << ETH_DMACHRDR_HRDAP_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMACHRDR_HRDAP                            ETH_DMACHRDR_HRDAP_Msk   /* Host receive descriptor address pointer */

/* Bit definition for Ethernet DMA Current Host Transmit Buffer Address Register */
#define ETH_DMACHTBAR_HTBAP_Pos                       (0U)                     
#define ETH_DMACHTBAR_HTBAP_Msk                       (0xFFFFFFFFU << ETH_DMACHTBAR_HTBAP_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMACHTBAR_HTBAP                           ETH_DMACHTBAR_HTBAP_Msk  /* Host transmit buffer address pointer */

/* Bit definition for Ethernet DMA Current Host Receive Buffer Address Register */
#define ETH_DMACHRBAR_HRBAP_Pos                       (0U)                     
#define ETH_DMACHRBAR_HRBAP_Msk                       (0xFFFFFFFFU << ETH_DMACHRBAR_HRBAP_Pos) /*!< 0xFFFFFFFF */
#define ETH_DMACHRBAR_HRBAP                           ETH_DMACHRBAR_HRBAP_Msk  /* Host receive buffer address pointer */


//-----------------------------------------------------------------------------
void ethernet_init(void);
void ethernet_deinit(void);
void ethernet_gpio_init(void);



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _ETHERNET_H_
