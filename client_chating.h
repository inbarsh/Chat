#ifndef __CLIENT_CHATING_H__
#define __CLIENT_CHATING_H__
#include "common.h"
/**

*/
int GetRoomName(RoomNameMessage *_roomName);
/**

*/
void RoomsUI(Register_Login_Message *_regLogMessage, AvailableRoomsMessage *_availableRooms, int _socket);

#endif /*__CLIENT_CHATING_H__*/
