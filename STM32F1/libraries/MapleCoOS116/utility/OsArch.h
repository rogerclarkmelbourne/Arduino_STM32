/**
 *******************************************************************************
 * @file       OsArch.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Implement function declare related to Cortex-M3(ARM-v7)
 * @details    This header file including functions or defines related to 
 *             Cortex-M3(ARM-v7).	 		
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


#ifndef  _CPU_H
#define  _CPU_H


#define NVIC_ST_CTRL    (*((volatile U32 *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile U32 *)0xE000E014))
#define RELOAD_VAL      ((U32)(( (U32)CFG_CPU_FREQ) / (U32)CFG_SYSTICK_FREQ) -1)

/*!< Initial System tick.	*/
// Maple
#define InitSysTick()		systick_attach_callback(&CoSysTick_Handler)
/* Maple
#define InitSysTick()   NVIC_ST_RELOAD =  RELOAD_VAL; \
                        NVIC_ST_CTRL   =  0x0007  
*/						

#define NVIC_SYS_PRI2   (*((volatile U32 *)0xE000ED1C))
#define NVIC_SYS_PRI3   (*((volatile U32 *)0xE000ED20))

/*!< Initialize PendSV,SVC and SysTick interrupt priority to lowest.          */

#define InitInt()       NVIC_SYS_PRI2 |=  0xFF000000;\
                        NVIC_SYS_PRI3 |=  0xFFFF0000


/*---------------------------- Variable declare ------------------------------*/
extern U64      OSTickCnt;          /*!< Counter for current system ticks.    */									

/*!< Initial context of task being created	*/
extern OS_STK  *InitTaskContext(FUNCPtr task,void *param,OS_STK *pstk);
extern void    SwitchContext(void);         /*!< Switch context                   */
extern void    SetEnvironment(OS_STK *pstk);/*!< Set environment for run          */
extern U8      Inc8 (volatile U8 *data);
extern U8      Dec8 (volatile U8 *data);
extern void    IRQ_ENABLE_RESTORE(void);
extern void    IRQ_DISABLE_SAVE(void);
extern void    CoSysTick_Handler(void);


#endif /* _CPU_H */
