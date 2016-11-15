#include "common.h"
#include "ChatCreator.h"
#include "client_actions.h"
#include "client_chating.h"

int main()
{
	int socket = ClientInit();
	if (socket > 0)
	{
		ClientUI(socket);
	}
	return 0;
}


	
