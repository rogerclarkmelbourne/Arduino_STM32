/**
 *******************************************************************************
 * @file       OsMutex.h	
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Mutex management header file
 * @details    This file including some defines and declare related to mutex
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


#ifndef _MUTEX_H
#define _MUTEX_H

#include <coocox.h>

#if CFG_MUTEX_EN > 0

/*---------------------------- Resource status -------------------------------*/
#define   MUTEX_FREE        0           /*!< Mutex is free                    */
#define   MUTEX_OCCUPY      1           /*!< Mutex is occupy                  */
#define   WAITING_MUTEX     0x80

/**
 * @struct   Mutex  mutex.h 	
 * @brief    Mutex control block
 * @details  This struct is use to mutex management.	
 */
typedef struct Mutex
{
    U8       originalPrio;              /*!< Mutex priority.                  */
    U8       mutexFlag;                 /*!< Mutex flag.                      */
    OS_TID   taskID;                    /*!< Task ID.                         */	
    OS_TID   hipriTaskID;               /*!< Mutex ID.                        */
    P_OSTCB  waittingList;              /*!< waitting the Mutex.              */
}MUTEX,*P_MUTEX;


/*---------------------------- Variable declare ------------------------------*/
/*!< Table use to save mutex control block.                                   */
extern MUTEX      MutexTbl[CFG_MAX_MUTEX];
extern OS_MutexID MutexFreeID;      /*!< A pointer to next vliad resource ID. */


/*---------------------------- Function declare ------------------------------*/
extern void   RemoveMutexList(P_OSTCB ptcb);

#endif    /* CFG_MUTEX_EN  */

#endif    /* _MUTEX_H      */
