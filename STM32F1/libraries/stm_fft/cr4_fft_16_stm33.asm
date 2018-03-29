/*;******************** (C) COPYRIGHT 2009  STMicroelectronics ********************
;* File Name          : cr4_fft_64_stm32.s
;* Author             : MCD Application Team
;* Version            : V2.0.0
;* Date               : 04/27/2009
;* Description        : Optimized 64-point radix-4 complex FFT for Cortex-M3
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

.global cr4_fft_16_stm32
.extern TableFFT
  
.equ NPT, 16


/*;*******************************************************************************
;* Function Name  : cr4_fft_16_stm32
;* Description    : complex radix-4 16 points FFT
;* Input          : - R0 = pssOUT: Output array .
;*                  - R1 = pssIN: Input array 
;*                  - R2 = Nbin: = 16 number of points, this optimized FFT function
;*                    can only convert 16 points.
;* Output         : None 
;* Return         : None
;********************************************************************************/
.thumb_func
cr4_fft_16_stm32:

        STMFD   SP!, {R4-R11, LR}
        
        MOV r12, #0
        MOV r3, r0 
        MOV r0,#0
        
preloop_v7:
        ADD     r14, r1, r12, LSR#28
       
        LDRSH r5, [r14, #2]
        LDRSH r4, [r14],#NPT
        LDRSH r9, [r14, #2]
        LDRSH r8, [r14],#NPT
        LDRSH r7, [r14, #2]
        LDRSH r6, [r14],#NPT
        LDRSH r11, [r14, #2]
        LDRSH r10, [r14],#NPT

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
         
         CMP r0,#4
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
        /*
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
        */
        
.end       
/******************* (C) COPYRIGHT 2009  STMicroelectronics *****END OF FILE****/
