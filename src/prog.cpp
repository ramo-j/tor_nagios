/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "checks.h"
#include "consts.h"
#include "misc.h"

int main(int argc, char** argv)
{
	bool verbose = 0;
	std::string timeoutStr = "60";
	std::string portStr = "9051";
	std::string warning = "0";
	std::string critical = "0";
	std::string hostname = "localhost";
	std::string authPass = "password";
	std::string checkType = "1";
	int c;
	int timeout;
	int port;

	while ((c = getopt(argc, argv, ":vVhH:a:t:w:c:p:C:")) != -1)
	{
		switch (c)
		{
		case 'v':
			verbose = 1;
			break;
		case 'V':
			version(argv[0]);
			return RET_UNKNOWN;
		case 'h':
			usage(argv[0]);
			return RET_UNKNOWN;
		case 'H':
			hostname = optarg;
			break;
		case 'a':
			authPass = optarg;
			break;
		case 't':
			timeoutStr = optarg;
			break;
		case 'w':
			warning = optarg;
			break;
		case 'c':
			critical = optarg;
			break;
		case 'C':
			checkType = optarg;
			break;
		case 'p':
			portStr = optarg;
			break;
		case ':':
		case '?':
		default:
			usage(argv[0]);
			return RET_UNKNOWN;
		}
	}

	timeout = atoi(timeoutStr.c_str());
	port = atoi(portStr.c_str());

	std::string message = "";
	int retVal = RET_UNKNOWN;

	switch (atoi(checkType.c_str()))
	{
	case CHK_SOCKSFETCH:
		retVal = check_socksFetch(hostname.c_str(), port, authPass.c_str(), warning.c_str(), critical.c_str(), timeout, verbose, &message);
		break;
	case CHK_HIBERNATE:
		retVal = check_hibernationState(hostname.c_str(), port, authPass.c_str(), warning.c_str(), critical.c_str(), verbose, &message);
		break;
	case CHK_BANDWIDTH:
		retVal = check_bandwidth(hostname.c_str(), port, authPass.c_str(), warning.c_str(), critical.c_str(), verbose, &message);
		break;		
	}

	std::cout << message << std::endl;
	return retVal;
}
