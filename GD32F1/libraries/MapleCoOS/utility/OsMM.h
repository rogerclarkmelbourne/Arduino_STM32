/**
 *******************************************************************************
 * @file       mm.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Header file	related to memory management 
 * @details    This file including some defines and function declare related to 
 *             memory management. 	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef  _MM_H
#define  _MM_H


typedef struct Memory
{
    U8*   memAddr;
    U8*   freeBlock;
    U32   blockSize;
    U32   blockNum;			
}MM,*P_MM;


typedef struct MemoryBlock
{
    struct MemoryBlock* nextBlock;
}MemBlk,*P_MemBlk;


extern U32  MemoryIDVessel;

#endif   /* _MM_H */

