#define _XOPEN_SOURCE 500
#define _BSD_SOURCE

#include "DLlist.h"
#include "common.h"
#include "logger_pub.h"
#include "HashMap.h"

#define CAPACITY 100
#define BACK_LOG 128
#define SERVER_CAPACITY 1000
#define SERVER_TIMEOUT_SECONDS 300
/*************************************************************************************************/
/*STRUCTS*/
typedef enum ServerErr 
{
	SERVER_OK = 0,
	SERVER_UNINITIALIZED_ERR,
	SERVER_INIT_ERR,
	SERVER_ALLOCATION_ERR,
	SERVER_ACCEPT_CLIENT_ERR
} ServerErr;

typedef struct Socket
{
	struct sockaddr_in sin;
	int sockfd;
	struct timeval tv;
}Socket;

typedef struct User
{
	int isOnline;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
}User;

typedef struct Room
{
	struct sockaddr_in roomId;	
	char RoomName[MESSAGE_SIZE];
}Room;

typedef struct Server
{
	Socket socketStruct;
	List *clientList;
	fd_set masterSet;
	fd_set readSet;
	int maxfd;
	size_t activeSockets;
	struct timeval timeout;
	HashMap *users;
	HashMap *rooms;
}Server;
/*************************************************************************************************/
/*GLOBAL*/
int g_serverRunFlag = 1;
/*************************************************************************************************/
/*FUNCTIONS HEADERS*/
Server *Server_Create();
static int FreeSocket(void *_socket , void *_params);
void Server_Destroy(Server *_server);
ServerErr Server_Init(Server *_server);
ServerErr Server_AcceptClient(Server *_server);
static void DeleteClient(Server *_server , Socket *_clientSocket , void *_itr);
void Server_ServeClients(Server *_server);
static void CloseServer(int _signal);
ServerReply *RegistrationFunc(Register_Login_Message *_details, HashMap* _map);
ServerReply *DeregistrationFunc(Register_Login_Message *_details, HashMap* _map);
void *LoginFunc(Register_Login_Message *_details, HashMap* _map, HashMap* _rooms);
/*void *CreateRoomFunc(RoomNameMessage *_room, HashMap* _rooms);
void *JoinRoomFunc(RoomNameMessage *_room, HashMap* _rooms);
void *LeaveRoomFunc(RoomNameMessage *_room, HashMap* _rooms);
void ExitFunc();*/
static int HashFuncString(HashKey _key, size_t _size);
static int EquaFunc(HashKey _firstKey, HashKey _secondKey);
/*static int	KeyAction(const void *_key, void* _value, void * _context);*/
/*************************************************************************************************/
/*PROGRAM*/
int main()
{
	Server *server = NULL;
	struct sigaction act;
	act.sa_handler = CloseServer;
	sigaction(SIGINT , &act , NULL);
	if ( (server = Server_Create()) != NULL )
	{
		if ( Server_Init(server) == SERVER_OK )
		{
			while ( g_serverRunFlag )
			{
				Server_AcceptClient(server);
				Server_ServeClients(server);
			}
			Server_Destroy(server);
		}
	}
	return 0;
}
/*************************************************************************************************/
/*FUNCTIONS*/
Server *Server_Create()
{
	List *list = NULL;
	Server *server = NULL;
	HashMap *users = NULL;
	HashMap *rooms = NULL;
	LogCreate(LOG_DEB, 0, "ActivityLog");
	LogCreate(LOG_DEB, 1, "chatHistory");
	if ( (list = ListCreate()) != NULL )
	{
		if ( (server = malloc(sizeof(Server))) != NULL )
		{
			server->clientList = list;
			if ((users = HashCreate(CAPACITY, &HashFuncString, &EquaFunc)) != NULL)
			{
				server->users = users;
				if ((rooms = HashCreate(CAPACITY, &HashFuncString, &EquaFunc)) != NULL)
				{
					server->rooms = rooms;
				}
				else
				{
					LOG_DEBUG_PRINT("%s", 0, "fail to create chat rooms hash")
					HashDestroy(users);
					ListDestroy(list);
					server = NULL;
				}
			}
			else
			{
				LOG_DEBUG_PRINT("%s", 0, "fail to create users hash")
				ListDestroy(list);
				server = NULL;
			}
		}
		else
		{
			LOG_DEBUG_PRINT("%s", 0, "fail to allocate server struct")
			ListDestroy(list);
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "fail to create clients list")
	}
	return server;
}

