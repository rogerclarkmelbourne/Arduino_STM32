/**
 *******************************************************************************
 * @file       OsFlag.h
 * @version    V1.1.6    
 * @date       2014.05.23
 * @brief      Evnet flag management header file
 * @details    This file including some defines and declares about flag management.	
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

