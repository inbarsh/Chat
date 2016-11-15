#include "client_actions.h"
#include "client_chating.h"

int ClientInit()
{
	struct sockaddr_in si_server;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		return sock;
	}
	memset(&si_server, 0, sizeof(si_server));
	si_server.sin_family = AF_INET;
	si_server.sin_addr.s_addr = inet_addr(SERVER_IP);
	si_server.sin_port = htons(SERVER_PORT);
	if (connect(sock, (struct sockaddr*)&si_server, sizeof(si_server)) == -1)
	{
		printf("Sorry, Cannot connect Server\n");
        exit(EXIT_FAILURE);
	}
	return sock;
}

void ClientUI(int _socket)
{
	int userChoice, ans;
	char cExit;
	AvailableRoomsMessage availableRooms;
	int recv_len;
	ServerReply binaryReply;
	Register_Login_Message regLogMessage;
	while(1)
	{
		printf("Hi! Welcome to Almog Chat! hope you will enjoy the app.\n Press 1 to REGISTER\n Press 2 to LOGIN\n \x1b[31m IMPORTANT - press this option only if you already have a username and password\x1b[0m\n Press 3 to Deregister\n \x1b[31m IMPORTANT - you will no longer have username and password if you will choose this option\x1b[0m\n Press 4 to Exit\n>");
		scanf("%d", &userChoice);
		switch (userChoice)
		{
		case 1: 
			ans = GetUserName(&regLogMessage);
			if (ans)
			{
				regLogMessage.type = CLIENT_REGISTER;
				if (send(_socket, (char*)&regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
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
				if (CheckReply(binaryReply))
				{
					printf("Yay! you are a registered user now. Now you can login and start chating with your online friends and co-workers\n");
				}
				else
				{
					printf("\x1b[31m Sorry, something went wrong\x1b[0m\n");
				}			
			}
			else
			{
				sleep(1);
			}
			break;
		
		case 2: 
			ans = GetUserName(&regLogMessage);
			if (ans)
			{
				regLogMessage.type = CLIENT_LOGIN;
				if (send(_socket, (char*)&regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
					{
						perror("send error");
						abort();
					}
				if ((recv_len = recv(_socket, &availableRooms, sizeof(availableRooms), 0)) < 0)
				{
					perror("receive error");
					abort();
				}
				availableRooms.type = availableRooms.type;
				if (availableRooms.type == SERVER_AVAILABLE_ROOMS)
				{
					RoomsUI(&regLogMessage, &availableRooms, _socket);
					printf("rhrhrhrhrhrhrfb\n");
				}          
				else
				{
					printf("\x1b[31m Sorry, no match or user is currently online\x1b[0m\n");
				}								
			}
			else
			{
				sleep(1);
			}
		break;		
		case 3:
			printf("\x1b[31m Are you sure you want to deregister? press Y if yes or N if no\x1b[0m\n");
			scanf(" %c",&cExit);
			if (cExit == 'y' || cExit == 'Y')
			{
				memset(&regLogMessage, 0, sizeof(Register_Login_Message));
				ans = GetUserName(&regLogMessage);
				if (ans)
				{
					regLogMessage.type = CLIENT_DEREGISTER;
					if (send(_socket, (char*)&regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
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
					if (CheckReply(binaryReply))
					{
						printf("deristered\n");
					}
					else
					{
						printf("failed to deregister\n");
					}			
				}
				else
				{
					sleep(1);
				}			
			}
		break;
		case 4:
			printf("\x1b[31m Are you sure you want to exit? press Y if yes or N if no\x1b[0m\n");
			scanf(" %c",&cExit);
			if (cExit == 'y' || cExit == 'Y')
			{
				regLogMessage.type = CLIENT_EXIT;
				if (send(_socket, (char*)&regLogMessage, sizeof(Register_Login_Message), 0) < 0) 
				{
					perror("send error");
					abort();
				}
				return;
			}
			break;
		default:
			printf("\x1b[31m Invalid character\x1b[0m\n");
			break;
		}
	}
}	

int CheckReply(ServerReply _binaryReply)
{
	printf("%d\n", _binaryReply.reply);
	if (_binaryReply.reply == OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int GetUserName(Register_Login_Message *_regLogMessage)
{
	printf("Please enter your exist username or the username you wish to create\n Username length should be %d characters\n", USERNAME_LEN-1);
	scanf("%s", _regLogMessage->username);
	printf("Please enter your exist password or the password you wish to create\n Password length should be %d characters\n", PASSWORD_LEN-1);
	scanf("%s", _regLogMessage->password);
	if (strlen(_regLogMessage->username) == 7 && strlen(_regLogMessage->password) == 7) 
	{
		return true;
	}
	printf("\x1b[31m Username and/or password are not in the right length!\x1b[0m\n Try again\n");
	return false;
}
