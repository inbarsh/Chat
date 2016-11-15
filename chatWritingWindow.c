#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include "common.h"

#define KGRN  "\x1B[32m"
#define MAG   "\x1b[35m"

int main(int argc, char *argv[])
{
	int socket_descriptor;
	struct sockaddr_in address;
	MultiCastMessage mcMessage;
	char message[SERVER_MESSAGE_SIZE];
	int port = atoi(argv[2]);
	socket_descriptor = socket (AF_INET, SOCK_DGRAM, 0);
	if (socket_descriptor == -1) 
	{
	 perror ("socket");
	 exit(EXIT_FAILURE);
	}
	memset (&address, 0, sizeof (address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr (argv[1]);
	address.sin_port = htons (port);
	
	sprintf(mcMessage.message, "--%s %s -%s- --", argv[3], "has entered", argv[4]);
	strcpy(mcMessage.username,"admin");
	sendto(socket_descriptor, &mcMessage, sizeof(mcMessage), 0,(struct sockaddr *)&address, sizeof(address));
	strcpy(mcMessage.username, argv[3]);
	printf("ip is: %s, port is: %s\n", argv[1] , argv[2]);
	printf("please type your messages here\ntype -leave- to exit\n-------------------------\n");
	while (1) 
	{
		memset(message,0,sizeof(message));
		printf("->");
		fgets(mcMessage.message, MESSAGE_SIZE, stdin);
		if (strcmp(mcMessage.message, "leave\n") == 0)
		{
			sprintf(mcMessage.message, "--%s %s", argv[3], "has left the room--");
			strcpy(mcMessage.username,"admin");
			if (sendto(socket_descriptor, &mcMessage, sizeof(mcMessage), 0,(struct sockaddr *)&address, sizeof(address)) == -1)
			{
				 perror("sendto");
				 exit(EXIT_FAILURE);
			}
			exit(0);
		}
		else
		{

			if (sendto(socket_descriptor, &mcMessage, sizeof(mcMessage), 0,(struct sockaddr *)&address, sizeof(address)) == -1)
			{
				 perror("sendto");
				 exit(EXIT_FAILURE);
			}
		}

	} 
	return 0;
}
