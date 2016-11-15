#include "DLlist.h"
#include "logger_pub.h"
#include "HashMap.h"
#include "server_user.h"

/*************************************************************************************************/
/*STRUCTS*/
typedef struct timeval timeval;

struct ClientInfo
{
	int socket;
	char username[USERNAME_LEN];
	sockaddr_in *sockaddr;
	timeval ttl;
};

typedef struct
{
	char username[USERNAME_LEN]; /**> for 1: username. 2: room name */
	char password[PASSWORD_LEN];
	int active;
}UserNPass;
/*************************************************************************************************/
/*FUNCTIONS*/
int GetCurrentSocket(void* _itr)
{
	ClientInfo *currentClientInfo = NULL;
	currentClientInfo = ListGetData(_itr);
	return currentClientInfo->socket;
}

ClientInfo* CreateClientInfo(int _new_socket, sockaddr_in *_sockIn)
{
	ClientInfo* ci = NULL;
	char user[USERNAME_LEN] = "guest";
	ci = malloc(sizeof(ClientInfo));
	if (ci != NULL)
	{
		ci->socket = _new_socket;
		ci->sockaddr = _sockIn;
		strcpy(ci->username, user);
	}
	return ci;	
}	

int ClientRegister(HashMap *_users, Register_Login_Message *_message)
{
	UserNPass *userToRegister = malloc(sizeof(UserNPass));
	strcpy(userToRegister->username, _message->username);
	strcpy(userToRegister->password, _message->password);
	userToRegister->active = false;
	if (HashInsert(_users, _message->username, userToRegister) == ERR_OK)
	{
		return true;
	}
	return false;
}

int ClientDeRegister(HashMap *_users, Register_Login_Message *_message)
{
	void *container;
	UserNPass *userInHash;
	char password[PASSWORD_LEN];
	if((container = HashFind(_users, _message->username)) == NULL)
	{
		return false;	
	}
	strcpy(password, _message->password);
	userInHash = (UserNPass *)ListGetData((ListItr)container);
	if(strcmp(userInHash->password, password) == 0)
	{
		if(HashRemove(_users, _message->username, &container) != ERR_OK)
		{
			return false;	
		}		
		free(userInHash);
		return true;
	}
	return false;
}

int ClientLogin(HashMap *_users, Register_Login_Message *_message)
{
	void *container;
	UserNPass *userInHash = NULL;
	int ans;
	if((container = HashFind(_users, _message->username)) == NULL)
	{
		return false;	
	}
	if ((userInHash = (UserNPass *)ListGetData((ListItr)container)) != NULL)
	{
		if (userInHash->active == true)
		{
			return false;
		}
		ans = strcmp(_message->password, userInHash->password);
		if (ans == 0)
		{
			printf("login\n");
			userInHash->active = 0;
			return true;
		}
	}
	return false;
}

void ClientLogout(HashMap *_users, Register_Login_Message *_message)
{
	void *container;
	UserNPass *userInHash;
	if((container = HashFind(_users, _message->username)) == NULL)
	{
		return;	
	}
	userInHash = ListGetData(container);
	userInHash->active = false;
	return;	
}

/*static char* GetAvailableRooms(SafeHashMap *_rooms)
{
	char *availableRooms = malloc(sizeof(char*)*SERVER_MESSAGE_SIZE);
	availableRooms = "Only -experis- room is open. Currently, you can ask IT for other rooms available.\n";
	return availableRooms;
}*/
