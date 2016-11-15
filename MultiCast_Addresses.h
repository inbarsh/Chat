#ifndef __CHAT_SERVER_H__
#define __CHAT_SERVER_H__

/** @file MultiCast_Server.h
 *  @brief Creates all the addresses for the multicast use. 
 *  @author Inbar Shalitin
 */

#define MULTICAST_ADDRESS "226.0.1.152"
#define NUM_OF_ROOMS 100
#define PORT_START 3500

typedef struct PortStruct PortStruct;


/** 
 * @brief CreateRoomList function. Creates all the ports for usage.
 * @return 1 if ok. 0 on failure.
 */
PortStruct* CreateRoomList();


/** 
 * @brief GetRoomPort function. get an available port room.
 * @return the new port if available. 0 on failure.
 */
int GetRoomPort(PortStruct* _portStruct);

/** 
 * @brief freeRoomPort function. Free an unused port.
 * @return the new port if available. 0 on failure.
 */
void freeRoomPort(PortStruct* _portStruct, int _port);


 
#endif /* __CHAT_SERVER_H__ */






