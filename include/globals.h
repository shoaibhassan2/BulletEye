
#ifndef GLOBALS_H
#define GLOBALS_H
#include <winsock2.h>
#include <windows.h>
#include <server/server_logic.h>

#define WM_APP_UPDATE_STATUS (WM_APP + 1)
#define WM_APP_ADD_LOG (WM_APP + 2)
#define WM_APP_ADD_CLIENT (WM_APP + 3)
#define WM_APP_UPDATE_PROGRESS (WM_APP + 4)
#define WM_APP_REMOVE_CLIENT (WM_APP + 5)

typedef struct
{
    HWND hwndToolbar;
    HWND hwndGroupControls;
    HWND hwndStaticIP;
    HWND hwndIpAddress;
    HWND hwndStaticPort;
    HWND hwndEditPort;
    HWND hwndButtonAdd;
    HWND hwndGroupStats;
    HWND hwndStaticClients;
    HWND hwndStaticClientsVal;

    HWND hwndStaticReceived;
    HWND hwndStaticReceivedVal;

    HWND hwndStaticSent;
    HWND hwndStaticSentVal;
    HWND hwndListServers;
} ServerPageControls;

typedef struct
{
    HWND hwndListClients;
} ClientPageControls;

typedef struct
{
    HWND hwndListLog;
    HWND hwndButtonClear;
    HWND hwndCheckAutoscroll;
    HWND hwndButtonSave;
    HWND hwndButtonCopy;
} LogPageControls;

typedef struct
{
    int max_connections;
    int connection_timeout;
    int send_buffer_kb;
    int recv_buffer_kb;
    BOOL limit_bandwidth;
    int max_upload_kbps;
    int max_download_kbps;
} NetworkSettings;

extern HFONT g_hFont;
extern HINSTANCE g_hinst;
extern HWND g_hWnd;
extern HWND hwndTab;
extern HWND g_hTooltip;
extern BOOL g_bIsRunning;
extern WCHAR MAIN_WINDOW_TITLE[128];

extern ServerPageControls g_server_controls;
extern ClientPageControls g_client_controls;
extern LogPageControls g_log_controls;

extern NetworkSettings g_networkSettings;

extern int g_serverContextMenuIndex;
extern int g_clientContextMenuIndex;

extern ActiveClient g_active_clients[MAX_CLIENTS];
extern int g_active_client_count;

extern BOOL g_bLogAutoScroll;
extern BOOL isDarkMode;
extern CRITICAL_SECTION g_cs_servers;
extern CRITICAL_SECTION g_cs_clients;

#endif