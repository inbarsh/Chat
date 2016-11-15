#ifndef __PROJECT_COMMON_H__
#define __PROJECT_COMMON_H__

/** @file project_common.h
 *  @brief 
 *  @author Adva Cohen, Inbar Shaltin and Jonathan Sokolowski  
 *  @bug No known bugs
 */
/*=================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>  
#include <sys/types.h>
#include <sys/socket.h>

#define SERVER_IP "10.0.2.15"
#define SERVER_PORT 8888
#define MESSAGE_SIZE 128
#define USERNAME_LEN 8
#define PASSWORD_LEN 8
#define SERVER_MESSAGE_SIZE 1024

typedef enum 
{
	/*client messages*/
	CLIENT_REGISTER = 0,
	CLIENT_LOGIN =1, 
	CLIENT_DEREGISTER = 2,
	CLIENT_EXIT = 3,
	CLIENT_CREATE_ROOM = 4, /*CREATE_ROOM*/
	CLIENT_JOIN_ROOM = 5,
	CLIENT_LEAVE_ROOM = 6, 
	CLIENT_LOGOUT = 7, /*CLIENT_LOG_OUT*/
	CLIENT_CHAT_MESSAGE = 8, /*CHAT_MESSAGE*/
	/*server messages*/	
	SERVER_REPLY = 9, /* LOGIN_REPLY, REGISTER_REPLY, DEREGISTER_REPLY, CREATE_ROOM_REPLY, CLIENT_LEAVE_ROOM_REPLY*/
	SERVER_AVAILABLE_ROOMS = 10, /*AVAILABLE_ROOMS_MESSAGE*/
	SERVER_ROOM_ID = 11 /*CLIENT_JOIN_ROOM_REPLY*/
} MessageType;

typedef enum 
{
	OK = 12,/*everything but login*/
	ALREADY_EXISTS = 13, /*REG, LOGIN*/
	NO_MATCH = 14, /*LOGIN, DEREG - username not exist or wrong password, CLIENT_JOIN_ROOM - twice or not exist room*/
	CRITICAL_PROBLEM = 15
} ServerReplyErr;

typedef struct 
{
	MessageType type;
	char message[SERVER_MESSAGE_SIZE]; 
}AvailableRoomsMessage;

typedef struct 
{
	MessageType type;
	ServerReplyErr reply; /**> for 1: every server reply 2: port number*/
}ServerReply;

typedef struct 
{
	MessageType type;
	char username[USERNAME_LEN]; /**> for 1: username */
	char password[PASSWORD_LEN];
}Register_Login_Message;

typedef struct
{
	MessageType type;
	struct sockaddr_in roomId;	
}RoomIdMessage;

typedef struct 
{
	MessageType type;
	char RoomName[MESSAGE_SIZE];
}RoomNameMessage;

typedef struct 
{
	MessageType type;
	char username[USERNAME_LEN];
	char message[MESSAGE_SIZE];
}MultiCastMessage;

#endif /* __PROJECT_COMMON_H__ */

