/**
 *******************************************************************************
 * @file       queue.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Queue management header file	
 * @details    This file including some defines and declares about queue management.
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef _QUEUE_H
#define _QUEUE_H


/**
 * @struct   Queue  queue.h	
 * @brief    Queue struct
 * @details  This struct use to manage queue.
 *	
 */
typedef struct Queue
{
    void    **qStart;                   /*!<                                  */
    U8      id;                         /*!<                                  */
    U16     head;                       /*!< The header of queue              */
    U16     tail;                       /*!< The end of queue                 */
    U16     qMaxSize;                   /*!< The max size of queue            */
    U16     qSize;                      /*!< Current size of queue            */
}QCB,*P_QCB;


#endif