static int FreeSocket(void *_socket , void *_params)
{
	free(_socket);
	return 1;
}

void Server_Destroy(Server *_server)
{
	if ( _server != NULL )
	{
		ListForEach(ListBegin(_server->clientList) , ListEnd(_server->clientList) , FreeSocket , NULL );
		ListDestroy(_server->clientList);
		FD_ZERO(&_server->masterSet);
		FD_ZERO(&_server->readSet);
		free(_server);
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "server struct is not initialized")
	}
}

ServerErr Server_Init(Server *_server)
{
	ServerErr err = SERVER_UNINITIALIZED_ERR;
	int optval = 1;
	
	if ( _server != NULL )
	{
		if ( (_server->socketStruct.sockfd = socket(AF_INET , SOCK_STREAM , 0)) != -1 )
		{
			if ( setsockopt(_server->socketStruct.sockfd , SOL_SOCKET , SO_REUSEADDR , &optval , sizeof(optval)) != -1 )
			{
				memset(&_server->socketStruct.sin , 0 , sizeof(_server->socketStruct.sin));
				_server->socketStruct.sin.sin_family = AF_INET;
				_server->socketStruct.sin.sin_addr.s_addr = INADDR_ANY;
				_server->socketStruct.sin.sin_port = htons(SERVER_PORT);
				if ( bind(_server->socketStruct.sockfd , (struct sockaddr*)&_server->socketStruct.sin , sizeof(_server->socketStruct.sin)) != -1 )
				{
					if ( listen(_server->socketStruct.sockfd , BACK_LOG) != -1 )
					{
						FD_ZERO(&_server->readSet);
						FD_ZERO(&_server->masterSet);
						FD_SET(_server->socketStruct.sockfd , &_server->masterSet);
						_server->maxfd = _server->socketStruct.sockfd;
						_server->activeSockets = 0;
						_server->timeout.tv_sec = SERVER_TIMEOUT_SECONDS;
						_server->timeout.tv_usec = 0;
						err = SERVER_OK;
					}
					else
					{
						LOG_DEBUG_PRINT("listen error: %s", 0, strerror(errno))
						err = SERVER_INIT_ERR;
					}		
				}
				else
				{
					LOG_DEBUG_PRINT("bind error: %s", 0, strerror(errno))
					err = SERVER_INIT_ERR;
				}
			}
			else
			{
				LOG_DEBUG_PRINT("setsockopt error: %s", 0, strerror(errno))
				err = SERVER_INIT_ERR;
			}
		}
		else
		{
			LOG_DEBUG_PRINT("socket error: %s", 0, strerror(errno))
			err = SERVER_INIT_ERR;
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "server struct is not initialized")
	}
	return err;
}

