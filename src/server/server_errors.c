#include <globals.h>
#include <windows.h>
#include <stdio.h>
#include <server/server_errors.h>

void ShowNetworkError(const WCHAR *title, int errorCode)
{
    WCHAR *s = NULL;
    WCHAR buffer[256];
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&s, 0, NULL);
    wsprintfW(buffer, L"%s\nError Code: %d\n%s", title, errorCode, s);
    MessageBoxW(g_hWnd, buffer, L"Network Error", MB_OK | MB_ICONERROR);
    LocalFree(s);
}