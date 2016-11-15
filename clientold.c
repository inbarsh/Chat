#define _XOPEN_SOURCE 500
#define _BSD_SOURCE

#include "common.h"

#define NUM_OF_CLIENTS 5
/*************************************************************************************************/
/*STRUCTS*/
typedef struct Clinet
{
	int    m_clientsocket;
	char   m_msg[SERVER_MESSAGE_SIZE];
	size_t m_msglen;
	int    m_isconnected;
} Clinet;
/*************************************************************************************************/
/*FUNCTIONS HEADERS*/
Register_Login_Message *UI();
/*void RegistrationFunc(Register_Login_Message *_details);
void DeregistrationFunc(Register_Login_Message *_details);
void LoginFunc(Register_Login_Message *_details);*/
void ExitFunc();
int sender();
int ReciveMessege(Clinet _clinet);
int SendMessege(Clinet* _clinet, struct sockaddr_in* _serverAddr);
int creatsocket(Clinet* _clinet, int _index);
int Connectsocket(Clinet* _clinet, struct sockaddr_in* _serverAddr);
/*int Disconnectsocket(Clinet* _clinet);*/
/*************************************************************************************************/
/*PROGRAM*/
int main()
{
	UI();
	return sender();
}
/*************************************************************************************************/
/*FUNCTIONS*/
Register_Login_Message *UI()
{
	char c, cExit;
	Register_Login_Message *details = calloc(1, sizeof(Register_Login_Message));
	printf("Hi! Welcome to Almog Chat! hope you will enjoy the app.\n Press 1 to REGISTER\n Press 2 to LOGIN\n \x1b[31m IMPORTANT - press this option only if you already have a username and password\x1b[0m\n Press 3 to Deregister\n \x1b[31m IMPORTANT - you will no longer have username and password if you will choose this option\x1b[0m\n Press 4 to Exit\n>");
	scanf(" %c",&c);
	while (c == '1' || c == '2' || c == '3')
	{
		printf("Please enter your exist username or the username you wish to create\n Username length should be %d characters\n", USERNAME_LEN-1);
		scanf(" %s",details->username);
		printf("Please enter your exist password or the password you wish to create\n Password length should be %d characters\n", PASSWORD_LEN-1);
		scanf(" %s",details->password);
		if (strlen(details->username) != 7 || strlen(details->password) != 7) 
		{
			printf("\x1b[31m Username and/or password are not in the right length!\x1b[0m\n Try again\n");
		}
		else 
		{
			break;
		}
	}
	switch (c){
	case '1':
		details->type = CLIENT_REGISTER; 
		/*RegistarationFunc(details);*/
		break;
	case '2':
		details->type = CLIENT_LOGIN;
		/*LoginFunc(details);*/
		break;
	case '3':
		printf("\x1b[31m Are you sure you want to deregister? press Y if yes or N if no\x1b[0m\n");
		scanf(" %c",&cExit);
		if (cExit == 'y' || cExit == 'Y')
		{
			details->type = CLIENT_DEREGISTER;
			/*DeregistrationFunc(details);*/
		}
		break;
	case '4':
		printf("\x1b[31m Are you sure you want to EXIT? press Y if yes or N if no\x1b[0m\n");
		scanf(" %c",&cExit);
		if (cExit == 'y' || cExit == 'Y')
		{
			details->type = CLIENT_EXIT;
			ExitFunc();
		}
		break;
	default:
		printf("\x1b[31m Invalid character\x1b[0m\n");
		break;
	}
	return details;
}

/*void RegistrationFunc(Register_Login_Message *_details)
{
	ServerReply *ret = NULL;
	if (ret->reply != 1)
	{
		printf("\x1b[31m Invalid username and/or password or problem to connect server!\x1b[0m\n Try again\n");
	}
	printf("Going back to main menu...\n");
}

void DeregistrationFunc(Register_Login_Message *_details)
{
	ServerReply *ret = NULL;
	if (ret->reply != 1)
	{
		printf("\x1b[31m Oops! Invalid username and/or password or problem to connect server!\x1b[0m\n Going back to main menu...\n");
	}	
}

void LoginFunc(Register_Login_Message *_details)
{
	ServerReply *ret = NULL;
	if (ret->reply != 1)
	{
		printf("\x1b[31m Oops! Invalid username and/or password or problem to connect server!\x1b[0m\n Going back to main menu...\n");
	}
}*/