ServerErr Server_AcceptClient(Server *_server)
{
	size_t count;
	ServerErr err = SERVER_UNINITIALIZED_ERR; 
	int clientSocketFd , activity;
	struct sockaddr_in sin;
	socklen_t sinSize = sizeof(sin);
	Socket *s = NULL;
	struct timeval timeout;
	
	if ( _server != NULL )
	{
		_server->readSet = _server->masterSet;
		timerclear(&timeout);
		if ( (s = ListGetData(ListBegin(_server->clientList))) != NULL )
		{
			if ( gettimeofday(&timeout , NULL) != -1 )
			{
				timersub(&timeout , &s->tv , &timeout);
			}
		}
		if ( (activity = select(_server->maxfd + 1 , &_server->readSet , NULL , NULL , &timeout)) > 0 )
		{
			_server->activeSockets = activity;
			if ( FD_ISSET(_server->socketStruct.sockfd , &_server->readSet) )
			{
				if ( (clientSocketFd = accept(_server->socketStruct.sockfd , (struct sockaddr*)&sin , &sinSize)) != -1 )
				{
					ListCountItems(_server->clientList, &count);
					if (count < SERVER_CAPACITY )
					{
						if ( (s = malloc(sizeof(Socket))) != NULL )
						{
							FD_SET(clientSocketFd , &_server->masterSet);
							s->sin = sin;
							s->sockfd = clientSocketFd;
							if ( gettimeofday(&s->tv , NULL) != -1 )
							{
								if ( clientSocketFd > _server->maxfd )
								{
									_server->maxfd = clientSocketFd;
								}
								if ( ListPushTail(_server->clientList , s) == ERR_OK )
								{
									err = SERVER_OK;
								}
								else
								{
									if ( close(clientSocketFd) == -1 )
									{
										LOG_DEBUG_PRINT("close error: %s", 0, strerror(errno))
									}
									free(s);
									err = SERVER_ACCEPT_CLIENT_ERR;
								}
							}
							else
							{
								LOG_DEBUG_PRINT("gettimeofday error: %s", 0, strerror(errno))
								if ( close(clientSocketFd) == -1 )
								{
									LOG_DEBUG_PRINT("close error: %s", 0, strerror(errno))
								}
								free(s);
								err = SERVER_ACCEPT_CLIENT_ERR;
							}
						}
						else
						{
							if ( close(clientSocketFd) == -1 )
							{
								LOG_DEBUG_PRINT("close error: %s", 0, strerror(errno))
							}
							err = SERVER_ALLOCATION_ERR;
						}
					}
					else
					{
						if ( close(clientSocketFd) == -1 )
						{
							LOG_DEBUG_PRINT("close error: %s", 0, strerror(errno))
						}
						err = SERVER_OK;
					}
				}
				else
				{
					LOG_DEBUG_PRINT("accept error: %s", 0, strerror(errno))
					err = SERVER_ACCEPT_CLIENT_ERR;
				}
				--_server->activeSockets;
			}
		}
		else if ( activity == -1 )
		{
			if ( errno != EINTR )
			{
				LOG_DEBUG_PRINT("select error: %s", 0, strerror(errno))
				err = SERVER_ACCEPT_CLIENT_ERR;
			}
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "server struct is not initialized")
	}
	return err;
}

static void DeleteClient(Server *_server , Socket *_clientSocket , void *_itr)
{
	/*_clientSocket = ListRemove(_server->clientList , _itr);*/
	_clientSocket = ListRemove(_itr);
	FD_CLR(_clientSocket->sockfd , &_server->masterSet);
	if ( close(_clientSocket->sockfd) == -1 )
	{
		LOG_DEBUG_PRINT("close error: %s", 0, strerror(errno))
	}
	free(_clientSocket);
}

