/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include <string>

class EXCEPTION
{
public:
	std::string getMessage() const { return message; }
	std::string getFile() const { return file; }
	std::string getFunction() const { return function; }
	long getLine() const { return line; }
protected:
	std::string message;
	std::string file;
	std::string function;
	long line;
};

#endif	/* EXCEPTION_H */

