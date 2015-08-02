#ifndef TYPICALWINDOWSAPP_H
#define TYPICALWINDOWSAPP_H

// resource IDs
#include "resource.h"

// tstring
#ifdef UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif

// NOTE: Digital Mars C/C++ Compiler doesn't define INT_PTR type likely.
#ifdef __DMC__
    #define INT_PTR BOOL
#endif

#endif  // ndef TYPICALWINDOWSAPP_H
