#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([tor_nagios], [0.01], [ramo@goodvikings.com])
AM_INIT_AUTOMAKE(tor_nagios, 0.01)
AC_CONFIG_SRCDIR([src/prog.cpp])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([netdb.h netinet/in.h stdlib.h string.h sys/socket.h unistd.h curl/curl.h])

# Checks for library functions.
AC_CHECK_FUNCS([bzero gethostbyname memset socket curl_easy_init])

AC_OUTPUT(Makefile src/Makefile)