void ExitFunc()
{
	printf("prees ctrl+c\n");
}

int sender()
{
	Clinet clinet[NUM_OF_CLIENTS];
	char *message = NULL;
	struct sockaddr_in serverAddr;
	int index=0;
	for (index=0; index<NUM_OF_CLIENTS; ++index)
	{
		clinet[index].m_clientsocket=0;
	}
	for (index=0; index<NUM_OF_CLIENTS; ++index)
	{
		clinet[index].m_isconnected=0;
	}
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_aton(SERVER_IP, &serverAddr.sin_addr);
	while (1)
	{
		for (index=0; index<NUM_OF_CLIENTS; ++index)
		{
			message = (char*)UI();
			memset(&(clinet[index].m_msg), 0, sizeof(clinet[index].m_msg));
			snprintf(clinet[index].m_msg, SERVER_MESSAGE_SIZE, "%s", message);
			if(0 == creatsocket(&clinet[index], index))
			{
				printf("create fail\n");
				return 0;
			}
			if(0 == Connectsocket(&clinet[index], &serverAddr))
			{
				printf("connect fail\n");
				return 0;
			}
			/*if(0 == Disconnectsocket(&clinet))
			{
				return 0;
			}*/
			if (0 == SendMessege( &clinet[index], &serverAddr))
			{
				printf("send fail\n");
				return 0;
			}
			if (0 == ReciveMessege(clinet[index]))
			{
				printf("rec fail\n");
				return 0;
			}
		}
	}

	return 1;
}

int creatsocket(Clinet* _clinet, int _index)
{
	if (_clinet->m_clientsocket == 0)
	{
		_clinet->m_clientsocket= socket(AF_INET, SOCK_STREAM, 0);
		if (_clinet->m_clientsocket == -1)
		{
			printf("clientsocket %s\n", strerror(errno));
			return 0;
		}
	}
	return 1;
}

int ReciveMessege(Clinet _clinet)
{
	char buffer[SERVER_MESSAGE_SIZE];
	ssize_t result;
    MessageType type;
	result = read(_clinet.m_clientsocket, buffer, 100);
    if (result == -1)
    {
    	printf("%s\n", strerror(errno));
    	return 0;
    }
    type = *((int*)buffer);
    /*type = (int)*buffer;*/
    printf("%d\n", type);
    switch (type) {
		case SERVER_REPLY:
			printf("type - %d, reply - %d\n", ((ServerReply*)buffer)->type, ((ServerReply*)buffer)->reply);
			break;
		case SERVER_AVAILABLE_ROOMS:
			printf("reply - %s\n", ((AvailableRoomsMessage*)buffer)->message);
			break; 
		/*case SERVER_ROOM_ID:
			reply = DeregistrationFunc((Register_Login_Message*)buffer, _server->users);
			break;*/
		default:
			break;
	}
    return 1;
}

int Connectsocket(Clinet* _clinet, struct sockaddr_in* _serverAddr)
{
	if(_clinet->m_isconnected == 0)
	{
		ssize_t result;
		_clinet->m_isconnected = 1;
		result= connect(_clinet->m_clientsocket, (const struct sockaddr*)_serverAddr, sizeof(*_serverAddr));
		if (result == -1)
		{
			printf("connect %s\n", strerror(errno));
			return 0;
		}
	}
	return 1;
}
/*int Disconnectsocket(Clinet* _clinet)
{
	ssize_t result;
	result= shutdown(_clinet->m_clientsocket, SHUT_RDWR);
	if (result == -1)
	{
		printf("connect %s\n", strerror(errno));
		return 0;
	}
	return 1;
}*/


int SendMessege(Clinet* _clinet, struct sockaddr_in* _serverAddr)
{
	ssize_t result;
	_clinet->m_msglen= sizeof(_clinet->m_msg)+1;
	result= write(_clinet->m_clientsocket, _clinet->m_msg, _clinet->m_msglen);
	if (result == -1)
	{
		printf("send %s\n", strerror(errno));
		return 0; 
	}

	return 1;
}

