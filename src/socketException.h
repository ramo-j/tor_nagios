/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef SOCKETEXCEPTION_H
#define SOCKETEXCEPTION_H

#include <string>
#include "exception.h"

#define THROWSOCKETEXCEPTION(message) throw SOCKETEXCEPTION(message, __FILE__, __FUNCTION__, __LINE__)

class SOCKETEXCEPTION : public EXCEPTION
{
public:
	SOCKETEXCEPTION(const std::string m, const std::string f, const std::string func, const int l) { message = m; file = f; function = func; line = l; }
};

#endif
