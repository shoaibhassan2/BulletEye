
#include <globals.h>
#include <windows.h>
#include <commctrl.h>
#include <ole2.h>
#include <tchar.h>
#include <gui/settings.h>
#include <gui/main_proc.h>

#include <resource.h>
#include <gui/registry_settings.h>
#include <gui/tooltip.h>
#include <gui/monitor.h>
#include <DarkMode.h>
#include <server/server_logic.h>
#include <stdio.h>
#include <crash_handler/crash_handler.h>

HINSTANCE g_hinst;
HWND g_hWnd;
HFONT g_hFont;
HWND hwndTab;
HWND g_hTooltip;
WCHAR MAIN_WINDOW_TITLE[128];
HWND hStatusBar;

ServerPageControls g_server_controls;
ClientPageControls g_client_controls;
LogPageControls g_log_controls;

NetworkSettings g_networkSettings = {256, 60, 8, 8, FALSE, 0, 0};

int g_serverContextMenuIndex = -1;
int g_clientContextMenuIndex = -1;

ActiveClient g_active_clients[MAX_CLIENTS];
int g_active_client_count = 0;

BOOL g_bLogAutoScroll = TRUE;
BOOL g_bIsRunning = FALSE;
BOOL isDarkMode = FALSE;
CRITICAL_SECTION g_cs_servers;
CRITICAL_SECTION g_cs_clients;

static int isDarkModeEnabled()
{
    HKEY hKey;
    DWORD value = 1;
    DWORD size = sizeof(value);

    if (RegOpenKeyExA(
            HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {

        RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size);
        RegCloseKey(hKey);
    }

    return value == 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

    SetUnhandledExceptionFilter(CrashHandler);

    isDarkMode = isDarkModeEnabled();

    if (!DarkMode_LoadLibrary(L"darkmode.dll"))
    {

        MessageBoxW(NULL, L"Failed to load darkmode.dll!", L"Error", MB_ICONERROR);
        return 1;
    }

    if (isDarkMode)
    {
        DarkMode_initDarkMode_NoIni();
        DarkMode_setDarkModeConfig((UINT)DarkMode_DarkModeType_dark);
        DarkMode_setDefaultColors(TRUE);
    }

    WCHAR MAIN_CLASS_NAME[128] = {0};
    memset(MAIN_WINDOW_TITLE, 0, 128);

    LoadStringW(hInstance, IDS_MAIN_CLASS_NAME, MAIN_CLASS_NAME, _countof(MAIN_CLASS_NAME));
    LoadStringW(hInstance, IDS_MAIN_APP_TITLE, MAIN_WINDOW_TITLE, _countof(MAIN_WINDOW_TITLE));

    g_hinst = hInstance;
    OleInitialize(NULL);

    InitializeCriticalSection(&g_cs_servers);
    InitializeCriticalSection(&g_cs_clients);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        g_active_clients[i].socket = INVALID_SOCKET;
        g_active_clients[i].hThread = NULL;
        g_active_clients[i].hExplorerWnd = NULL;
        g_active_clients[i].isActive = FALSE;
        InitializeCriticalSection(&g_active_clients[i].cs_socket_access);
    }
    g_active_client_count = 0;

    INITCOMMONCONTROLSEX icex;
    ZeroMemory(&icex, sizeof(icex));
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_USEREX_CLASSES | ICC_INTERNET_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = MAIN_CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));

    if (!RegisterClassExW(&wc))
    {
        WCHAR msg[256], caption[64];
        LoadStringW(NULL, IDS_ERR_REGISTER_FAIL_MAIN, msg, _countof(msg));
        LoadStringW(NULL, IDS_ERR_CAPTION, caption, _countof(caption));
        MessageBoxW(NULL, msg, caption, MB_OK | MB_ICONERROR);
        OleUninitialize();
        return 0;
    }

    g_hWnd = CreateWindowExW(
        WS_EX_CLIENTEDGE | WS_EX_LAYERED, MAIN_CLASS_NAME, MAIN_WINDOW_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 520, 600,
        NULL, NULL, hInstance, NULL);

    if (!g_hWnd)
    {
        WCHAR msg[256], caption[64];
        LoadStringW(NULL, IDS_ERR_CREATE_FAIL_MAIN, msg, _countof(msg));
        LoadStringW(NULL, IDS_ERR_CAPTION, caption, _countof(caption));
        MessageBoxW(NULL, msg, caption, MB_OK | MB_ICONERROR);
        OleUninitialize();
        return 0;
    }

    WINDOWPLACEMENT wp = {0};
    wp.length = sizeof(WINDOWPLACEMENT);
    if (LoadWindowSettings(&wp))
    {
        wp.showCmd = (wp.showCmd == SW_SHOWMINIMIZED) ? SW_SHOWNORMAL : wp.showCmd;
        SetWindowPlacement(g_hWnd, &wp);
    }
    else
    {
        ShowWindow(g_hWnd, nCmdShow);
    }

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    LoadServerSettingsFromRegistry();

    HANDLE hThread = CreateThread(NULL, 0, UpdateTitleThread, NULL, 0, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (hThread)
    {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        DeleteCriticalSection(&g_active_clients[i].cs_socket_access);
    }

    DeleteCriticalSection(&g_cs_servers);
    DeleteCriticalSection(&g_cs_clients);

    OleUninitialize();
    DarkMode_FreeLibrary();
    return (int)msg.wParam;
}