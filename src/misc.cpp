/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#include <iostream>
#include "misc.h"

void usage(char* argv0)
{
	std::cout << "Usage\n " << argv0 << " [-hvV] [-a authPasswd] [-c critical] [-C checkType] [-H hostname] [-p port] [-t timeout] [-w warning]\n" <<
			"Options:\n" <<
			" -h\n\tDisplay this help text\n" <<
			" -v\n\tverbose output\n" <<
			" -V\n\tPrint version and exit\n" <<
			" -a authPasswd\n\tAuthentication password to use (default password)\n" <<
			" -c critical\n\tcritical threshold (default 0)\n" <<
			" -C checkType\n\tWhat we are checking. Specific values below. (default 1)\n" <<
			" -H hostname\n\tHostname to connect to (default localhost)\n" <<
			" -p port\n\tControl port to connect to (default 9051)\n" <<
			" -t timeout\n\tconnect timeout (default 60)\n" <<
			" -w warning\n\tWarning threshold (default 0)\n" <<
			"\nCheck Types are as follows:\n" <<
			"1 - Page fetch through socks port. Socks port must be enabled.\n"
			"2 - Hibernation state check. Warn and Crit values: 0 = awake, 1 = soft, 2 = hard hibernation. '-w 1 -c 2' recommended\n" <<
			"3 - Bandwidth in bytes/sec (up and down). Warn and Crit must be full values (eg, 20480 instead of 20KB).\n"
			"4 - OR Port reachability (1 for success, 0 for failure)\n"
			"5 - DIR Port reachability (1 for success, 0 for failure)\n"
			"6 - Bytes remaining in accounting before hubernation (both read and write)\n"
			"\nWarning and Critical thresholds can take the values:\n" <<
			"\tx\talert if less than 0 or greater than or equal to x\n" <<
			"\tx:\talert if less than x\n" <<
			"\t~:x\talert if greater than x\n" <<
			"\tx:y\talert if less than x OR greater than y\n" <<
			"\t@x:y\talert if greater than or equal to x AND less than or equal to y\n" << std::endl;
}

void version(char* argv0)
{
	std::cout << argv0 << " v" << VERSION << std::endl;
}
