#include <stdlib.h>
#include <string.h>
#include "MultiCast_Addresses.h"

struct PortStruct
{
	int portList[NUM_OF_ROOMS];
	int occupiedList[NUM_OF_ROOMS];
};

PortStruct* CreateRoomList()
{
	int index = 0, currentPort;
	PortStruct *portStruct = malloc(sizeof(PortStruct));
	memset(portStruct, 0, sizeof(PortStruct));
	if (portStruct == NULL)
	{
		return 0;
	}
	for (currentPort = PORT_START; currentPort < PORT_START + NUM_OF_ROOMS; ++currentPort)
	{
		portStruct->portList[index] = currentPort;
		++index;
	}
	return portStruct;
}

int GetRoomPort(PortStruct* _portStruct)
{
	int i = 0;
	if (_portStruct == NULL)
	{
		return 0;
	}
	while(_portStruct->occupiedList[i] == 1)
	{
		if (i == NUM_OF_ROOMS)
		{
			return 0;
		}
		++i;
	}
	_portStruct->occupiedList[i] = 1;
	return(_portStruct->portList[i]);
}

void freeRoomPort(PortStruct* _portStruct, int _port)
{
	if (_portStruct != NULL && _port > 0 && _port < NUM_OF_ROOMS)
	{
		_portStruct->occupiedList[_port] = 0;
	}
}







