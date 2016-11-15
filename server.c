#define _XOPEN_SOURCE 500
#define _BSD_SOURCE

#include "DLlist.h"
#include "common.h"
#include "logger_pub.h"
#include "HashMap.h"
#include "MultiCast_Addresses.h"
#include "server_user.h"
#include "server_rooms.h"
#define NUM_OF_BUILT_IN_ROOMS 5
#define MAX_NUM_OF_CLIENTS 1000
/*************************************************************************************************/
/*STRUCTS*/
typedef struct timeval timeval;

typedef struct
{
	int addrlen;
	int mainSocket;
	HashMap *users;
	HashMap *rooms;
	List *onlineUsers;
	PortStruct *roomsPorts;
}Server;
/*************************************************************************************************/
/*STATIC*/
/*static char* roomsNames[NUM_OF_BUILT_IN_ROOMS] = {"sunday", "almog", "experis", "room", "inbar"};*/
/*************************************************************************************************/
/*FUNCTIONS HEADERS*/
static Server* Server_Init();
static void SetSocketOpt(int _socket);
static void Server_Run(Server* _chatServer);
static void ServerAppRun(Server* _chatServer, int _socket, Register_Login_Message* _message);
static int HashFuncString(HashKey _key);
static int StringEqualFunc(HashKey _key, HashKey _otherKey);
/*************************************************************************************************/
/*PROGRAM*/
int main()
{
	Server *chatServer = NULL;
	chatServer = Server_Init();
	if (chatServer != NULL)
	{
		Server_Run(chatServer);
	}
    return 0;
}
/*************************************************************************************************/
/*FUNCTIONS*/
static Server* Server_Init()
{
    sockaddr_in address;
    size_t i;
    Server *chatServer = NULL;
    RoomNameMessage roomsMessage[NUM_OF_BUILT_IN_ROOMS];
    char* roomsNames[NUM_OF_BUILT_IN_ROOMS] = {"sunday", "almog", "experis", "room", "inbar"};
    LogCreate(LOG_DEB, 0, "ActivityLog");
	LogCreate(LOG_DEB, 1, "chatHistory");
    chatServer = malloc(sizeof(Server));
    if (chatServer == NULL)
    {
    	LOG_DEBUG_PRINT("%s", 0, "fail to allocate server structure")
    }
    if ((chatServer->rooms = HashCreate(MAX_NUM_OF_CLIENTS, HashFuncString, StringEqualFunc)) == NULL)
    {
    	LOG_DEBUG_PRINT("%s", 0, "fail to create chat rooms hash")
    }
    if ((chatServer->users = HashCreate(MAX_NUM_OF_CLIENTS, HashFuncString, StringEqualFunc)) == NULL)
    {
    	LOG_DEBUG_PRINT("%s", 0, "fail to create chat users hash")
    }
   	if ((chatServer->onlineUsers = ListCreate()) == NULL)
   	{
   		LOG_DEBUG_PRINT("%s", 0, "fail to create online users list")
   	}
    if ((chatServer->roomsPorts = CreateRoomList()) == NULL)
    {
    	LOG_DEBUG_PRINT("%s", 0, "fail to create rooms ports list")
    }
    if((chatServer->mainSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        LOG_DEBUG_PRINT("socket error: %s", 0, strerror(errno))
        exit(EXIT_FAILURE);
    }
  	SetSocketOpt(chatServer->mainSocket);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(SERVER_IP);
    address.sin_port = htons(SERVER_PORT);
    
    if (bind(chatServer->mainSocket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        LOG_DEBUG_PRINT("bind error: %s", 0, strerror(errno))
        exit(EXIT_FAILURE);
    }
     
    if (listen(chatServer->mainSocket, MAX_NUM_OF_CLIENTS) < 0)
    {
        LOG_DEBUG_PRINT("listen error: %s", 0, strerror(errno))
        exit(EXIT_FAILURE);
    }
    for (i = 0; i < NUM_OF_BUILT_IN_ROOMS; i++)
    {
		strcpy(roomsMessage[i].RoomName, roomsNames[i]);
		CreateRoom(chatServer->rooms, chatServer->roomsPorts, (char*)&(roomsMessage[i]));
    }
	return chatServer;
}

static void SetSocketOpt(int _socket)
{
    char *opt;
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        LOG_DEBUG_PRINT("setsockopt error: %s", 0, strerror(errno))
        exit(EXIT_FAILURE);
    }
}

static void Server_Run(Server* _chatServer)
{
    int currentActivity;
    sockaddr_in sockIn;
    ClientInfo *newClientInfo = NULL;
    void *listItr = NULL;
    void *ListTempItr = NULL;
    char message[SERVER_MESSAGE_SIZE];
    int numOfClients = 0;
    fd_set readfds, tempfds;
    int valread, i, sd, activity, new_socket;
    int max_sd = _chatServer->mainSocket;
    FD_ZERO(&readfds);
    FD_ZERO(&tempfds);
    FD_SET(_chatServer->mainSocket, &readfds);
    LOG_DEBUG_PRINT("%s", 0, "server is running")
    while(true) 
    {
		tempfds = readfds;
        activity = select(max_sd + 1, &tempfds, NULL, NULL, NULL);
    	
        if ((activity < 0) && (errno!=EINTR)) 
        {
            LOG_DEBUG_PRINT("select error: %s", 0, strerror(errno))
            exit(EXIT_FAILURE);
            
        }
        if (FD_ISSET(_chatServer->mainSocket, &tempfds)) 
        {
            if ((new_socket = accept(_chatServer->mainSocket, (struct sockaddr *)&sockIn, (socklen_t*)&_chatServer->addrlen)) < 0)
            {
                LOG_DEBUG_PRINT("accept error: %s", 0, strerror(errno))
                exit(EXIT_FAILURE);
            }
            if (max_sd < new_socket)
            {
            	max_sd = new_socket;
            }
            FD_SET(new_socket, &readfds);
            ++numOfClients;
            --activity;
            if (NULL == (newClientInfo = CreateClientInfo(new_socket, &sockIn)))
            {
            	LOG_DEBUG_PRINT("%s", 0, "fail to create clientInfo structure")
            	exit(EXIT_FAILURE);
            }
            if (numOfClients < MAX_NUM_OF_CLIENTS)
            {
            	ListPushHead(_chatServer->onlineUsers, newClientInfo);
            }
            else
            {
				close(new_socket);
				free(newClientInfo);
            }
            if (activity == 0)
            {
            	continue;
            }
        }
        
        currentActivity = 0;
        if(NULL == (listItr = ListBegin(_chatServer->onlineUsers)))
        {
			LOG_DEBUG_PRINT("%s", 0, "listBegin")
			exit(EXIT_FAILURE);        
        }
		for(i = 0; i < numOfClients && currentActivity < activity; ++i)
        {

            sd = GetCurrentSocket(listItr);
			listItr = ListNext(listItr);
			ListTempItr = ListPrev(listItr);		
            if(sd != 0)
            {
		        if (FD_ISSET(sd, &tempfds)) 
		        {
		            ++currentActivity;
		            if ((valread = recv(sd, &message, SERVER_MESSAGE_SIZE, 0)) < 1)
		            {
		                close(sd);
		                FD_CLR(sd, &readfds);
		                --numOfClients;
		                
		                ListRemove(ListTempItr);

		             	if (valread == 0)
				        {
				            LOG_DEBUG_PRINT("Host disconnected, ip %s, port %d ", 0, inet_ntoa(sockIn.sin_addr) , ntohs(sockIn.sin_port))
				        }
		            }
		            else
					{
		                LOG_DEBUG_PRINT("incoming transmission from fd %d", 0, sd)
		                ServerAppRun(_chatServer, sd, (Register_Login_Message*)message);
					}
		        }
            }
        }
    }
}

void ServerAppRun(Server* _chatServer, int _socket, Register_Login_Message* _message)
{
	ServerReply binaryReply;
	RoomStruct *room;
	int type;
	RoomIdMessage roomIdMessage;
	AvailableRoomsMessage roomsMessage;
	roomIdMessage.type = htonl(SERVER_ROOM_ID);
	type = _message->type;
	switch (type)
	{
		case CLIENT_REGISTER:
			if (ClientRegister(_chatServer->users, _message))
			{
				binaryReply.type = SERVER_REPLY;
				binaryReply.reply = OK;
				LOG_DEBUG_PRINT("%s registered to chat", 0, _message->username)
				if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}
			}
			else
			{
				binaryReply.type = SERVER_REPLY;
				binaryReply.reply = ALREADY_EXISTS;
				if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}			
			}
		break;
		case CLIENT_DEREGISTER:
			if (ClientDeRegister(_chatServer->users, _message))
			{
				binaryReply.type = SERVER_REPLY;
				binaryReply.reply = OK;
				LOG_DEBUG_PRINT("%s deregistered from chat", 0,_message->username)
				if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}
			}
			else
			{
				binaryReply.type = SERVER_REPLY;
				binaryReply.reply = NO_MATCH;
				if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}			
			}
		break;
		case CLIENT_LOGIN:
			if (ClientLogin(_chatServer->users, _message))
			{
				roomsMessage.type = SERVER_AVAILABLE_ROOMS;
				LOG_DEBUG_PRINT("%s loged in to chat", 0, _message->username)
				/*strcpy(roomsMessage.message, roomsNames[i]);
				for (i = 1; i < NUM_OF_BUILT_IN_ROOMS; ++i)
				{
					strcat(roomsMessage.message, " ");
					strcat(roomsMessage.message, roomsNames[i]);
				}*/
				if (send(_socket, (char*)&roomsMessage, sizeof(AvailableRoomsMessage), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}
			}
			else
			{
				binaryReply.type = SERVER_REPLY;
				binaryReply.reply = NO_MATCH;
				if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
				{
					LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
				}
			}	
		break;
		case CLIENT_JOIN_ROOM:
				room = GetRoomId(_chatServer->rooms, ((RoomNameMessage*)_message)->RoomName);
				if (room != NULL)
				{
					roomIdMessage.roomId = room->roomId;
					++room->onlineUsers;
					LOG_DEBUG_PRINT("%s joined room %s", 0, _message->username, room->roomName)
					if (send(_socket, (char*)&roomIdMessage, sizeof(RoomIdMessage), 0) < 0) 
					{
						LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno));
					}			
				}
				else
				{
					binaryReply.type = SERVER_REPLY;
					binaryReply.reply = NO_MATCH;
					if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
					{
						LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
					}								
				}			
		break;
		case CLIENT_LEAVE_ROOM:
				room = GetRoomId(_chatServer->rooms, ((RoomNameMessage*)_message)->RoomName);
				if (room != NULL)
				{
					roomIdMessage.roomId = room->roomId;
					--room->onlineUsers;
					LOG_DEBUG_PRINT("%s left room %s", 0, _message->username, room->roomName)
					binaryReply.type = SERVER_REPLY;
					binaryReply.reply = OK;
					if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
					{
						LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
					}			
				}
				else
				{
					binaryReply.type = SERVER_REPLY;
					binaryReply.reply = NO_MATCH;
					if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
					{
						LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
					}								
				}			
		break;
		/*case CLIENT_EXIT:
			printf("bye bye! come back soon!\n");
			return;
		break;*/
		case CLIENT_LOGOUT:
			ClientLogout(_chatServer->users, _message);
			binaryReply.type = SERVER_REPLY;
			binaryReply.reply = OK;
			LOG_DEBUG_PRINT("%s loged out", 0, _message->username)
			if (send(_socket, (char*)&binaryReply, sizeof(ServerReply), 0) < 0) 
			{
				LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
			}			
		break;
		default:
			break;
	}
	memset(_message, 0, SERVER_MESSAGE_SIZE);
}
static int HashFuncString(HashKey _key)
{
	int intKey = 0, i;
	char* key = (char*)_key;
	for (i = 0; i < USERNAME_LEN; ++i)
	{
		intKey += *(char*)key;
		++key;
	}
	intKey = (intKey % MAX_NUM_OF_CLIENTS); 
	return intKey;
}

static int StringEqualFunc(HashKey _key, HashKey _otherKey)
{
	char* leftKey = (char*)_key;
	char* rightKey = (char*)_otherKey;
	if (strcmp(leftKey, rightKey) != 0)
	{
		return 1;
	}
	return 0;
}