void Server_ServeClients(Server *_server)
{
	size_t numOfClients = 0 , clientIndex;
	char buffer[SERVER_MESSAGE_SIZE] = {'\0'};
	int receivedBytes , underTimeOut;
	MessageType type;
	Socket *clientSock = NULL;
	void *itr = NULL;
	struct timeval tv;
	void *reply = NULL;
	if ( _server != NULL )
	{
		ListCountItems(_server->clientList, &numOfClients);
		itr = ListBegin(_server->clientList);
		for ( clientIndex = 0 ; _server->activeSockets > 0 && clientIndex < numOfClients ; ++clientIndex )
		{
			clientSock = (Socket *)ListGetData(itr);
			gettimeofday(&tv , NULL);
			timersub(&tv , &clientSock->tv , &tv);
			if ( (underTimeOut = timercmp(&_server->timeout , &tv , >=)) && FD_ISSET(clientSock->sockfd, &_server->readSet) )
			{
				if ( (receivedBytes = recv(clientSock->sockfd , buffer , sizeof(buffer) , 0)) > 0 )
				{
					type = *((int*)buffer);
					/*type = (int)*buffer;*/
					switch (type) {
					case CLIENT_REGISTER:
						reply = RegistrationFunc((Register_Login_Message*)buffer, _server->users);
						break;
					case CLIENT_LOGIN:
						reply = LoginFunc((Register_Login_Message*)buffer, _server->users, _server->rooms);
						break; 
					case CLIENT_DEREGISTER:
						reply = DeregistrationFunc((Register_Login_Message*)buffer, _server->users);
						break;
					/*case CLIENT_EXIT:
					
						break;
					case CLIENT_CREATE_ROOM:
					
						break;
					case CLIENT_JOIN_ROOM:
					
						break;
					case CLIENT_LEAVE_ROOM:
						
						break; 
					case CLIENT_LOGOUT:
					
						break;
					case CLIENT_CHAT_MESSAGE
					
						break;
						*/
					default:
						break;
					}
					if ( send(clientSock->sockfd , reply , sizeof(reply) , 0) == -1 )
					{
						LOG_DEBUG_PRINT("send error: %s", 0, strerror(errno))
					}
					timeradd(&clientSock->tv , &tv , &clientSock->tv);
					ListPushTail(_server->clientList , clientSock);
					/*ListRemove(_server->clientList , itr);*/
					ListRemove(itr);
				}
				else
				{
					LOG_DEBUG_PRINT("recieve error: %s", 0, strerror(errno))
					DeleteClient(_server , clientSock , itr);
					ListCountItems(_server->clientList, &numOfClients);
				}
				--_server->activeSockets;
			}
			else if ( !underTimeOut )
			{
				LOG_DEBUG_PRINT("%s", 0, "client timeout")/*TODO - add which client*/
				DeleteClient(_server , clientSock , itr);
				ListCountItems(_server->clientList, &numOfClients);
			}
			itr = ListNext(itr);
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "server struct is not initialized")
	}
}

static void CloseServer(int _signal)
{
	g_serverRunFlag = 0;
}

