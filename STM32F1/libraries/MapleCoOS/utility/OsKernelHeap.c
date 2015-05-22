/**
 *******************************************************************************
 * @file       kernelHeap.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      kernel heap management implementation code of CooCox CoOS kernel.	
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


#if CFG_KHEAP_EN >0
/*---------------------------- Variable Define -------------------------------*/
U32     KernelHeap[KHEAP_SIZE] = {0};   /*!< Kernel heap                      */
P_FMB   FMBlist = NULL;                 /*!< Free memory block list           */
KHeap   Kheap   = {0};                  /*!< Kernel heap control              */


/*---------------------------- Function Declare ------------------------------*/
static P_FMB  GetPreFMB(P_UMB usedMB);
/**
 *******************************************************************************
 * @brief      Create kernel heap	 
 * @param[in]  None
 * @param[out] None
 * @retval     None			 
 *
 * @par Description
 * @details    This function is called to create kernel heap.
 *******************************************************************************
 */
void CoCreateKheap(void)
{
    Kheap.startAddr  = (U32)(KernelHeap); /* Initialize kernel heap control   */
    Kheap.endAddr    = (U32)(KernelHeap) + KHEAP_SIZE*4;
    FMBlist          = (P_FMB)KernelHeap; /* Initialize free memory block list*/
    FMBlist->nextFMB = NULL;	
    FMBlist->nextUMB = NULL;
    FMBlist->preUMB  = NULL;
}


/**
 *******************************************************************************
 * @brief      Allocation size bytes of memory block from kernel heap.
 * @param[in]  size     Length of menory block.	
 * @param[out] None
 * @retval     NULL     Allocate fail.
 * @retval     others   Pointer to memory block.		 
 *
 * @par Description
 * @details    This function is called to allocation size bytes of memory block.
 *******************************************************************************
 */
void* CoKmalloc(U32 size)
{
    P_FMB freeMB,newFMB,preFMB;
    P_UMB usedMB,tmpUMB;
    U8*   memAddr;
    U32   freeSize;
    U32   kheapAddr;
    
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if( size == 0 )
    {
        return NULL;
    }
#endif

    /* Word alignment,and add used memory head size */
    size      = (((size+3)>>2)<<2) + 8;
    kheapAddr = Kheap.endAddr;        /* Get the end address of kernel heap   */
    OsSchedLock();                    /* Lock schedule                        */
    freeMB = FMBlist;                 /* Get first item of free memory list   */
    preFMB = NULL;
    while(freeMB != NULL )            /* Is out of free memory list?          */
    {                                 /* No                                   */
        if(freeMB->nextUMB == NULL)   /* Is last item of free memory list?    */
        {                             /* Yes,get size for this free item      */
            freeSize = kheapAddr - (U32)(freeMB);
        }
        else                          /* No,get size for this free item       */
        {							   
            freeSize = (U32)(freeMB->nextUMB) -1 - (U32)(freeMB);	
        }
        if(freeSize >= size)        /* If the size equal or greater than need */
        {                           /* Yes,assign in this free memory         */
            usedMB=(P_UMB)freeMB;/* Get the address for used memory block head*/
            
            /* Get the address for used memory block                          */
            memAddr = (U8*)((U32)(usedMB) + 8);	
            
            /* Is left size of free memory smaller than 12?                   */	 
            if((freeSize-size) < 12)	 
            {		
                /* Yes,malloc together(12 is the size of the header information 
                   of free memory block ).                                    */
                if(preFMB != NULL)/* Is first item of free memory block list? */
                {                             /* No,set the link for list     */
                    preFMB->nextFMB = freeMB->nextFMB;
                }
                else                          /* Yes,reset the first item     */
                {						
                    FMBlist = freeMB->nextFMB;		
                }
                
                if(freeMB->nextUMB != NULL)   /* Is last item?                */
                {                             /* No,set the link for list     */
                    tmpUMB = (P_UMB)((U32)(freeMB->nextUMB)-1); 
                    tmpUMB->preMB = (void*)((U32)usedMB|0x1);
                }
                
                usedMB->nextMB = freeMB->nextUMB;/* Set used memory block link*/
                usedMB->preMB  = freeMB->preUMB;
            }
            else                            /* No,the left size more than 12  */
            {		
                /* Get new free memory block address                          */
                newFMB = (P_FMB)((U32)(freeMB) + size);
                
                if(preFMB != NULL)/* Is first item of free memory block list? */ 
                {						
                    preFMB->nextFMB = newFMB; /* No,set the link for list     */		
                }	
                else
                {					    
                    FMBlist = newFMB;         /* Yes,reset the first item     */	
                }
                
                /* Set link for new free memory block                         */
                newFMB->preUMB  = (P_UMB)((U32)usedMB|0x1);
                newFMB->nextUMB = freeMB->nextUMB;
                newFMB->nextFMB = freeMB->nextFMB;
                
                if(freeMB->nextUMB != NULL) /* Is last item?                  */
                {                           /* No,set the link for list       */
                    tmpUMB = (P_UMB)((U32)(freeMB->nextUMB)-1); 
                    tmpUMB->preMB = newFMB;
                }
                
                usedMB->nextMB = newFMB;    /* Set used memory block link     */
                usedMB->preMB  = freeMB->preUMB;
            }
          
            if(freeMB->preUMB != NULL)      /* Is first item?                 */
            {                               /* No,set the link for list       */
                tmpUMB = (P_UMB)((U32)(freeMB->preUMB)-1); 
                tmpUMB->nextMB = (void*)((U32)usedMB|0x1);
            }
          
            OsSchedUnlock();              /* Unlock schedule                  */
            return memAddr;               /* Return used memory block address */
        }
        preFMB = freeMB;        /* Save current free memory block as previous */
        freeMB = freeMB->nextFMB;         /* Get the next item as current item*/
    }
    OsSchedUnlock();                      /* Unlock schedule                  */
    return NULL;                          /* Error return                     */
}


