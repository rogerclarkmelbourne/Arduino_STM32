/*;******************** (C) COPYRIGHT 2009  STMicroelectronics ********************
;* File Name          : cr4_fft_256_stm32.s
;* Author             : MCD Application Team
;* Version            : V2.0.0
;* Date               : 04/27/2009
;* Description        : Optimized 256-point radix-4 complex FFT for Cortex-M3
;********************************************************************************
;* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
;* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
;* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
;* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
;* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
;* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************/

.cpu cortex-m3
.fpu softvfp   
.syntax unified
.thumb
.text	

.global cr4_fft_256_stm32
.extern TableFFT
  
.equ NPT, 256


/*;*******************************************************************************
;* Function Name  : cr4_fft_256_stm32
;* Description    : complex radix-4 256 points FFT
;* Input          : - R0 = pssOUT: Output array .
;*                  - R1 = pssIN: Input array 
;*                  - R2 = Nbin: =256 number of points, this optimized FFT function  
;*                    can only convert 256 points.
;* Output         : None 
;* Return         : None
;********************************************************************************/
.thumb_func
cr4_fft_256_stm32:

        STMFD   SP!, {R4-R11, LR}
        
        MOV r12, #0
        MOV r3, r0 
        MOV r0,#0
        
preloop_v7:
        ADD     r14, r1, r12, LSR#24 /*256pts*/
       
        LDRSH r5, [r14, #2]       
        LDRSH r4, [r14]
        ADD   r14, #NPT
        LDRSH r9, [r14, #2]
        LDRSH r8, [r14]
        ADD   r14, #NPT      
        LDRSH r7, [r14, #2]
        LDRSH r6, [r14]
        ADD   r14, #NPT        
        LDRSH r11, [r14, #2]
        LDRSH r10, [r14]
        ADD   r14, #NPT

        ADD     r8, r8, r10
        ADD     r9, r9, r11
        SUB     r10, r8, r10, LSL#1  
        SUB     r11, r9, r11, LSL#1  

        MOV     r4, r4, ASR#2
        MOV     r5, r5, ASR#2
        ADD     r4, r4, r6, ASR#2
        ADD     r5, r5, r7, ASR#2
        SUB     r6, r4, r6, ASR#1
        SUB     r7, r5, r7, ASR#1

        ADD     r4, r4, r8, ASR#2
        ADD     r5, r5, r9, ASR#2
        SUB     r8, r4, r8, ASR#1
        SUB     r9, r5, r9, ASR#1

        ADD     r6, r6, r11, ASR#2
        SUB     r7, r7, r10, ASR#2
        SUB     r11, r6, r11, ASR#1
        ADD     r10, r7, r10, ASR#1
   
        STRH    r5, [r3, #2]
        STRH    r4, [r3], #4
        STRH    r7, [r3, #2]
        STRH    r6, [r3], #4
        STRH    r9, [r3, #2]
        STRH    r8, [r3], #4
        STRH    r10, [r3, #2]  
        STRH    r11, [r3], #4
        
         ADD r0, r0, #1
         
         RBIT r12, r0 
         
         CMP r0,#64 /*256pts*/  
         BNE  preloop_v7

         SUB     r1, r3, r2, LSL#2
         MOV     r0, #16
         MOVS    r2, r2, LSR#4   

/*;------------------------------------------------------------------------------
;   The FFT coefficients table can be stored into Flash or RAM. 
;   The following two lines of code allow selecting the method for coefficients 
;   storage. 
;   In the case of choosing coefficients in RAM, you have to:
;   1. Include the file table_fft.h, which is a part of the DSP library, 
;      in your main file.
;   2. Decomment the line LDR.W pssK, =TableFFT and comment the line 
;      ADRL    pssK, TableFFT_V7
;   3. Comment all the TableFFT_V7 data.
;------------------------------------------------------------------------------*/
         ADR    r3, TableFFT_V7
         /*LDR.W r3, =TableFFT*/


passloop_v7:
         STMFD   SP!, {r1,r2}
         ADD     r12, r0, r0, LSL#1
         ADD     r1, r1, r12
         SUB     r2, r2, #1<<16

grouploop_v7:
         ADD     r2,r2,r0,LSL#(16-2)

butterloop_v7:
        		
         LDRSH r5, [r1, #2]
         LDRSH r4, [r1]
         SUB r1, r1, r0

      	LDRSH r11, [r3, #2]
      	LDRSH r10, [r3]
      	ADD r3, r3, #4

         SUB  r14, r5, r4         
         MUL  r12, r14, r11        
         ADD  r14, r10, r11, LSL#1  
         MLA  r11, r5, r10, r12     
         MLA  r10, r4, r14, r12   

         LDRSH r5, [r1, #2]
         LDRSH r4, [r1]
         SUB r1, r1, r0
				
      	LDRSH r9, [r3, #2]
      	LDRSH r8, [r3]
      	ADD r3, r3, #4
        
         SUB  r14, r5, r4         
         MUL  r12, r14, r9        
         ADD  r14, r8, r9, LSL#1  
         MLA  r9, r5, r8, r12     
         MLA  r8, r4, r14, r12   
	
         LDRSH r5, [r1, #2]
         LDRSH r4, [r1]
         SUB r1, r1, r0
				
         LDRSH r7, [r3, #2]
      	LDRSH r6, [r3]
      	ADD r3, r3, #4
		
         SUB  r14, r5, r4        
         MUL  r12, r14, r7        
         ADD  r14, r6, r7, LSL#1  
         MLA  r7, r5, r6, r12     
         MLA  r6, r4, r14, r12   
		
         LDRSH r5, [r1, #2]
      	LDRSH r4, [r1]
    		
         ADD     r8, r8, r10
         ADD     r9, r9, r11
         SUB     r10, r8, r10, LSL#1
         SUB     r11, r9, r11, LSL#1

         MOV     r4, r4, ASR#2
         MOV     r5, r5, ASR#2
         ADD     r4, r4, r6, ASR#(2+14)
         ADD     r5, r5, r7, ASR#(2+14)
         SUB     r6, r4, r6, ASR#(1+14)
         SUB     r7, r5, r7, ASR#(1+14)

         ADD     r4, r4, r8, ASR#(2+14)
         ADD     r5, r5, r9, ASR#(2+14)
         SUB     r8, r4, r8, ASR#(1+14)
         SUB     r9, r5, r9, ASR#(1+14)

         ADD     r6, r6, r11, ASR#(2+14)
         SUB     r7, r7, r10, ASR#(2+14)
         SUB     r11, r6, r11, ASR#(1+14)
         ADD     r10, r7, r10, ASR#(1+14)      

         STRH    r5, [r1, #2]
         STRH    r4, [r1]
         ADD 	r1, r1, r0
         STRH    r7, [r1, #2]
         STRH    r6, [r1]
         ADD     r1, r1, r0
         STRH    r9, [r1, #2]
         STRH    r8, [r1]
         ADD     r1, r1, r0
         STRH    r10, [r1, #2]  
         STRH    r11, [r1], #4
         SUBS        r2,r2, #1<<16
         BGE     butterloop_v7
         ADD     r12, r0, r0, LSL#1
         ADD     r1, r1, r12

         SUB     r2, r2, #1
         MOVS    r14, r2, LSL#16
         IT      ne
         SUBNE   r3, r3, r12
         BNE     grouploop_v7

         LDMFD   sp!, {r1, r2}
         MOV  r0,r0,LSL#2		
         MOVS    r2, r2, LSR#2
       	BNE     passloop_v7
       	LDMFD   SP!, {R4-R11, PC}


TableFFT_V7:
        /*N=16*/
        .short 0x4000,0x0000, 0x4000,0x0000, 0x4000,0x0000
        .short 0xdd5d,0x3b21, 0x22a3,0x187e, 0x0000,0x2d41
        .short 0xa57e,0x2d41, 0x0000,0x2d41, 0xc000,0x4000
        .short 0xdd5d,0xe782, 0xdd5d,0x3b21, 0xa57e,0x2d41
        /*N=64*/
        .short 0x4000,0x0000, 0x4000,0x0000, 0x4000,0x0000
        .short 0x2aaa,0x1294, 0x396b,0x0646, 0x3249,0x0c7c
        .short 0x11a8,0x238e, 0x3249,0x0c7c, 0x22a3,0x187e
        .short 0xf721,0x3179, 0x2aaa,0x1294, 0x11a8,0x238e
        .short 0xdd5d,0x3b21, 0x22a3,0x187e, 0x0000,0x2d41
        .short 0xc695,0x3fb1, 0x1a46,0x1e2b, 0xee58,0x3537
        .short 0xb4be,0x3ec5, 0x11a8,0x238e, 0xdd5d,0x3b21
        .short 0xa963,0x3871, 0x08df,0x289a, 0xcdb7,0x3ec5
        .short 0xa57e,0x2d41, 0x0000,0x2d41, 0xc000,0x4000
        .short 0xa963,0x1e2b, 0xf721,0x3179, 0xb4be,0x3ec5
        .short 0xb4be,0x0c7c, 0xee58,0x3537, 0xac61,0x3b21
        .short 0xc695,0xf9ba, 0xe5ba,0x3871, 0xa73b,0x3537
        .short 0xdd5d,0xe782, 0xdd5d,0x3b21, 0xa57e,0x2d41
        .short 0xf721,0xd766, 0xd556,0x3d3f, 0xa73b,0x238e
        .short 0x11a8,0xcac9, 0xcdb7,0x3ec5, 0xac61,0x187e
        .short 0x2aaa,0xc2c1, 0xc695,0x3fb1, 0xb4be,0x0c7c
        /*N=256*/
        .short 0x4000,0x0000, 0x4000,0x0000, 0x4000,0x0000
        .short 0x3b1e,0x04b5, 0x3e69,0x0192, 0x3cc8,0x0324
        .short 0x35eb,0x0964, 0x3cc8,0x0324, 0x396b,0x0646
        .short 0x306c,0x0e06, 0x3b1e,0x04b5, 0x35eb,0x0964
        .short 0x2aaa,0x1294, 0x396b,0x0646, 0x3249,0x0c7c
        .short 0x24ae,0x1709, 0x37af,0x07d6, 0x2e88,0x0f8d
        .short 0x1e7e,0x1b5d, 0x35eb,0x0964, 0x2aaa,0x1294
        .short 0x1824,0x1f8c, 0x341e,0x0af1, 0x26b3,0x1590
        .short 0x11a8,0x238e, 0x3249,0x0c7c, 0x22a3,0x187e
        .short 0x0b14,0x2760, 0x306c,0x0e06, 0x1e7e,0x1b5d
        .short 0x0471,0x2afb, 0x2e88,0x0f8d, 0x1a46,0x1e2b
        .short 0xfdc7,0x2e5a, 0x2c9d,0x1112, 0x15fe,0x20e7
        .short 0xf721,0x3179, 0x2aaa,0x1294, 0x11a8,0x238e
        .short 0xf087,0x3453, 0x28b2,0x1413, 0x0d48,0x2620
        .short 0xea02,0x36e5, 0x26b3,0x1590, 0x08df,0x289a
        .short 0xe39c,0x392b, 0x24ae,0x1709, 0x0471,0x2afb
        .short 0xdd5d,0x3b21, 0x22a3,0x187e, 0x0000,0x2d41
        .short 0xd74e,0x3cc5, 0x2093,0x19ef, 0xfb8f,0x2f6c
        .short 0xd178,0x3e15, 0x1e7e,0x1b5d, 0xf721,0x3179
        .short 0xcbe2,0x3f0f, 0x1c64,0x1cc6, 0xf2b8,0x3368
        .short 0xc695,0x3fb1, 0x1a46,0x1e2b, 0xee58,0x3537
        .short 0xc197,0x3ffb, 0x1824,0x1f8c, 0xea02,0x36e5
        .short 0xbcf0,0x3fec, 0x15fe,0x20e7, 0xe5ba,0x3871
        .short 0xb8a6,0x3f85, 0x13d5,0x223d, 0xe182,0x39db
        .short 0xb4be,0x3ec5, 0x11a8,0x238e, 0xdd5d,0x3b21
        .short 0xb140,0x3daf, 0x0f79,0x24da, 0xd94d,0x3c42
        .short 0xae2e,0x3c42, 0x0d48,0x2620, 0xd556,0x3d3f
        .short 0xab8e,0x3a82, 0x0b14,0x2760, 0xd178,0x3e15
        .short 0xa963,0x3871, 0x08df,0x289a, 0xcdb7,0x3ec5
        .short 0xa7b1,0x3612, 0x06a9,0x29ce, 0xca15,0x3f4f
        .short 0xa678,0x3368, 0x0471,0x2afb, 0xc695,0x3fb1
        .short 0xa5bc,0x3076, 0x0239,0x2c21, 0xc338,0x3fec
        .short 0xa57e,0x2d41, 0x0000,0x2d41, 0xc000,0x4000
        .short 0xa5bc,0x29ce, 0xfdc7,0x2e5a, 0xbcf0,0x3fec
        .short 0xa678,0x2620, 0xfb8f,0x2f6c, 0xba09,0x3fb1
        .short 0xa7b1,0x223d, 0xf957,0x3076, 0xb74d,0x3f4f
        .short 0xa963,0x1e2b, 0xf721,0x3179, 0xb4be,0x3ec5
        .short 0xab8e,0x19ef, 0xf4ec,0x3274, 0xb25e,0x3e15
        .short 0xae2e,0x1590, 0xf2b8,0x3368, 0xb02d,0x3d3f
        .short 0xb140,0x1112, 0xf087,0x3453, 0xae2e,0x3c42
        .short 0xb4be,0x0c7c, 0xee58,0x3537, 0xac61,0x3b21
        .short 0xb8a6,0x07d6, 0xec2b,0x3612, 0xaac8,0x39db
        .short 0xbcf0,0x0324, 0xea02,0x36e5, 0xa963,0x3871
        .short 0xc197,0xfe6e, 0xe7dc,0x37b0, 0xa834,0x36e5
        .short 0xc695,0xf9ba, 0xe5ba,0x3871, 0xa73b,0x3537
        .short 0xcbe2,0xf50f, 0xe39c,0x392b, 0xa678,0x3368
        .short 0xd178,0xf073, 0xe182,0x39db, 0xa5ed,0x3179
        .short 0xd74e,0xebed, 0xdf6d,0x3a82, 0xa599,0x2f6c
        .short 0xdd5d,0xe782, 0xdd5d,0x3b21, 0xa57e,0x2d41
        .short 0xe39c,0xe33a, 0xdb52,0x3bb6, 0xa599,0x2afb
        .short 0xea02,0xdf19, 0xd94d,0x3c42, 0xa5ed,0x289a
        .short 0xf087,0xdb26, 0xd74e,0x3cc5, 0xa678,0x2620
        .short 0xf721,0xd766, 0xd556,0x3d3f, 0xa73b,0x238e
        .short 0xfdc7,0xd3df, 0xd363,0x3daf, 0xa834,0x20e7
        .short 0x0471,0xd094, 0xd178,0x3e15, 0xa963,0x1e2b
        .short 0x0b14,0xcd8c, 0xcf94,0x3e72, 0xaac8,0x1b5d
        .short 0x11a8,0xcac9, 0xcdb7,0x3ec5, 0xac61,0x187e
        .short 0x1824,0xc850, 0xcbe2,0x3f0f, 0xae2e,0x1590
        .short 0x1e7e,0xc625, 0xca15,0x3f4f, 0xb02d,0x1294
        .short 0x24ae,0xc44a, 0xc851,0x3f85, 0xb25e,0x0f8d
        .short 0x2aaa,0xc2c1, 0xc695,0x3fb1, 0xb4be,0x0c7c
        .short 0x306c,0xc18e, 0xc4e2,0x3fd4, 0xb74d,0x0964
        .short 0x35eb,0xc0b1, 0xc338,0x3fec, 0xba09,0x0646
        .short 0x3b1e,0xc02c, 0xc197,0x3ffb, 0xbcf0,0x0324
        

.end       
/******************* (C) COPYRIGHT 2009  STMicroelectronics *****END OF FILE****/
