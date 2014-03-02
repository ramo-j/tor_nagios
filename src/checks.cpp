/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#include <string>
#include <cstring>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "checks.h"
#include "consts.h"
#include "socketException.h"
#include "csocket.h"
#include "checkException.h"
#include "misc.h"

int statusCheck(const double val, const std::string warn, const std::string crit);
bool alertValue(const double val, const std::string range);
void buildMessage(const char* result, const char* label, const char* uom, const int status, const char* warning, const char* critical, std::string* message);
std::string controlPortRequest(const char* hostname, const int port, const char* authPass, const char* requestStr, const bool verbose);
std::string getSocksAddress(const char* hostname, const int port, const char* authPass, const bool verbose);
std::string getHibernationState(const char* hostname, const int port, const char* authPass, const bool verbose);
double curlFetch(const char* socksAddr, const int timeout, const bool verbose);
double getBytesRead(const char* hostname, const int port, const char* authPass, const bool verbose);
double getBytesWritten(const char* hostname, const int port, const char* authPass, const bool verbose);

/**
 * Performs a check of a page fetch through the socks proxy.
 * @param hostname The location of the tot server to check
 * @param port The control port of the Tor server
 * @param authPass The authentication password of the tor control port
 * @param warning The warning threshold
 * @param critical The critical threshold
 * @param timeout The timeout value
 * @param verbose Print detailed log to STDOUT
 * @param message The result message
 * @return RET_OK, RET_WARN, RET_CRIT or RET_UNKNOWN depending on the result of the check
 */
int check_socksFetch(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const int timeout, const bool verbose, std::string* message)
{
	VERBOSE("Entering check_socksFetch");
	try
	{
		*message = "SOCKSFETCH ";

		// first we need to get the socks port from tor
		std::string socksAddr = getSocksAddress(hostname, port, authPass, verbose);
		VERBOSE("Retrieved socks address");
		VERBOSE(socksAddr);

		// then we can perform a curl that socks port to fetch a page (any page will do)
		double time = curlFetch(socksAddr.c_str(), timeout, verbose);
		int status = statusCheck(time, warning, critical);

		char* str = new char[64];
		sprintf(str, "%f", time);

		buildMessage(str, "time", "s", status, warning, critical, message);

		delete [] str;
		return status;
	}
	catch (EXCEPTION e)
	{
		*message += "CRITICAL " + e.getMessage();
		return RET_UNKNOWN;
	}
}

/**
 * Checks the hibernation state of the tor server
 * @param hostname The hostname of the tor server
 * @param port The control port
 * @param authPass The authentication passwd on the conrtol port
 * @param warning The warning threshold
 * @param critical The critical threshold
 * @param verbose Print detailed log to STDOUT
 * @param message the reslt message
 * @return RET_OK, RET_WARN, RET_CRIT or RET_UNKNOWN depending on the result of the check
 */
int check_hibernationState(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const bool verbose, std::string* message)
{
	VERBOSE("Entering check_hibernationState");
	
	*message = "HIBERNATIONSTATE ";

	try
	{
		//Ask the control port if we are hibernating.
		std::string state = getHibernationState(hostname, port, authPass, verbose);
		int status = statusCheck(state == "awak" ? 0 : state == "soft" ? 1 : /* hard */ 2, warning, critical);

		char* str = new char[64];
		sprintf(str, "%d", status);

		buildMessage(str, "state", "", status, warning, critical, message);

		delete [] str;
		return status;
	}
	catch (EXCEPTION e)
	{
		*message += "CRITICAL " + e.getMessage();
		return RET_UNKNOWN;
	}
}

/**
 * Checks the bandwidth of the tor server
 * @param hostname The hostname of the tor server
 * @param port The control port
 * @param authPass The authentication passwd on the conrtol port
 * @param warning The warning threshold
 * @param critical The critical threshold
 * @param verbose Print detailed log to STDOUT
 * @param message the reslt message
 * @return RET_OK, RET_WARN, RET_CRIT or RET_UNKNOWN depending on the result of the check
 */