/**
 *******************************************************************************
 * @brief      Release memory block to kernel heap.  
 * @param[in]  memBuf    Pointer to memory block.
 * @param[out] None
 * @retval     None  		 
 *
 * @par Description
 * @details    This function is called to release memory block.
 *******************************************************************************
 */
void CoKfree(void* memBuf)
{
    P_FMB    curFMB,nextFMB,preFMB;
    P_UMB    usedMB,nextUMB,preUMB;

#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if(memBuf == NULL)
    {
        return;
    }
#endif
    
    usedMB = (P_UMB)((U32)(memBuf)-8);
    
#if CFG_PAR_CHECKOUT_EN >0              /* Check validity of parameter        */
    if((U32)(memBuf) < Kheap.startAddr)
    {
        return;
    }
    if((U32)(memBuf) > Kheap.endAddr)
    {
        return;
    }
#endif
    
    
    OsSchedLock();                      /* Lock schedule                      */

#if CFG_PAR_CHECKOUT_EN >0              /* Check UMB in list                  */ 
    if((U32)(usedMB) < (U32)(FMBlist))
    {
        preUMB = (P_UMB)((U32)(FMBlist->preUMB)-1);
        while(preUMB != usedMB)	
        {
            if(preUMB == NULL)
            {
                OsSchedUnlock();
                return;
            }
            preUMB = (P_UMB)((U32)(preUMB->preMB)-1);	
        }
    }
    else
    {
        if(FMBlist == NULL)
        {
            nextUMB = (P_UMB)(Kheap.startAddr);	
        }
        else
        {
            if(FMBlist->nextUMB != NULL)
            {
                nextUMB = (P_UMB)((U32)(FMBlist->nextUMB)-1);	
            }
            else
            {
                nextUMB = NULL;	
            }
        }
    	
        while(nextUMB != usedMB)	
        {
            if(nextUMB == NULL)
            {
                OsSchedUnlock();
                return;
            }	
            if(((U32)(nextUMB->nextMB)&0x1) == 0)		
            {
                nextFMB = (P_FMB)(nextUMB->nextMB);
                nextUMB = (P_UMB)((U32)(nextFMB->nextUMB)-1);		
            }
            else
            {
                nextUMB = (P_UMB)((U32)(nextUMB->nextMB)-1);	
            }
        }		
    }
#endif
    
    
    /* Is between two free memory block? */	 
    if( (((U32)(usedMB->nextMB)&0x1) == 0) && (((U32)(usedMB->preMB)&0x1)==0) )	
    {                             /* Yes,is the only one item in kernel heap? */
        if((usedMB->nextMB == NULL) && (usedMB->preMB == NULL))
        {
            curFMB = (P_FMB)usedMB;       /* Yes,release this item            */
            curFMB->nextFMB = NULL;	
            curFMB->nextUMB = NULL;
            curFMB->preUMB  = NULL;	
            FMBlist = curFMB;	
        }
        else if(usedMB->preMB == NULL)    /* Is the first item in kernel heap */
        {		
            /* Yes,release this item,and set link for list                    */						
            curFMB  = (P_FMB)usedMB; 
            nextFMB = (P_FMB)usedMB->nextMB;
            
            curFMB->nextFMB = nextFMB->nextFMB;	
            curFMB->nextUMB = nextFMB->nextUMB;
            curFMB->preUMB  = NULL;
            FMBlist         = curFMB;
        }
        else if(usedMB->nextMB == NULL)   /* Is the last item in kernel heap  */
        {                      /* Yes,release this item,and set link for list */
            curFMB = (P_FMB)(usedMB->preMB);	
            curFMB->nextFMB = NULL;
            curFMB->nextUMB = NULL;
        }							    
        else                  /* All no,show this item between two normal FMB */
        {		
            /* release this item,and set link for list                        */						  
            nextFMB = (P_FMB)usedMB->nextMB;
            curFMB  = (P_FMB)(usedMB->preMB);	
            
            curFMB->nextFMB = nextFMB->nextFMB;
            curFMB->nextUMB = nextFMB->nextUMB;
        }
    }
    else if(((U32)(usedMB->preMB)&0x1) == 0)  /* Is between FMB and UMB?      */
    {								   
        if(usedMB->preMB == NULL)   /* Yes,is the first item in kernel heap?  */
        {
            /* Yes,release this item,and set link for list                    */
            curFMB          = (P_FMB)usedMB;      
            nextUMB         = (P_UMB)usedMB->nextMB;		
            curFMB->nextUMB = nextUMB;
            curFMB->preUMB  = NULL;
            curFMB->nextFMB = FMBlist;
            FMBlist         = curFMB;
        }
        else                    /* No,release this item,and set link for list */
        {							      
            curFMB          = (P_FMB)usedMB->preMB;
            nextUMB         = (P_UMB)usedMB->nextMB;
            curFMB->nextUMB = nextUMB;
        }
    
    }
    else if(((U32)(usedMB->nextMB)&0x1) == 0)   /* Is between UMB and FMB?    */
    {                                           /* Yes                        */
        preUMB = (P_UMB)(usedMB->preMB);        /* Get previous UMB           */
        curFMB = (P_FMB)(usedMB);               /* new FMB                    */
        preFMB = GetPreFMB(usedMB);             /* Get previous FMB           */
        if(preFMB == NULL)                      /* Is previous FMB==NULL?     */
        {	
            nextFMB = FMBlist;                  /* Yes,get next FMB           */ 
            FMBlist = curFMB;   /* Reset new FMB as the first item of FMB list*/
        }
        else
        {
            nextFMB = preFMB->nextFMB;          /* No,get next FMB            */
            preFMB->nextFMB  = curFMB;          /* Set link for FMB list      */
        }
        
        if(nextFMB == NULL)           /* Is new FMB as last item of FMB list? */
        {	
            curFMB->preUMB  = preUMB;           /* Yes,set link for list      */
            curFMB->nextUMB = NULL;
            curFMB->nextFMB = NULL;			
        }	
        else
        {
            curFMB->preUMB  = preUMB;           /* No,set link for list       */
            curFMB->nextUMB = nextFMB->nextUMB;
            curFMB->nextFMB = nextFMB->nextFMB;	
        }
    }
    else                                    /* All no,show UMB between two UMB*/
    {									  
        curFMB  = (P_FMB)(usedMB);          /* new FMB                        */
        preFMB  = GetPreFMB(usedMB);        /* Get previous FMB               */
        preUMB  = (P_UMB)(usedMB->preMB);   /* Get previous UMB               */
        nextUMB = (P_UMB)(usedMB->nextMB);  /* Get next UMB                   */
        
        if(preFMB == NULL )                 /* Is previous FMB==NULL?         */
        {
            nextFMB = FMBlist;              /* Yes,get next FMB               */
            FMBlist = curFMB;  /* Reset new FMB as the first item of FMB list */
      	}
      	else
      	{
            nextFMB = preFMB->nextFMB;      /* No,get next FMB                */
            preFMB->nextFMB = curFMB;       /* Set link for FMB list          */
      	}
      	
        curFMB->preUMB  = preUMB;           /* Set current FMB link for list  */
        curFMB->nextUMB = nextUMB;
        curFMB->nextFMB = nextFMB;
    }
    
    if(curFMB->preUMB != NULL)/* Is current FMB as first item in kernel heap? */
    {                         /* No,set link for list                         */
      	preUMB = (P_UMB)((U32)(curFMB->preUMB)-1); 
      	preUMB->nextMB = (void*)curFMB;
    }
    if(curFMB->nextUMB != NULL)/* Is current FMB as last item in kernel heap? */
    {                          /* No,set link for list                        */
      	nextUMB = (P_UMB)((U32)(curFMB->nextUMB)-1); 
      	nextUMB->preMB = (void*)curFMB;		
    }
    OsSchedUnlock();           /* Unlock schedule                             */
}


/**
 *******************************************************************************
 * @brief      Get previous free memory block pointer.  
 * @param[in]  usedMB    Current used memory block.
 * @param[out] None
 * @retval     Previous free memory block pointer.		 
 *
 * @par Description
 * @details    This function is called to get previous free memory block pointer.
 *******************************************************************************
 */
static P_FMB GetPreFMB(P_UMB usedMB)
{
    P_UMB preUMB;
    preUMB = usedMB;
    while(((U32)(preUMB->preMB)&0x1))   /* Is previous MB as FMB?             */
    {                                   /* No,get previous MB                 */
        preUMB = (P_UMB)((U32)(preUMB->preMB)-1);
    }	
    return (P_FMB)(preUMB->preMB);      /* Yes,return previous MB             */
}

#endif
