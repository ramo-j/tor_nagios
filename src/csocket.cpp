/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "csocket.h"
#include "socketException.h"

/*
 Create a 'blank' csocket.
 */
csocket::csocket()
{
	buffer = new unsigned char[SOCKETBUFFERLEN];
}

/*
 Create a csocket based on a socket fd and an address which have already been
 connected. Used by ssocket when accepting new connections.
 */
csocket::csocket(int fd, struct sockaddr_in* addr)
{
	buffer = new unsigned char[SOCKETBUFFERLEN];

	sockfd = fd;
	cli_addr = addr;
}

csocket::~csocket()
{
	delete [] buffer;
	delete cli_addr;
}

/*
 Configure the csocket to talk to an address / port
 */
void csocket::Configure(const char* addr, const unsigned short port)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	cli_addr = new sockaddr_in;
	struct hostent* server = gethostbyname(addr);

	if (sockfd < 0)
	{
		THROWSOCKETEXCEPTION("Error creating socket");
	}

	if (server == NULL)
	{
		THROWSOCKETEXCEPTION("Error no such host");
	}

	// fill in the server details to connect to
	bzero((char *) cli_addr, sizeof (*cli_addr));
	cli_addr->sin_family = AF_INET;
	bcopy((char *) (server->h_addr), (char *) &(cli_addr->sin_addr.s_addr), (unsigned long)server->h_length);
	cli_addr->sin_port = htons(port);
}

/*
 Try to connect to the configured address and port
 */
void csocket::Connect()
{
	if (connect(sockfd, (struct sockaddr *) cli_addr, sizeof (*cli_addr)) < 0)
	{
		THROWSOCKETEXCEPTION("Failed to connect");
	}
}

/*
 Close off the socket
 */
void csocket::Close()
{
	close(sockfd);
}

/*
 Read from the socket into a buffer 'data'. data should already be allocated and
 no longer than SOCKETBUFFERLEN bytes.
 */
long csocket::Read(unsigned char* data)
{
	bzero(buffer, SOCKETBUFFERLEN);
	long n = read(sockfd, buffer, SOCKETBUFFERLEN - 1);

	if (n < 0)
	{
		THROWSOCKETEXCEPTION("Error occurred reading from socket");
	}

	bcopy(buffer , data, (unsigned long) n);

	return n;
}

/*
 Write data to the socket. 
 */
long csocket::Send(const unsigned char* data, const unsigned long len)
{
	return write(sockfd, data, len);
}

