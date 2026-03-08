#ifndef PLATFORM___LINUX__H
#define PLATFORM___LINUX__H

#undef	EXEC

#define CC "/usr/bin/env", "cc"
#define CMD_DOWNLOAD "curl", "-o"

#include "linux.c"

#endif
