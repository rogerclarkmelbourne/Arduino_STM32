/**
 *******************************************************************************
 * @file       prot.c
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Compiler adapter for CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met: 
 *  
 *      * Redistributions of source code must retain the above copyright 
 *  notice, this list of conditions and the following disclaimer. 
 *      * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution. 
 *      * Neither the name of the <ORGANIZATION> nor the names of its 
 *  contributors may be used to endorse or promote products derived 
 *  from this software without specific prior written permission. 
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * <h2><center>&copy; COPYRIGHT 2014 CooCox </center></h2>
 *******************************************************************************
 */ 

/*---------------------------- Include ---------------------------------------*/
#include "coocox.h"


//******************************************************************************
//                              EQUATES
//******************************************************************************	
U32 NVIC_INT_CTRL  = 0xE000ED04;            // Interrupt control state register
U32 NVIC_PENDSVSET = 0x10000000;            // Value to trigger PendSV exception
U32 INT_EXIT       = 0xFFFFFFFC;

//******************************************************************************
//                         PUBLIC FUNCTIONS
//******************************************************************************
extern U8     Inc8(volatile U8 *data) ;
extern U8     Dec8(volatile U8 *data) ; 
extern void   IRQ_ENABLE_RESTORE(void);
extern void   IRQ_DISABLE_SAVE(void);
extern void   SetEnvironment(OS_STK *pstk) __attribute__ ((naked)); 	
extern void   SwitchContext(void)          __attribute__ ((naked));
extern void   PendSV_Handler(void)         __attribute__ ((naked));


/**
 ******************************************************************************
 * @brief      Plus a byte integers and Saved into memory cell
 * @param[in]  data    byte integers.	 
 * @param[out] None  
 * @retval     Returns Original value.		 
 *
 * @par Description
 * @details    This function is called to Plus a byte integers 
 *             and Saved into memory cell.
 ******************************************************************************
 */
U8 Inc8 (volatile U8 *data)
{
  register U8  result = 0;
  
  __asm volatile 
  (
      " PUSH    {R1}     \n"
      " CPSID   I        \n"
      " LDRB    R1,[%1]  \n"
      " ADD     R1,#1    \n"
      " STRB    R1,[%1]  \n"
      " CPSIE   I        \n"
      " SUB     R1,#0x1  \n"
      " MOVS    %0,R1    \n"
      " POP     {R1}     \n"
      :"=r"(result)
      :"r"(data)
  );
  return (result);
  
}
 

/**
 ******************************************************************************
 * @brief      Decrease a byte integers and Saved into memory cell
 * @param[in]  data    byte integers.	 
 * @param[out] None  
 * @retval     Returns Original value.		 
 *
 * @par Description
 * @details    This function is called to Decrease a byte integers 
 *             and Saved into memory cell.
 ******************************************************************************
 */
U8 Dec8 (volatile U8 *data)
{
  register U8  result = 0;
  __asm volatile 
  (
      " PUSH    {R1}     \n"
      " CPSID   I        \n"
      " LDRB    R1,[%1]  \n"
      " SUB     R1,#1    \n"
      " STRB    R1,[%1]  \n"
      " CPSIE   I        \n"
      " MOVS    %0,R1    \n"
      " POP     {R1}     \n"
      :"=r"(result)
      :"r"(data)
  ); 
  return (result); 
}

/**
 ******************************************************************************
 * @brief      ENABLE Interrupt
 * @param[in]  None	 
 * @param[out] None  
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called to ENABLE Interrupt.
 ******************************************************************************
 */
void IRQ_ENABLE_RESTORE(void)
{ 
  __asm volatile 
  (
      " CPSIE   I        \n"
  );	
  return;
}

/**
 ******************************************************************************
 * @brief      Close Interrupt
 * @param[in]  None	 
 * @param[out] None  
 * @retval     None		 
 *
 * @par Description
 * @details    This function is called to close Interrupt.
 ******************************************************************************
 */
void IRQ_DISABLE_SAVE(void)
{  
  __asm volatile 
  (
      " CPSID   I        \n"
  );	
  return;
}


/**
 ******************************************************************************
 * @brief      Set environment	for Coocox OS running
 * @param[in]  pstk    stack pointer	 
 * @param[out] None  
 * @retval     None.		 
 *
 * @par Description
 * @details    This function is called to Set environment
 *              for Coocox OS running.
 ******************************************************************************
 */
void SetEnvironment (OS_STK *pstk)
{
    __asm volatile
    (
#if CFG_CHIP_TYPE == 3
       " SUB    R0,#100 \n"
#else
       " SUB    R0,#28 \n"
#endif
       " MSR    PSP,R0 \n"
       " BX      LR               \n"
    );
  
}


/**
 ******************************************************************************
 * @brief      Do ready work to Switch Context for task change
 * @param[in]  None	 
 * @param[out] None  
 * @retval     None.		 
 *
 * @par Description
 * @details    This function is called to Do ready work to 
 *              Switch Context for task change
 ******************************************************************************
 */
