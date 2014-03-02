/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef CSOCKET_H
#define	CSOCKET_H

#include <netinet/in.h>

#define SOCKETBUFFERLEN 1024

class csocket
{
public:
	csocket();
	csocket(int fd, struct sockaddr_in* addr);
	~csocket();

	void Configure(const char* addr, const unsigned short port);
	void Connect();
	void Close();

	long Read(unsigned char* data);
	long Send(const unsigned char*, const unsigned long len);
private:
	unsigned char* buffer;
	int sockfd;
	struct sockaddr_in* cli_addr;
};

#endif
