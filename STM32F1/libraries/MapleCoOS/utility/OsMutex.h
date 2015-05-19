/**
 *******************************************************************************
 * @file       mutex.h	
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Mutex management header file
 * @details    This file including some defines and declare related to mutex
 *             management.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
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
