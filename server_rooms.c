#include "DLlist.h"
#include "common.h"
#include "logger_pub.h"
#include "HashMap.h"
#include "MultiCast_Addresses.h"
#include "ChatCreator.h"
#include "server_rooms.h"

RoomStruct* GetRoomId(HashMap *_roomsHash, char _message[SERVER_MESSAGE_SIZE])
{
	void *container;
	RoomStruct *room = NULL;
	char *roomName = malloc(sizeof(char)*MESSAGE_SIZE);
	strcpy(roomName, _message);
	if((container = HashFind(_roomsHash, roomName)) == NULL)
	{
		return NULL;	
	}
	room = ListGetData(container);
	free(roomName);
	return room;
}

int CreateRoom(HashMap *_roomsHash, PortStruct *_roomList, char _message[SERVER_MESSAGE_SIZE])
{
	sockaddr_in roomId;
	RoomStruct *room = NULL;
	int port;
	roomId.sin_family = AF_INET;
    roomId.sin_addr.s_addr = inet_addr(MULTICAST_ADDRESS);
	room = malloc(sizeof(RoomStruct));
    port = GetRoomPort(_roomList);
    if (port == 0)
    {
    	return false;
    }
    roomId.sin_port = htons(port);
	strcpy(room->roomName, ((RoomNameMessage*)_message)->RoomName);
	room->onlineUsers = 1;
	room->roomId = roomId;
	if(HashInsert(_roomsHash, room->roomName, room) == ERR_OK)
	{
		return true;
	}
	return false;
}
