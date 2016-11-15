#include "client_chating.h"

int GetRoomName(RoomNameMessage *_roomName)
{
	printf("Please enter your desired room name\n");
	scanf("%s", _roomName->RoomName);
	if (strlen(_roomName->RoomName) >= 4 && strlen(_roomName->RoomName) <= 8)
	{
		return true;
	}
	return false;
}

void RoomsUI(Register_Login_Message *_regLogMessage, AvailableRoomsMessage *_availableRooms, int _socket)
{
	int userChoice, ans;
	int onFlag = 1;
	RoomIdMessage roomId;
	RoomNameMessage roomName;
	ServerReply binaryReply;
	int recv_len;
	printf("Current exist rooms:\n%s\n", _availableRooms->message);
	while(onFlag)
	{
		printf("Press 1 to join room\n Press 2 to leave a room\n Press 3 to logout\n >\n");
		scanf("%d", &userChoice);
		switch (userChoice)
		{
		case 1:
			ans = GetRoomName(&roomName);
			if (ans)
			{
				roomName.type = htonl(CLIENT_JOIN_ROOM);
				if (send(_socket, (char*)&roomName, sizeof(RoomNameMessage), 0) < 0) 
					{
						perror("send error");
						abort();
					}
				if ((recv_len = recv(_socket, &roomId, sizeof(RoomIdMessage), 0)) < 0)
				{
					perror("receive error");
					abort();
				}
				roomId.type = ntohl(roomId.type);
				/*if (roomId.type == SERVER_ROOM_ID)
				{
					OpenNewRoom(roomId.roomId, _regLogMessage->username, roomName.RoomName);
				}*/
			}
			else
			{
				printf("Sorry, Room is not exist!\n");
				sleep(1);
			}
		break;
		case 2:
			ans = GetRoomName(&roomName);
			if (ans)
			{
				roomName.type = htonl(CLIENT_LEAVE_ROOM);
				if (send(_socket, (char*)&roomName, sizeof(RoomNameMessage), 0) < 0) 
					{
						perror("send error");
						abort();
					}
				if ((recv_len = recv(_socket, &binaryReply, sizeof(ServerReply), 0)) < 0)
				{
					perror("receive error");
					abort();
				}
				binaryReply.type = ntohl(binaryReply.type);
				if (binaryReply.reply == OK)
				{
					printf("Room %s left successfully\n", roomName.RoomName);
				}
			}
			else
			{
				printf("Sorry, wrong room name or you not in this room yet\n");
				sleep(1);
			}
		break;
		case 3:
			_regLogMessage->type = htonl(CLIENT_LOGOUT);
			if (send(_socket, (char*)_regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
			{
				perror("send error");
				abort();
			}
			if ((recv_len = recv(_socket, &binaryReply, sizeof(ServerReply), 0)) < 0)
			{
				perror("Receive error");
				abort();
			}
			binaryReply.type = ntohl(binaryReply.type);
			if (binaryReply.reply == OK)
			{
				onFlag = false;
			}
			else
			{
				perror("failed to logout");
				exit(EXIT_FAILURE);
			}
		break;
/*		case 7: 
			printf(KNRM"bye bye! %s come back soon!\n", _regLogMessage->username);
			_regLogMessage->type = htonl(CLIENT_LOGOUT);
			if (send(_socket, (char*)_regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
			{
				perror("Server Fail...");
				abort();
			}
			exit(EXIT_SUCCESS);
		break;*/
		default:
			printf("\x1b[31m Invalid character\x1b[0m\n");
			break;
		}
	}
}
