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
#include "common.h"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define BLUE  "\x1b[34m"
#define KMAG  "\x1B[35m"

static int setup_multicast_socket(char _port[], char _address[], struct ip_mreq command);

int main (int argc, char *argv[])
{
	int sin_len;
	int socket;
	MultiCastMessage mcMessage;
	struct ip_mreq command;
	struct sockaddr_in sin;
	struct hostent *server_host_name;
	if( argc < 4)
	{
	printf("not enough arguments\n");
	return 0;
	}
	if ((server_host_name = gethostbyname(argv[2])) == 0)
	{
	 perror ("gethostbyname");
	 exit(EXIT_FAILURE);
	}
	memset(mcMessage.message, 0, MESSAGE_SIZE);
	socket = setup_multicast_socket(argv[3], argv[2], command);
	printf(KMAG"-hello %s, in this window,messages will popup-\n", argv[1]);
	printf("ip is: %s, port is: %s\n", argv[3] , argv[2]);
	printf(KRED"----------------------------------------------\n");
	while (1) 
	{

		sin_len = sizeof (sin);
		if (recvfrom(socket, &mcMessage, SERVER_MESSAGE_SIZE, 0, (struct sockaddr *) &sin, (socklen_t*)&sin_len)> 0)
		{
		   printf(KGRN"%s says: ", mcMessage.username);
		   printf (KRED"%s\n",mcMessage.message);
		} 
	}
	if (setsockopt (socket,IPPROTO_IP,IP_DROP_MEMBERSHIP,&command, sizeof (command)) < 0 )
	{
	  perror ("setsockopt:IP_DROP_MEMBERSHIP");
	}
	memset(mcMessage.message, 0, MESSAGE_SIZE);
	close(socket);
	return 0;
}

static int setup_multicast_socket(char _port[], char _address[], struct ip_mreq command)
{
	int loop = 1;
	int port;
	int socket_descriptor;
	struct sockaddr_in sin;
	memset (&sin, 0, sizeof (sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	port = atoi(_port);
	sin.sin_port = htons(port);
	if ((socket_descriptor = socket(PF_INET,SOCK_DGRAM, 0)) == -1) {
	 perror ("socket()");
	 exit (EXIT_FAILURE);
	}
	loop = 1;
	if (setsockopt (socket_descriptor,SOL_SOCKET,SO_REUSEADDR,&loop, sizeof (loop)) < 0)
	{
		 perror ("setsockopt:SO_REUSEADDR");
		 exit (EXIT_FAILURE);
	}
	if(bind( socket_descriptor,(struct sockaddr *)&sin,sizeof(sin)) < 0) 
	{
		 perror ("bind");
		 exit (EXIT_FAILURE);
	}
	loop = 1;
	if (setsockopt(socket_descriptor,IPPROTO_IP,IP_MULTICAST_LOOP,&loop, sizeof (loop)) < 0) 
	{
		 perror ("setsockopt:IP_MULTICAST_LOOP");
		 exit(EXIT_FAILURE);
	}
	command.imr_multiaddr.s_addr = inet_addr (_address);
	command.imr_interface.s_addr = htonl (INADDR_ANY);
	if (command.imr_multiaddr.s_addr == -1) 
	{
		 perror ("_address\n");
		 exit(EXIT_FAILURE);
	}
	if (setsockopt(socket_descriptor, IPPROTO_IP,IP_ADD_MEMBERSHIP, &command, sizeof (command)) < 0)
	{
		perror ("setsockopt:IP_ADD_MEMBERSHIP");
		exit(EXIT_FAILURE);
	}
	return socket_descriptor;
}


