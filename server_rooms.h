#ifndef __SERVER_ROOMS_H__
#define __SERVER_ROOMS_H__

typedef struct 
{
	char roomName[MESSAGE_SIZE];
	sockaddr_in roomId;
	size_t onlineUsers;
} RoomStruct;

/**

*/
RoomStruct* GetRoomId(HashMap *_roomsHash, char _message[SERVER_MESSAGE_SIZE]);
/**

*/
int CreateRoom(HashMap *_roomsHash, PortStruct *_roomList, char _message[SERVER_MESSAGE_SIZE]);

#endif /*__SERVER_ROOMS_H__*/
