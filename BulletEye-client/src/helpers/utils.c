// src/helpers/utils.c
#include <helpers/utils.h>
#include <stdlib.h>

char* ConvertWCHARToCHAR(const WCHAR *wstr) {
    if (!wstr) return NULL;
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return NULL;
    char* out = (char*)malloc(len);
    if(out) WideCharToMultiByte(CP_UTF8, 0, wstr, -1, out, len, NULL, NULL);
    return out;
}

WCHAR* ConvertCHARToWCHAR(const char *cstr) {
    if (!cstr) return NULL;
    int len = MultiByteToWideChar(CP_UTF8, 0, cstr, -1, NULL, 0);
    if (len <= 0) return NULL;
    WCHAR* out = (WCHAR*)malloc(len * sizeof(WCHAR));
    if(out) MultiByteToWideChar(CP_UTF8, 0, cstr, -1, out, len);
    return out;
}