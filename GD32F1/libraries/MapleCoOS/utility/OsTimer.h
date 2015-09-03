/**
 *******************************************************************************
 * @file       timer.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Timer manage header file	
 * @details    This file including some declares and defines related to timer 
 *             management.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
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
