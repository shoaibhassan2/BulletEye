#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H
#include <windows.h>

// Install the crash handler globally
void InstallCrashHandler(void);

// Crash handler itself (used internally)
LONG WINAPI CrashHandler(EXCEPTION_POINTERS *ep);

#endif // CRASH_HANDLER_H
