/**
 *******************************************************************************
 * @file       OsTimer.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Timer manage header file	
 * @details    This file including some declares and defines related to timer 
 *             management.
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

#ifndef _TIMER_H
#define _TIMER_H

#define   TMR_STATE_RUNNING     0       /*!< Timer State: Running             */
#define   TMR_STATE_STOPPED     1       /*!< Timer State: Stopped             */

/**
 * @struct   tmrCtrl  timer.h  	
 * @brief    Timer control block 
 * @details  This struct is use to manage user timer. 
 *	
 */
typedef struct tmrCtrl                  /* Timer Control Block Define.        */
{
    OS_TCID          tmrID;             /*!< Timer ID.                        */
    U8               tmrType;           /*!< Timer Type.                      */
    U8               tmrState;          /*!< Timer State.                     */
    U32              tmrCnt;            /*!< Timer Counter.                   */
    U32              tmrReload;         /*!< Timer Reload Counter Value.      */	
    vFUNCPtr         tmrCallBack; /*!< Call-back Function When Timer overrun. */	
    struct tmrCtrl*  tmrNext;       /*!< Point to Next Timer Control Block.   */
    struct tmrCtrl*  tmrPrev;       /*!< Point to Previous Timer Control Block*/

}TmrCtrl,*P_TmrCtrl;

/*---------------------------- Variable declare ------------------------------*/
extern P_TmrCtrl  TmrList;              /*!< A pointer to the timer list.     */ 
extern U32        TmrIDVessel;
/*---------------------------- Function declare ------------------------------*/
extern void  TmrDispose(void);          /*!< Timer counter function.          */
extern void  isr_TmrDispose(void);
#endif
