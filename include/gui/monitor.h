#ifndef MONITOR_H
#define MONITOR_H

#include <globals.h>

ULONGLONG FileTimeToUlong(const FILETIME ft);
DWORD WINAPI UpdateTitleThread(LPVOID lpParam);

#endif