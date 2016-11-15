#include "ADTErr.h"
#include <stdio.h>

char* GetMessageADTErr (int _error)
{
	switch(_error)
	{
		case ERR_OK: /* 0 */
			return "OK";
			break;
		case ERR_UNDERFLOW:
			return "Underflow";
			break;
		case ERR_OVERFLOW:
			return "Overflow";
			break;
		case ERR_NOT_INITIALIZED:
			return "ADT not initialized";
			break;
		case ERR_ALLOCATION_FAILED:
			return "Memory allocation failed";
			break;
		case ERR_NULL_POINTER:
			return "Received NULL pointer";
			break;
		case ERR_WRONG_INDEX:
			return "Wrong index";
			break;
		case ERR_DUPLICATE_KEY:
			return "Duplicate key";
			break;
		case ERR_ALREADY_EXISTS:
			return "Data already exists";
			break;
		case ERR_DIVISION_BY_ZERO:
			return "Division by zero";
			break;
		case ERR_NOT_FOUND:
			return "Data not found";
			break;
		case ERR_CAN_NOT_OPEN_FILE:
			return "Can not open file";
			break;
		case ERR_UNKNOWN_PARAMETER:
			return "Unknown option character";
			break;
		case ERR_LOG_CREATE_FAILED:
			return "Log create failed";
			break;
		case ERR_FAILED_WRITE_TO_FILE:
			return "Failure writing to file";
			break;
		case ERR_THREAD_CREATE:
			return "Thread creation failed";
			break;
		case ERR_INVALID_CDR:
			return "Invalid CDR received";
			break;
		case ERR_DATABASE_UPDATE_FAILED:
			return "Database update failed";
			break;
		case ERR_CAN_NOT_CREATE_FOLDER:
			return "Failed creating folder";
			break;
		case ERR_CORRUPTED_FILE:
			return "Cuurapted backup file";
			break;
		case ERR_PERMISSION_DENIED:
			return "Permission denied";
			break;
		case ERR_SEND:
			return "Sending mesaage to group error";
			break;
		case ERR_LIST_ENCODING:
			return "List encoding error";
			break;
		case ERR_ROOM_NOT_FOUND:
			return "Room not found in database";
			break;
		case ERR_USER_NOT_FOUND:
			return "User not found in database";
			break;
		default:
			return "Unknown error type";
			break;
	}
}
