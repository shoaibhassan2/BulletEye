

#include <requests.h>

char* GetRequest(const char* url) {
    HINTERNET hInternet = InternetOpenA("WinINet JSON", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) return NULL;

    // Set timeouts (in milliseconds)
    DWORD timeout = 5000; // 5 seconds
    InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
    InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    HINTERNET hFile = InternetOpenUrlA(hInternet, url, NULL, 0,
                                       INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (!hFile) {
        InternetCloseHandle(hInternet);
        return NULL;
    }

    char buffer[4096];
    DWORD bytesRead;
    size_t totalSize = 0;
    char* result = NULL;

    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        char* newResult = realloc(result, totalSize + bytesRead + 1);
        if (!newResult) {
            free(result);
            result = NULL;
            break;
        }
        result = newResult;
        memcpy(result + totalSize, buffer, bytesRead);
        totalSize += bytesRead;
    }

    if (result)
        result[totalSize] = '\0';

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);
    return result;
}
