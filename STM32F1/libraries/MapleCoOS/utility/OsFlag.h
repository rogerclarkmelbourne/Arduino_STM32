/**
 *******************************************************************************
 * @file       flag.h
 * @version    V1.12    
 * @date       2010.03.01
 * @brief      Evnet flag management header file
 * @details    This file including some defines and declares about flag management.	
 *******************************************************************************
 * @copy
 *
 * INTERNAL FILE,DON'T PUBLIC.
 * 
 * <h2><center>&copy; COPYRIGHT 2009 CooCox </center></h2>
 *******************************************************************************
 */ 


#ifndef _FLAG_H
#define _FLAG_H

/**
 * @struct  FlagNode  flag.h 	
 * @brief   Flag node struct
 * @details 	 	
 */
typedef struct FlagNode
{
    struct FlagNode*  nextNode;         /*!< A pointer to next flag node      */
    struct FlagNode*  prevNode;         /*!< A pointer to prev flag node      */
    U32               waitFlags;        /*!< Flag value                       */
    P_OSTCB           waitTask;         /*!< A pointer to task waitting flag  */
    U8                waitType;         /*!< Wait type                        */
}FLAG_NODE,*P_FLAG_NODE;


/**
 * @struct  Flag    flag.h  	
 * @brief   Flag control block
 * @details This struct use to mange event flag.	
 */
typedef struct Flag
{
    U32           flagRdy;              /*!< Ready flag                       */
    U32           resetOpt;             /*!< Reset option                     */
    U32           flagActive;           /*!< Active flag                      */
    P_FLAG_NODE   headNode;             /*!< Head node                        */
    P_FLAG_NODE   tailNode;             /*!< Tail node                        */
}FCB,*P_FCB;


/*---------------------------- Variable declare ------------------------------*/
extern FCB FlagCrl;					

/*---------------------------- Function declare ------------------------------*/
extern void        RemoveLinkNode(P_FLAG_NODE pnode);
#endif

