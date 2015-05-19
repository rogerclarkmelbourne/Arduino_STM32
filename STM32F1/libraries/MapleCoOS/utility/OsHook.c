/**
 *******************************************************************************
 * @file       hook.c
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      hook management implementation code of CooCox CoOS kernel.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


/*---------------------------- Inlcude --------------------------------------*/
#include <coocox.h>

/**
 *******************************************************************************
 * @brief      IDLE task of OS	 
 * @param[in]  pdata	The parameter passed to IDLE task.		 
 * @param[out] None 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is system IDLE task code.	 
 *******************************************************************************
 */
void CoIdleTask(void* pdata)
{
    /* Add your codes here */
    for(; ;) 
    {
        /* Add your codes here */
    }
}


/**
 *******************************************************************************
 * @brief      Hook for stack overflow	 
 * @param[in]  taskID	Piont to the task which lead to stack overflow.		 
 * @param[out] None 
 * @retval     None	 
 *
 * @par Description
 * @details    This function is a hook for stack overflow.	 
 *******************************************************************************
 */
void CoStkOverflowHook(OS_TID taskID)
{
    /* Process stack overflow  here */
    for(; ;) 
    {
      
    }
}






