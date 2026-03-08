#ifndef PLATFORM___PLATFORM__H
#define PLATFORM___PLATFORM__H

// <https://www.geeksforgeeks.org/operating-systems/how-to-detect-operating-system-through-a-c-program/>
#ifdef __linux__
//#	warning	"Using `Linux OS` Options"
#	include "linux.h"

#elif __APPLE__
#	error	"The platform `Mac OS` is currently not supported!"

#elif _WIN32
#	error	"The platform `Windows OS` is currently not supported!"

#elif TARGET_OS_EMBEDDED
#	error	"The platform `iOS embedded OS` is currently not supported!"

#elif TARGET_IPHONE_SIMULATOR
#	error	"The platform `iOS simulator OS` is currently not supported!"

#elif TARGET_OS_IPHONE
#	error	"The platform `iPhone OS` is currently not supported!"

#elif TARGET_OS_MAC
#	error	"The platform `MAC OS` is currently not supported!"

#elif__ANDROID__
#	error	"The platform `android OS` is currently not supported!"

#elif __unix__
#	error	"The platform `unix` is currently not supported!"

#elif _POSIX_VERSION
#	error	"The platform `POSIX based OS` is currently not supported!"

#elif __sun
#	error	"The platform `Solaris` is currently not supported!"

#elif __hpux
#	error	"The platform `HP UX` is currently not supported!"

#elif BSD
#	error	"The platform `Solaris` is currently not supported!"

#elif __DragonFly__
#	error	"The platform `DragonFly BSD` is currently not supported!"

#elif __FreeBSD__
#	error	"The platform `FreeBSD` is currently not supported!"

#elif __NetBSD__
#	error	"The platform `Net BSD` is currently not supported!"

#elif __OpenBSD__
#	error	"The platform `Open BSD` is currently not supported!"

#else
#	error	"Unknown system in use"

#endif

#endif
