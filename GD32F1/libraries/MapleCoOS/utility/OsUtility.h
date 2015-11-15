/**
 *******************************************************************************
 * @file       utility.h
 * @version    V1.00    Initial version
 * @date       2009.06.26
 * @brief      Utility function header file 
 * @details    This file including some defines and declares related to utility 
 *             function.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef  _UTILITY_H
#define  _UTILITY_H


/**
 * @struct  Time struct  utility.h  	
 * @brief   Time struct	 
 * @details This struct use to manage time 	 
 */
typedef struct SysTime
{
    U8   sec;                           /*!< Second                           */
    U8   min;                           /*!< Minute                           */
    U8   hour;                          /*!< Hour                             */
    U8   date;                          /*!< Date                             */
    U8   month;                         /*!< Month                            */
    U16  year;                          /*!< Year                             */
}TIME;

#endif
