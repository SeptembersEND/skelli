#ifndef PLATFORM___LINUX__H
#define PLATFORM___LINUX__H

#undef CHECK
#undef DOWNLOAD
#undef EXEC
#undef EXISTS

#define CC "/usr/bin/env", "cc"
#define CMD_DOWNLOAD "curl", "-o"

#ifndef __NO_ANSI_ESCAPE
#define AE_BLK "\x1b[0;30m"
#define AE_RED "\x1b[0;31m"
#define AE_GRN "\x1b[0;32m"
#define AE_YEL "\x1b[0;33m"
#define AE_BLU "\x1b[0;34m"
#define AE_MAG "\x1b[0;35m"
#define AE_CYN "\x1b[0;36m"
#define AE_WHT "\x1b[0;37m"
#define AE_RES "\x1b[0;0m"

#else
#define AE_BLK
#define AE_RED
#define AE_GRN
#define AE_YEL
#define AE_BLU
#define AE_MAG
#define AE_CYN
#define AE_WHT
#define AE_RES
#endif

#define _ERROR_MSG AE_RED "Error" AE_RES

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "linux.c"

#endif
