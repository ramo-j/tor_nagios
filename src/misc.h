/*
 * ------------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ramo@goodvikings.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return - Ramo
 * ------------------------------------------------------------------------------
 */

#ifndef MISC_H
#define	MISC_H

#define VERSION 0.001

#define VERBOSE(message) std::cout << (verbose ? message : "") << (verbose ? "\n" : "")

void usage(char* argv0);
void version(char* argv0);

#endif	/* MISC_H */

