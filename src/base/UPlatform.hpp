#pragma once

namespace UPlatform
{
#if defined(_WIN32)
#include "UPlatformWindows.h"
	const UPlatformWindows::TPlatformWindows
#elif defined(__APPLE__)
#include "UPlatformMacOSX.h"
	const UPlatformMacOSX::TPlatformMacOSX
#elif defined(__unix__)
#include "UPlatformLinux.h"
	const UPlatformLinux::TPlatformLinux
#endif
		Platform;
}