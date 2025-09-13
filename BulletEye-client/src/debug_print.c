#include <debug_print.h>
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
void DEBUG_PRINT(const char *fmt, ...)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    // Print timestamp and debug prefix
    printf("[%02d:%02d:%02d] [DEBUG] ", st.wHour, st.wMinute, st.wSecond);

    // Handle variadic args
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}