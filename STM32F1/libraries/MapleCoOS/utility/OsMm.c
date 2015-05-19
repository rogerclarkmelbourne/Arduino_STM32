/**
 *******************************************************************************
 * @file       mm.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      memory management implementation code of CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


/*---------------------------- Include ---------------------------------------*/
#include <coocox.h>


#if  CFG_MM_EN > 0
/*---------------------------- Variable Define -------------------------------*/
MM    MemoryTbl[CFG_MAX_MM] = {{0}};/*!< Table which save memory control block. */
U32   MemoryIDVessel = 0;         /*!< Memory ID container.                   */

/**
 *******************************************************************************
 * @brief      Create a memory partition	 
 * @param[in]  memBuf       Specify memory partition head address.		 
 * @param[in]  blockSize    Specify memory block size.  
 * @param[in]  blockNum     Specify memory block number.
 * @param[out] None
 * @retval     E_CREATE_FAIL  Create memory partition fail.
 * @retval     others         Create memory partition successful.			 
 *
 * @par Description
 * @details    This function is called to create a memory partition.
 *******************************************************************************
 */
OS_MMID CoCreateMemPartition(U8* memBuf,U32 blockSize,U32 blockNum)
{
    U8        i,j;
    U8        *memory;
    P_MemBlk  memBlk;
    memory = memBuf;
	
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(memBuf == NULL)
    {
        return 	E_CREATE_FAIL;
    }
    if(blockSize == 0)
    {
        return 	E_CREATE_FAIL;	
    }
    if((blockSize&0x3) != 0)
    {
        return 	E_CREATE_FAIL;	
    }
    if(blockNum<=1)
    {
        return 	E_CREATE_FAIL;
    }
#endif

    OsSchedLock();                      /* Lock schedule                      */
    for(i = 0; i < CFG_MAX_MM; i++)
    {
        if((MemoryIDVessel & (1 << i)) == 0)  /* Is free memory ID?           */
        {
            MemoryIDVessel |= (1<<i);   /* Yes,assign ID to this memory block */
            OsSchedUnlock();            /* Unlock schedule                    */
            MemoryTbl[i].memAddr   = memory;/* Initialize memory control block*/
            MemoryTbl[i].freeBlock = memory;  	
            MemoryTbl[i].blockSize = blockSize;
            MemoryTbl[i].blockNum  = blockNum;
            memBlk  = (P_MemBlk)memory;     /* Bulid list in this memory block*/ 
            for(j=0;j<blockNum-1;j++)
            {
                memory = memory+blockSize;
                memBlk->nextBlock = (P_MemBlk)memory;
                memBlk = memBlk->nextBlock;
            }
            memBlk->nextBlock = NULL;
            return i;                   /* Return memory block ID             */
        }
    }
    OsSchedUnlock();                    /* Unlock schedule                    */
    return E_CREATE_FAIL;               /* Error return                       */
}


/**
 *******************************************************************************
 * @brief      Delete a memory partition	  
 * @param[in]  mmID     Specify	memory partition that want to delete.	
 * @param[out] None
 * @retval     E_INVALID_ID   The memory partition id passed was invalid,delete fail.
 * @retval     E_OK           Delete successful.			 
 *
 * @par Description
 * @details    This function is called to Delete a memory partition.
 *******************************************************************************
 */
StatusType CoDelMemoryPartition(OS_MMID mmID)
{
    P_MM  memCtl;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(mmID >= CFG_MAX_MM)
    {
        return E_INVALID_ID;
    }
    if( ((1<<mmID)&MemoryIDVessel) == 0)
    {
        return E_INVALID_ID;
    }
#endif	
    OsSchedLock();                      /* Lock schedule                      */
    memCtl = &MemoryTbl[mmID];          /* Release memory control block       */
    MemoryIDVessel &= ~(1<<mmID);
    OsSchedUnlock();                    /* Unlock schedule                    */
    
    memCtl->memAddr   = NULL;
    memCtl->freeBlock = NULL;	
    memCtl->blockSize = 0;
    memCtl->blockNum  = 0;	
    return E_OK;                        /* Return OK                          */
}


/**
 *******************************************************************************
 * @brief      Get free block number in a memory partition	  
 * @param[in]  mmID    Specify memory partition.	
 *
 * @param[out] E_INVALID_ID  Invalid ID was passed and get counter failure.	  
 * @param[out] E_OK          Get current counter successful.
 * @retval     fbNum         The number of free block.	
 *
 * @par Description
 * @details    This function is called to get free block number in a memory 
 *             partition.
 *******************************************************************************
 */
