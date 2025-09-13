#include <service/notification.h>
#include <windows.h>
#include <shellapi.h>
#include <resource.h>
#include <stdio.h>

VOID CALLBACK NotificationTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    NotificationData *data = (NotificationData *)idEvent;
    if (!data)
        return;

    Shell_NotifyIcon(NIM_DELETE, &data->nid);
    KillTimer(hwnd, data->timerId);
    free(data);
}

DWORD WINAPI NotificationThread(LPVOID param)
{
    NotificationData *data = (NotificationData *)param;
    if (!data)
        return 0;

    ZeroMemory(&data->nid, sizeof(NOTIFYICONDATA));
    data->nid.cbSize = sizeof(NOTIFYICONDATA);
    data->nid.hWnd = data->hWnd;
    data->nid.uID = (UINT_PTR)data;
    data->nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    data->nid.uCallbackMessage = WM_TRAYICON;

    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(data->hWnd, GWLP_HINSTANCE);
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_MAIN));
    if (!hIcon)
        hIcon = LoadIcon(NULL, IDI_INFORMATION);
    data->nid.hIcon = hIcon;

#ifndef __TINYC__
    wcsncpy_s(data->nid.szTip, sizeof(data->nid.szTip) / sizeof(data->nid.szTip[0]), L"My App", _TRUNCATE);
    wcsncpy_s(data->nid.szInfoTitle, sizeof(data->nid.szInfoTitle) / sizeof(data->nid.szInfoTitle[0]), data->title, _TRUNCATE);
    wcsncpy_s(data->nid.szInfo, sizeof(data->nid.szInfo) / sizeof(data->nid.szInfo[0]), data->msg, _TRUNCATE);
#else
    snwprintf(data->nid.szTip, sizeof(data->nid.szTip) / sizeof(data->nid.szTip[0]), L"%ls", L"My App");
    snwprintf(data->nid.szInfoTitle, sizeof(data->nid.szInfoTitle) / sizeof(data->nid.szInfoTitle[0]), L"%ls", data->title);
    snwprintf(data->nid.szInfo, sizeof(data->nid.szInfo) / sizeof(data->nid.szInfo[0]), L"%ls", data->msg);
#endif

    data->nid.dwInfoFlags = NIIF_INFO;

    Shell_NotifyIcon(NIM_ADD, &data->nid);
    Shell_NotifyIcon(NIM_MODIFY, &data->nid);

    data->timerId = SetTimer(data->hWnd, (UINT_PTR)data, NOTIFY_TIMEOUT, NotificationTimerProc);

    return 0;
}

void ShowTrayNotification(HWND hWnd, const wchar_t *title, const wchar_t *msg)
{
    if (!hWnd || !title || !msg)
        return;

    NotificationData *data = (NotificationData *)malloc(sizeof(NotificationData));
    if (!data)
        return;

    data->hWnd = hWnd;
#ifndef __TINYC__
    wcsncpy_s(data->title, 64, title, _TRUNCATE);
    wcsncpy_s(data->msg, 256, msg, _TRUNCATE);
#else
    snwprintf(data->title, 64, L"%ls", title);
    snwprintf(data->msg, 256, L"%ls", msg);
#endif

    HANDLE hThread = CreateThread(NULL, 0, NotificationThread, data, 0, NULL);
    if (hThread)
        CloseHandle(hThread);
    else
        free(data);
}