ServerReply *RegistrationFunc(Register_Login_Message *_details, HashMap* _map)
{
	ServerReply *answer = NULL;
	User *user = NULL;
	ADTErr ret;
	if (_details != NULL && _map != NULL)
	{
		if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
		{
			answer->type = SERVER_REPLY;
			if ((user = calloc(1, sizeof(User))) != NULL)
			{	
				strcpy(user->username, _details->username);
				strcpy(user->password, _details->password);
				ret = HashInsert(_map, (HashKey)_details->username, (HashData)user);
				if (ret == ERR_OK)
				{
					LOG_DEBUG_PRINT("%s registered succesfuly", 0, _details->username)
					answer->reply = OK; 
				}
				else if (ret == ERR_DUPLICATE_KEY)
				{
					LOG_DEBUG_PRINT("%s could not register - username exists in hash", 0, _details->username)
					answer->reply = ALREADY_EXISTS;
				}
				else
				{
					LOG_DEBUG_PRINT("%s error register", 0, _details->username)
					answer->reply = CRITICAL_PROBLEM;
				}
			}
		}
		else
		{
			LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or users hash are not initialized")
	}
	return answer;
}

ServerReply *DeregistrationFunc(Register_Login_Message *_details, HashMap* _map)
{
	ServerReply *answer = NULL;
	ADTErr ret;
	HashData data;
	if (_details != NULL && _map != NULL)
	{
		if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
		{
			answer->type = SERVER_REPLY;
			ret = HashRemove(_map, (HashKey)_details->username, &data);
			if (ret == ERR_OK)
			{
				LOG_DEBUG_PRINT("%s deregistered succesfuly", 0, _details->username)
				answer->reply = OK; 
			}
			else if (ret == ERR_NOT_FOUND)
			{
				LOG_DEBUG_PRINT("%s could not deregister - user doesn't exixst in hash", 0, _details->username)
				answer->reply = NO_MATCH;
			}
			else
			{
				LOG_DEBUG_PRINT("%s error deregister", 0, _details->username)
				answer->reply = CRITICAL_PROBLEM;
			}
		}
		else
		{
			LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or users hash are not initialized")
	}
	return answer;
}
/*
typedef struct User
{
	int isOnline;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
}User;

typedef struct Room
{
	struct sockaddr_in roomId;	
	char RoomName[MESSAGE_SIZE];
}Room;*/
void *LoginFunc(Register_Login_Message *_details, HashMap* _map, HashMap* _rooms)
{
	void *answer = NULL;
	/*char str[SERVER_MESSAGE_SIZE] = {0};*/
	HashData user;
	if (_details != NULL && _map != NULL)
	{
		user = HashFind(_map, (HashKey)_details->username);
		if (user != NULL)
		{
			if (((User*)user)->isOnline == 0)
			{
				if (strcmp(((User*)user)->password, _details->password) == 0)
				{
					if ((answer = calloc(1, sizeof(AvailableRoomsMessage))) != NULL)
					{
						((User*)user)->isOnline = 1;
						((AvailableRoomsMessage*)answer)->type = SERVER_AVAILABLE_ROOMS;
						strcpy(((AvailableRoomsMessage*)answer)->message, "You loged in succesfuly! Now you can choose which chat room you would like to enter from the list below\n OR\n you can open a new chat room.");
						/*HashMap_ForEach(_rooms, KeyAction, str);*/
					}
					else
					{
						LOG_DEBUG_PRINT("%s", 0, "fail to allocate available rooms struct")
					}
				}
				else
				{
					if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
					{
						((ServerReply*)answer)->type = SERVER_REPLY;
						((ServerReply*)answer)->reply = NO_MATCH;
					}
					else
					{
						LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
					}
				}
			}
			else
			{
				if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
				{
					((ServerReply*)answer)->type = SERVER_REPLY;
					((ServerReply*)answer)->reply = ALREADY_EXISTS;
				}
				else
				{
					LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
				}
			}
		}
		else
		{
			LOG_DEBUG_PRINT("%s could not login", 0, _details->username)
			if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
			{
				((ServerReply*)answer)->type = SERVER_REPLY;
				((ServerReply*)answer)->reply = CRITICAL_PROBLEM;
			}
			else
			{
				LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
			}
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or users hash are not initialized")
	}
	return answer;
}

/*ServerReply *CreateRoomFunc(RoomNameMessage *_room, HashMap* _rooms)
{
	ServerReply *answer = NULL;
	if (_room != NULL && _rooms != NULL)
	{
		if (HashMap_Insert(_rooms, _room->message, room) == ERR_OK)
		{
			if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
			{
				answer->type = SERVER_REPLY;
				answer->reply = OK; 
				JoinRoomFunc(_room, _rooms);
			}
			else
			{
				LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
			}
		}
		else
		{
			LOG_DEBUG_PRINT("could not create room %s", 0, _room->message)
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or rooms hash are not initialized")
	}
	return answer;
}

void *JoinRoomFunc(RoomNameMessage *_room, HashMap* _rooms)
{
	void *answer = NULL;
	if (_room != NULL && _rooms != NULL)
	{
		if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
		{
			answer->type = 	SERVER_REPLY;
			if (HashMap_ForEach(_rooms, KeyValueActionFunction _action, _room->message) != 0)TODO - add client to the room by the room name and username
			{
				answer->reply = 1; 
			}
			else
			{
				LOG_DEBUG_PRINT("could not join room %s", 0, _room->message)
			}
		if (HashMap_Insert(_rooms, _room->message, room) == ERR_OK)
		{
			if ((answer = calloc(1, sizeof(RoomIdMessage))) != NULL)
			{
				((RoomIdMessage*)answer)->type = CREATE_ROOM_REPLY;
				((RoomIdMessage*)answer)->roomId = TODO; 
			}
			else
			{
				LOG_DEBUG_PRINT("%s", 0, "fail to allocate room id struct")
			}
		}
		else
		{
			if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
			{
				((ServerReply*)answer)->type = CREATE_ROOM_REPLY;
				LOG_DEBUG_PRINT("could not create room %s", _room->message)
			}
			else
			{
				LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
			}
		}
		
		}
		else
		{
			LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or rooms hash are not initialized")
	}
	return answer;
}

void *LeaveRoomFunc(RoomNameMessage *_room, HashMap* _rooms)
{
	ServerReply *answer = NULL;
	if (_room != NULL && _rooms != NULL)
	{
		if ((answer = calloc(1, sizeof(ServerReply))) != NULL)
		{
			answer->type = SERVER_REPLY;
			if (HashMap_ForEach(_rooms, KeyValueActionFunction _action, _room->message != 0)TODO
			{
				answer->reply = 1; 
			}
			else
			{
				LOG_DEBUG_PRINT("could not leave room %s", 0, _room->message)
			}
		}
		else
		{
			LOG_DEBUG_PRINT("%s", 0, "fail to allocate reply struct")
		}
	}
	else
	{
		LOG_DEBUG_PRINT("%s", 0, "details struct or rooms hash are not initialized")
	}
	return answer;
}*/
/*use in case of an empty room*/
/*void CloseRoom(AvailableRoomsMessage *_room, HashMap* _rooms) 
{
	if (_room != NULL && _rooms != NULL)
	{
		HashMap_Remove(_rooms, _room->message);
	}
}*/
/*void ExitFunc(HashMap* _map)
{

}*/

/*static int HashFuncString(HashKey _key, )
{
    unsigned long hash = 5381;
    int c;
	unsigned char *str = (unsigned char*)_key;
    while  (0 != (c = *str++))
    {
        hash = ((hash << 5) + hash) + c; 
	}
    return (size_t)hash;
}*/

static int HashFuncString(HashKey _key, size_t _size)
{
	int intKey = 0, i;
	char* key = (char*)_key;
	for (i = 0; i < USERNAME_LEN; ++i)
	{
		intKey += *(char*)key;
		++key;
	}
	_size = SERVER_CAPACITY;
	intKey = (intKey % _size); 
	return intKey;
}

static int EquaFunc(HashKey _firstKey, HashKey _secondKey)
{
    
    return (strcmp((char*)_firstKey, (char*)_secondKey) == 0);
}

/*static int	KeyAction(const void *_key, void* _value, void * _context)
{
	strcat((char*)_context,((Room*)_value)->RoomName);
	return 0;
}

static int SubscriberPrintToFile(const void *_key, void *_subscriber, void *_filePtr)
{
	if ( fprintf((FILE*)_filePtr , "IMSI: %s | Incoming Calls: %d | Outgoing Calls: %d | Received SMS: %d | Sent SMS: %d | MB Downloaded: %f | MB Uploaded: %f\n" , 
			((struct Subscriber*)_subscriber)->IMSI , ((struct Subscriber*)_subscriber)->incomingCallsSeconds , 
			((struct Subscriber*)_subscriber)->outgoingCallsSeconds , ((struct Subscriber*)_subscriber)->SMSReceived , 
			((struct Subscriber*)_subscriber)->SMSSent , ((struct Subscriber*)_subscriber)->downloads , 
			((struct Subscriber*)_subscriber)->uploads) < 0 )
	{
		return 0;
	}
	((struct Subscriber*)_subscriber)->incomingCallsSeconds = 0;
	((struct Subscriber*)_subscriber)->outgoingCallsSeconds = 0;
	((struct Subscriber*)_subscriber)->SMSReceived = 0;
	((struct Subscriber*)_subscriber)->SMSSent = 0;
	((struct Subscriber*)_subscriber)->downloads = 0;
	((struct Subscriber*)_subscriber)->uploads = 0;
	return 1;
}*/
