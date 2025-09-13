#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <globals.h>
#define WM_TRAYICON (WM_USER + 1)
#define NOTIFY_TIMEOUT 5000

typedef struct
{
    HWND hWnd;
    wchar_t title[64];
    wchar_t msg[256];
    UINT_PTR timerId;
    NOTIFYICONDATA nid;
} NotificationData;

void ShowTrayNotification(HWND hWnd, const wchar_t *title, const wchar_t *msg);
#endif