#include "common.h"
#include "ChatCreator.h"
#include "MultiCast_Addresses.h"

void OpenNewRoom(sockaddr_in _address, char *_Username, char *_roomName)
{
	char bashCommandRecieve[SERVER_MESSAGE_SIZE];
	char bashCommandSend[SERVER_MESSAGE_SIZE];
	
	sprintf(bashCommandRecieve ,"%s %s %s %d %s %s","gnome-terminal --geometry=50x40+10+10 -x sh -c \"./chatwindowReceive", _Username, inet_ntoa(_address.sin_addr), ntohs(_address.sin_port) , _roomName, ";bash\"");
	
	sprintf(bashCommandSend ,"%s %s %d %s %s %s","gnome-terminal --geometry=50x5+10+810 -x sh -c \"./chatwindowsend", inet_ntoa(_address.sin_addr), ntohs(_address.sin_port) , _Username, _roomName, ";bash\"");
	system(bashCommandRecieve);
	system(bashCommandSend);
}




