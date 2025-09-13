#ifndef LOG_PAGE_H
#define LOG_PAGE_H

#include <globals.h>
#include <stdarg.h>

void CreateLogPage(HWND parent);
void AddLogMessage(const WCHAR *fmt, ...);
void PostLogMessage(const WCHAR *fmt, ...);
#endif