int check_bandwidth(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const bool verbose, std::string* message)
{
	VERBOSE("Entering check_bandwidth");
	
	*message = "BANDWIDTH ";

	try
	{
		// Ask the control port for how many bytes read and written
		VERBOSE("Getting initial values");
		double init = getBytesRead(hostname, port, authPass, verbose) + getBytesWritten(hostname, port, authPass, verbose);
		// wait and ask again
		VERBOSE("Pausing...");
		sleep(SLEEPDURATION);
		VERBOSE("Collecting updated values");
		double bw = (getBytesRead(hostname, port, authPass, verbose) + getBytesWritten(hostname, port, authPass, verbose) - init) / SLEEPDURATION;
		
		int status = statusCheck(bw, warning, critical);
		char* str = new char[64];
		sprintf(str, "%d", (int) bw);

		buildMessage(str, "bandwidth", "B", status, warning, critical, message);

		delete [] str;
		return status;
	}
	catch (EXCEPTION e)
	{
		*message += "CRITICAL " + e.getMessage();
		return RET_UNKNOWN;
	}
}

// <editor-fold defaultstate="collapsed" desc="Local functions">
// <editor-fold defaultstate="collapsed" desc="Functions common to multiple checks">

/**
 * Makes a command request on the tor control port and returns the output
 * @param hostname The hostname to make the request to
 * @param port The control port
 * @param authPass The authentication passwd
 * @param requestStr The request to make
 * @return The command result
 */
std::string controlPortRequest(const char* hostname, const int port, const char* authPass, const char* requestStr, const bool verbose)
{
	VERBOSE("Entering controlPortRequest");

	csocket *cs = new csocket;
	char* buffer = new char[SOCKETBUFFERLEN];
	memset(buffer, 0, SOCKETBUFFERLEN);

	cs->Configure(hostname, (unsigned short) port);
	cs->Connect();

	VERBOSE("Connected to control port, making request");

	strncat(buffer, "authenticate \"", SOCKETBUFFERLEN);
	strncat(buffer, authPass, SOCKETBUFFERLEN);
	strncat(buffer, "\"\n", SOCKETBUFFERLEN);

	cs->Send((const unsigned char*) buffer, strnlen(buffer, SOCKETBUFFERLEN)); // send authentication
	cs->Read((unsigned char*) buffer); // 250 OK

	if (strncmp(buffer, "250 OK\r\n", 8) != 0)
	{
		std::string message = "Connection to Tor's control port failed: ";
		THROWCHECKEXCEPTION(message + buffer);
	}

	cs->Send((const unsigned char*) requestStr, strnlen(requestStr, 64));
	cs->Read((unsigned char*) buffer); // 250-net/listeners/socks="127.0.0.1:9050"\n250 OK
	cs->Send((const unsigned char*) "quit\n", strnlen("quit\n", 32));

	VERBOSE("Control port request complete");

	std::string retVal = buffer;

	delete cs;
	delete [] buffer;

	return retVal;
}

int statusCheck(const double val, const std::string warn, const std::string crit)
{
	if (alertValue(val, crit))
	{
		return RET_CRIT;
	}
	if (alertValue(val, warn))
	{
		return RET_WARN;
	}
	return RET_OK;
}

bool alertValue(const double val, const std::string range)
{
	/*
	 * Range can be:
	 * x		alert if less than 0 or greater than x
	 * x:		alert if less than x
	 * ~:x		alert if greater than x
	 * x:y		alert if less than x OR greater than y
	 * @x:y		alert if greater than or equal to x AND less than or equal to y
	 */

	unsigned long loc = 0;
	double x = 0, y = 0;
	std::istringstream iss;

	// @x:y
	if (range[0] == '@')
	{
		loc = range.find(':', 0);
		iss.str(range.substr(1, loc));
		iss >> x;
		iss.clear();
		iss.str(range.substr(loc + 1, range.size()));
		iss >> y;

		return (val >= x && val <= y) ? true : false;
	}

	// ~:x
	if (range[0] == '~')
	{
		iss.str(range.substr(2, range.size()));
		iss >> x;

		return (val > x) ? true : false;
	}

	// x:
	if (range[range.size() - 1] == ':')
	{
		iss.str(range.substr(0, range.size() - 1));
		iss >> x;

		return (val < x) ? true : false;
	}

	// x:y
	if (range.find(':', 0) != std::string::npos)
	{
		loc = range.find(':', 0);
		iss.str(range.substr(0, loc));
		iss >> x;
		iss.clear();
		iss.str(range.substr(loc + 1, range.size()));
		iss >> y;

		return (val < x || val > y) ? true : false;
	}

	// only x left
	iss.str(range);
	iss >> x;
	return (val < 0 || val >= x) ? true : false;
}

