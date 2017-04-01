#pragma once
#include <cef/include/base/cef_build.h>
#include <cstdint>

#if defined(OS_WIN)
#include <windef.h>
#else
#error No implementaiton for this OS.
#endif


namespace ProjectC {
	namespace Interface {
#if defined(OS_WIN)
		typedef HWND WindowHandle;
#else
#error No implementaiton for this OS.
#endif
	}
}