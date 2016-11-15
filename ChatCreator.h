#ifndef __CHAT_CREATOR_H__
#define __CHAT_CREATOR_H__

/** @file ChatCreator.h
 *  @brief Creates new windows for the chat.
 *  @author Inbar Shalitin
 */

typedef struct sockaddr_in sockaddr_in;

/** 
 * @brief OpenNewRoom function. it opens the a chat window.
 */
void OpenNewRoom(sockaddr_in _address, char *_Username, char *_roomName);



 
#endif /* __CHAT_CREATOR_H__ */