void SwitchContext(void)
{
  __asm volatile
  (
      " LDR     R3,=NVIC_INT_CTRL  \n"
	  " LDR     R3,[R3]            \n"
	  " LDR     R2,=NVIC_PENDSVSET \n"
	  " LDR     R1,[R2]            \n"
      " STR     R1, [R3]           \n"
      " BX      LR               \n"   
  );
 
}



/**
 ******************************************************************************
 * @brief      Switch Context for task change
 * @param[in]  None	 
 * @param[out] None  
 * @retval     None.		 
 *
 * @par Description
 * @details    This function is called to Switch Context for task change.
 ******************************************************************************
 */
#if CFG_CHIP_TYPE == 2
// Maple
void __exc_pendsv(void)
{
  __asm volatile
 (
    " LDR     R3,=TCBRunning   \n"
    " LDR     R1,[R3]          \n"   // R1 == running tcb
    " LDR     R2,=TCBNext      \n"
    " LDR     R2,[R2]          \n"   // R2 == next tcb

    " CMP     R1,R2            \n"
    " BEQ     exitPendSV       \n"          
    " MRS     R0, PSP          \n"    // Get PSP point (can not use PUSH,in ISR,SP is MSP )
  
    " SUB     R0,R0,#32        \n"
    " STR     R0,[R1]          \n"    // Save orig PSP
								                      // Store r4-r11,r0 -= regCnt * 4,r0 is new stack 
                                      // top point (addr h->l r11,r10,...,r5,r4)
    " STMIA   R0!,{R4-R7}      \n"    // Save old context (R4-R7)
    " MOV     R4,R8            \n"
    " MOV     R5,R9            \n"
    " MOV     R6,R10           \n"
    " MOV     R7,R11           \n"
    " STMIA   R0!,{R4-R7}      \n"    // Save old context (R8-R11)

    " popStk:                  \n" 
    " STR     R2, [R3]         \n"    // TCBRunning  = TCBNext;
    " LDR     R0, [R2]         \n"    // Get SP of task that be switch into.
	
    " ADD     R0,R0,#16        \n"
    " LDMIA   R0!,{R4-R7}      \n"    // Restore new Context (R8-R11)
    " MOV     R8,R4            \n"
    " MOV     R9,R5            \n"
    " MOV     R10,R6           \n"
    " MOV     R11,R7           \n"
    " SUB     R0,R0,#32        \n"
    " LDMIA   R0!,{R4-R7}      \n"    // Restore new Context (R4-R7)
    " ADD     R0,R0,#16        \n"
    " MSR     PSP, R0          \n"    // Mov new stack point to PSP

    " exitPendSV:              \n"
	" LDR    R3,=OSSchedLock   \n"
	" MOV    R0, #0x0          \n"
	" STRB   R0, [R3]          \n"

    " LDR     R3,=INT_EXIT     \n"
	" LDR     R0, [R3]         \n"
    " BX      R0               \n"    // Exit interrupt
  );
}
#endif

#if CFG_CHIP_TYPE == 1 || 3 == CFG_CHIP_TYPE
// Maple
void __exc_pendsv(void)
{
////////debug block /////////////////////////
  __asm volatile
 (
    " LDR    R3,=TCBRunning \n" 
    " LDR    R1,[R3]        \n"    // R1 == running tcb
    " LDR    R2,=TCBNext    \n"
    " LDR    R2,[R2]        \n"    // R2 == next tcb
    
    " CMP    R1,R2          \n"
    " BEQ    exitPendSV     \n"              
    
    " MRS    R0, PSP        \n"    // Get PSP point (can not use PUSH,in ISR,SP is MSP )
    " STMDB  R0!,{R4-R11}   \n"    // Store r4-r11,r0 -= regCnt * 4,r0 is new stack 
                                   // top point (addr h->l r11,r10,...,r5,r4)
#if CFG_CHIP_TYPE == 3
    " VSTMDB R0!, {S16-S31}   \n" // store remaining FPU registers
#endif // CFG_FPU_ENABLE

    " STR    R0,[R1]        \n"    // Save orig PSP
     
    " STR    R2, [R3]       \n"    // TCBRunning  = TCBNext;
    " LDR    R0, [R2]       \n"    // Get SP of task that be switch into.

#if CFG_CHIP_TYPE == 3
    " VLDMIA R0!, {S16-S31}   \n" // load remaining FPU registers
#endif // CFG_FPU_ENABLE

    " LDMIA  R0!,{R4-R11}   \n"    // POP {R4-R11},R0 += regCnt * 4
    " MSR    PSP, R0        \n"    // Mov new stack point to PSP

    " exitPendSV:           \n"
	" LDR    R3,=OSSchedLock\n"
	" MOVS   R0, #0x0       \n"
	" STRB   R0, [R3]       \n"
    " ORR    LR,LR,#0x04    \n"    // Ensure exception return uses process stack
#if CFG_CHIP_TYPE == 3
    " LDR    LR,=0xFFFFFFED \n"
#endif
    " BX     LR             \n"    // Exit interrupt
  );
}
#endif
