/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef CHECKEXCEPTION_H
#define CHECKEXCEPTION_H

#include <string>
#include "exception.h"

#define THROWCHECKEXCEPTION(message) throw CHECKEXCEPTION(message, __FILE__, __FUNCTION__, __LINE__)

class CHECKEXCEPTION : public EXCEPTION
{
public:
	CHECKEXCEPTION(const std::string m, const std::string f, const std::string func, const int l) { message = m; file = f; function = func; line = l; }
};

#endif
