/**
 *******************************************************************************
 * @file       cpu.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Implement function declare related to Cortex-M3(ARM-v7)
 * @details    This header file including functions or defines related to 
 *             Cortex-M3(ARM-v7).	 		
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef  _CPU_H
#define  _CPU_H


#define NVIC_ST_CTRL    (*((volatile U32 *)0xE000E010))
#define NVIC_ST_RELOAD  (*((volatile U32 *)0xE000E014))
#define RELOAD_VAL      ((U32)(( (U32)CFG_CPU_FREQ) / (U32)CFG_SYSTICK_FREQ) -1)

/*!< Initial System tick.	*/
#define InitSysTick()
//			NVIC_ST_RELOAD =  RELOAD_VAL; \
//			NVIC_ST_CTRL   =  0x0007

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
extern U8      Inc8 (U8 *data);
extern U8      Dec8 (U8 *data);

#endif
