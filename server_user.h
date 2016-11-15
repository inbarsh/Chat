#ifndef __SERVER_USER_H__
#define __SERVER_USER_H__
#include "common.h"
typedef struct ClientInfo ClientInfo;
typedef struct sockaddr_in sockaddr_in;
/**

*/
int GetCurrentSocket(void* _itr);
/**

*/
ClientInfo* CreateClientInfo(int _new_socket, sockaddr_in *_sockIn);
/**

*/
int ClientRegister(HashMap *_usersHash, Register_Login_Message* _message);
/**

*/
int ClientDeRegister(HashMap *_usersHash, Register_Login_Message* _message);
/**

*/
int ClientLogin(HashMap *_usersHash, Register_Login_Message* _message);
/**

*/
void ClientLogout(HashMap *_usersHash, Register_Login_Message* _message);

#endif /*__SERVER_ROOMS_H__*/
