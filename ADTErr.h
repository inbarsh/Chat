/** @file   ADTErr.h
 *  @brief  ADTErr library Functions
 *  @author Inbar Shalitin
 *  @date 	08/02/2016
 *  @bug No known bugs.
 */

#ifndef __ADTDEFS_H__
#define __ADTDEFS_H__

typedef enum
{
    /* General ADT Errors begin*/
    ERR_OK              = 0,
    ERR_GENERAL,
    ERR_NOT_INITIALIZED,
    ERR_ALLOCATION_FAILED,
    ERR_REALLOCATION_FAILED,
    ERR_UNDERFLOW,
    ERR_OVERFLOW,
    ERR_WRONG_INDEX,
    ERR_NULL_POINTER,
    /* Vector Errors */

	/*Tree Errors*/
	ERR_DATA_EXIST,
    /* Bit Vector*/

	/*Hash Set Errors*/
	ERR_DUPLICATE_KEY,
	ERR_ALREADY_EXISTS,
	ERR_DIVISION_BY_ZERO,
	ERR_NOT_FOUND,

	/*File Errors*/
	ERR_CAN_NOT_OPEN_FILE,
	ERR_UNKNOWN_PARAMETER,
	ERR_LOG_CREATE_FAILED,
	ERR_FAILED_WRITE_TO_FILE,
    ERR_CAN_NOT_CREATE_FOLDER,
    ERR_CORRUPTED_FILE,

	/*Threads*/
	ERR_THREAD_CREATE,

	/*CDR Project*/
	ERR_INVALID_CDR,
	ERR_DATABASE_UPDATE_FAILED,

    /*Chat Project*/
    ERR_PERMISSION_DENIED,
    ERR_SEND,
    ERR_RECEIVE,
    ERR_LIST_ENCODING,
    ERR_NO_ADMIN,
    ERR_ROOM_NOT_FOUND,
    ERR_USER_NOT_FOUND,
    ERR_CORRUPTED_MESSAGE

} ADTErr;

 /** @brief Get message that describes the error code as a string
 *
 * @params _error : ADTErr recived
 * @retval _message : Translated message
 */
char* GetMessageADTErr (int _error);

#endif /*__ADTDEFS_H__*/
