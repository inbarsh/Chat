#ifndef __CLIENT_ACTIONS_H__
#define __CLIENT_ACTIONS_H__
#include "common.h"
/**

*/
int ClientInit();
/**

*/
void ClientUI(int _socket);
/**

*/
int CheckReply(ServerReply _binaryReply);
/**

*/
int GetUserName(Register_Login_Message *_regLogMessage);

#endif /*__CLIENT_ACTIONS_H__*/
