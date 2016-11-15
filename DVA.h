/******************************************************************************************
Author:Inbar Shalitin
Creation date: 18.2.16
Last modified date: 18.2.16
description: Errors
*******************************************************************************************/

#ifndef __DVA_H__
#define __DVA_H__

typedef enum

{

    /* General ADT Errors */

    ERR_OK                            =1,

    ERR_GENERAL,

    ERR_NOT_INITIALIZED,

    ERR_ALLOCATION_FAILED,

    ERR_UNDERFLOW,
    
    ERR_WRONG_INDEX,
    
    ERR_SOCKET_CLOSED,
    
    ERR_NULL_POINTER,
    
    ERR_NO_MSG

} ADTErr;

#endif /*__DVA_H__*/
