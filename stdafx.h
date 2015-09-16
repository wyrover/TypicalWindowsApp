// We must do it strictly!!!
#define STRICT 1

// the target versioning header
#include "targetver.h"

#ifdef _MSC_VER
    // for detecting memory leak (MSVC only)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

// Windows headers
#include <windows.h>

// C runtime headers
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <tchar.h>

// C++ headers
#include <string>
#include <vector>
#include <new>

// private headers
#include "TypicalWindowsApp.h"

#ifdef _MSC_VER
    // for detecting memory leak (MSVC only)
    #define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
