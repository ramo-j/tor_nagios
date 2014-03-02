/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef CHECKS_H
#define	CHECKS_H

int check_socksFetch(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const int timeout, const bool verbose, std::string* message);
int check_hibernationState(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const bool verbose, std::string* message);
int check_bandwidth(const char* hostname, const int port, const char* authPass, const char* warning, const char* critical, const bool verbose, std::string* message);

#endif	/* CHECKS_H */