U32 CoGetFreeBlockNum(OS_MMID mmID,StatusType* perr)
{
    U32       fbNum;	
    P_MM      memCtl;
    P_MemBlk  memBlk;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(mmID >= CFG_MAX_MM)
    {
        *perr = E_INVALID_ID;
        return 0;
    }
    if( ((1<<mmID)&MemoryIDVessel) == 0)
    {
        *perr = E_INVALID_ID;           /* Invalid memory id,return 0         */
        return 0;
    }
#endif	
    memCtl = &MemoryTbl[mmID];
    OsSchedLock();                      /* Lock schedule                      */
    memBlk = (P_MemBlk)(memCtl->freeBlock);/* Get the free item in memory list*/
    fbNum  = 0;
    while(memBlk != NULL)               /* Get counter of free item           */
    {
        fbNum++;
        memBlk = memBlk->nextBlock;     /* Get next free iterm                */
    }
    OsSchedUnlock();                    /* Unlock schedul                     */
    *perr = E_OK;							   
    return fbNum;                       /* Return the counter of free item    */
}


/**
 *******************************************************************************
 * @brief      Get a memory buffer from memory partition	    
 * @param[in]  mmID     Specify	memory partition that want to assign buffer.	
 * @param[out] None
 * @retval     NULL     Assign buffer fail.
 * @retval     others   Assign buffer successful,and return the buffer pointer.	
 *		 
 * @par Description
 * @details    This function is called to Delete a memory partition.
 *******************************************************************************
 */
void* CoGetMemoryBuffer(OS_MMID mmID)
{
    P_MM      memCtl;
    P_MemBlk  memBlk;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(mmID >= CFG_MAX_MM)
    {
        return NULL;
    }
    if( ((1<<mmID)&MemoryIDVessel)  == 0)
    {
        return NULL;
    }
#endif
    memCtl = &MemoryTbl[mmID];	
    OsSchedLock();                      /* Lock schedule                      */	        
    if(memCtl->freeBlock == NULL )    /* Is there no free item in memory list */
    {
        OsSchedUnlock();                /* Unlock schedule                    */
        return NULL;                    /* Yes,error return                   */
    }
    memBlk = (P_MemBlk)memCtl->freeBlock;       /* Get free memory block      */
    memCtl->freeBlock = (U8*)memBlk->nextBlock; /* Reset the first free item  */
    OsSchedUnlock();                    /* Unlock schedule                    */
    return memBlk;                      /* Return free memory block address   */
}



/**
 *******************************************************************************
 * @brief      Free a memory buffer to memory partition	 
 * @param[in]  mmID    Specify	memory partition.
 * @param[in]  buf     Specify	memory buffer that want to free.	
 * @param[out] None
 * @retval     E_INVALID_ID          The memory partition id passed was invalid.
 * @retval     E_INVALID_PARAMETER   The parameter passed was invalid.	
 * @retval     E_OK                  Free successful.	 
 *
 * @par Description
 * @details    This function is called to Delete a memory partition.
 *******************************************************************************
 */
StatusType CoFreeMemoryBuffer(OS_MMID mmID,void* buf)
{
    P_MM      memCtl;
    P_MemBlk  memBlk;
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(mmID >= CFG_MAX_MM)
    {
        return E_INVALID_ID;
    }
    if( ((1<<mmID)&MemoryIDVessel) == 0)
    {
        return E_INVALID_ID;
    }
    if(buf == NULL)
    {
        return E_INVALID_PARAMETER;
    }
#endif	

    memCtl = &MemoryTbl[mmID];
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if((U32)buf < (U32)(memCtl->memAddr))
    {
        return E_INVALID_PARAMETER;
    }
    if((U32)buf > (U32)(memCtl->memAddr + memCtl->blockSize*memCtl->blockNum))
    {
        return E_INVALID_PARAMETER;
    }
    if(((U32)buf - (U32)(memCtl->memAddr))%(memCtl->blockSize) != 0)
    {	
        return E_INVALID_PARAMETER;	
    }
#endif
    memBlk = (P_MemBlk)buf;             /* Reset the first free item          */
    OsSchedLock();
    memBlk->nextBlock = (P_MemBlk)memCtl->freeBlock;
    memCtl->freeBlock = buf;
    OsSchedUnlock();
    return E_OK;                        /* Return OK                          */
}

#endif

