#include <helpers/utils.h>
#include <windows.h>
#include <stdlib.h>
#include <commctrl.h>

char *ConvertWCHARToCHAR(LPCWSTR wideStr)
{
    if (!wideStr)
        return NULL;
    int len = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    if (len <= 0)
        return NULL;
    char *out = malloc(len);
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, out, len, NULL, NULL);
    return out;
}

wchar_t *ConvertCHARToWCHAR(const char *utf8Str)
{
    if (!utf8Str)
        return NULL;
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    wchar_t *w = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, w, len);
    return w;
}