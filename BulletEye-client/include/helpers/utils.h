// include/helpers/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

// Converts a WCHAR string (UTF-16) to a CHAR string (UTF-8)
char* ConvertWCHARToCHAR(const WCHAR *wstr);

// Converts a CHAR string (UTF-8) to a WCHAR string (UTF-16)
WCHAR* ConvertCHARToWCHAR(const char *cstr);

#endif // UTILS_H