void buildMessage(const char* result, const char* label, const char* uom, const int status, const char* warning, const char* critical, std::string* message)
{
	char* str = new char[64];

	*message += (status == RET_OK ? "OK" : status == RET_WARN ? "WARNING" : "CRITICAL");
	*message += " | ";
	*message += label;
	*message += "=";
	*message += result;
	*message += uom;
	*message += ";";
	*message += warning;
	*message += ";";
	*message += critical;
	*message += ";0;";

	delete [] str;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="socks fetch internal functions">

/**
 * Connects to the control port of the tor server, asks for the socks address
 * @param hostname The location of the tor server
 * @param port The control port
 * @param authPass The authentication password
 * @return The socks listening address
 */
std::string getSocksAddress(const char* hostname, const int port, const char* authPass, const bool verbose)
{
	std::string retVal = controlPortRequest(hostname, port, authPass, "GETINFO net/listeners/socks\n", verbose);
	const char prefix[] = "250-net/listeners/socks=\"";
	const unsigned long prefixLen = strnlen(prefix, 64);

	if (retVal.substr(0, prefixLen).compare(prefix) != 0)
	{
		std::string message = "Fetching socks address failed: ";
		THROWCHECKEXCEPTION(message + retVal);
	}

	return retVal.substr(prefixLen, retVal.find_last_of('"') - prefixLen);
}

/**
 * Fetchs a page ("https://check.torproject.org/") through the socks address passed.
 * @param socksAddr The socks address to use
 * @param timeout Timeout value
 * @return The time taken to fetch the page
 */
double curlFetch(const char* socksAddr, const int timeout, const bool verbose)
{
	VERBOSE("Entering curlFetch");
	CURL *curl;
	CURLcode res;
	double time = 0;

	curl = curl_easy_init();
	if (curl)
	{
		FILE* file = fopen("/dev/null", "w");

		curl_easy_setopt(curl, CURLOPT_URL, "https://check.torproject.org/");
		curl_easy_setopt(curl, CURLOPT_PROXY, socksAddr);
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

		VERBOSE("Making the page fetch through curl");

		res = curl_easy_perform(curl);

		VERBOSE("Page retrieved");

		if (res != CURLE_OK)
		{
			std::string message = "Failed to fetch page: ";
			message += curl_easy_strerror(res);
			THROWCHECKEXCEPTION(message);
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &time);
		}

		curl_easy_cleanup(curl);
		curl_global_cleanup();
		fclose(file);
	}
	else
	{
		THROWCHECKEXCEPTION("Memory allocation failed");
	}

	return time;
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Hibernation state internal functions">

std::string getHibernationState(const char* hostname, const int port, const char* authPass, const bool verbose)
{
	std::string retVal = controlPortRequest(hostname, port, authPass, "GETINFO accounting/hibernating\n", verbose);
	const char prefix[] = "250-accounting/hibernating=";
	const unsigned long prefixLen = strnlen(prefix, 64);

	if (retVal.substr(0, prefixLen).compare(prefix) != 0)
	{
		std::string message = "Fetching hibernation state failed: ";
		THROWCHECKEXCEPTION(message + retVal);
	}

	return retVal.substr(prefixLen, 4);
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Bandwidth internal functions">

double getBytesRead(const char* hostname, const int port, const char* authPass, const bool verbose)
{
	std::string retVal = controlPortRequest(hostname, port, authPass, "getinfo traffic/read\n", verbose);
	const char prefix[] = "250-traffic/read=";
	const unsigned long prefixLen = strnlen(prefix, 64);

	if (retVal.substr(0, prefixLen).compare(prefix) != 0)
	{
		std::string message = "Fetching bytes read failed: ";
		THROWCHECKEXCEPTION(message + retVal);
	}

	double val;
	std::istringstream iss;
	iss.str(retVal.substr(prefixLen, retVal.find('\r') - prefixLen));
	iss >> val;

	return val;
}

double getBytesWritten(const char* hostname, const int port, const char* authPass, const bool verbose)
{
	std::string retVal = controlPortRequest(hostname, port, authPass, "getinfo traffic/written\n", verbose);
	const char prefix[] = "250-traffic/written=";
	const unsigned long prefixLen = strnlen(prefix, 64);

	if (retVal.substr(0, prefixLen).compare(prefix) != 0)
	{
		std::string message = "Fetching bytes read failed: ";
		THROWCHECKEXCEPTION(message + retVal);
	}

	double val;
	std::istringstream iss;
	iss.str(retVal.substr(prefixLen, retVal.find('\r') - prefixLen));
	iss >> val;

	return val;
}
// </editor-fold>
// </editor-fold>
