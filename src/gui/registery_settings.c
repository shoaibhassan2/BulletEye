#include <gui/registry_settings.h>
#include <globals.h>
#include <resource.h>
#include <gui/log_page.h>
#include <server/server_logic.h>
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>

#ifdef __TINYC__
LONG WINAPI RegDeleteTreeW(HKEY hKey, LPCWSTR lpSubKey);
#endif

static void GetServersRegPath(WCHAR *pathBuffer, DWORD bufferSize)
{
    WCHAR basePath[256];
    WCHAR serversSubKey[128];

    LoadStringW(g_hinst, IDS_REG_KEY_PATH, basePath, _countof(basePath));
    LoadStringW(g_hinst, IDS_REG_SUBKEY_SERVERS, serversSubKey, _countof(serversSubKey));

    swprintf_s(pathBuffer, bufferSize, L"%s\\%s", basePath, serversSubKey);
}

void SaveServerSettingsToRegistry()
{
    HKEY hKey;
    WCHAR szServersPath[MAX_PATH];

    GetServersRegPath(szServersPath, _countof(szServersPath));

    RegDeleteTreeW(HKEY_CURRENT_USER, szServersPath);

    EnterCriticalSection(&g_cs_servers);

    if (g_server_count == 0)
    {
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    if (RegCreateKeyExW(HKEY_CURRENT_USER, szServersPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
    {
        PostLogMessage(L"Failed to create registry key for saving server settings.");
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    WCHAR szIpValueName[64], szPortValueName[64];
    LoadStringW(g_hinst, IDS_REG_VALUE_IP, szIpValueName, _countof(szIpValueName));
    LoadStringW(g_hinst, IDS_REG_VALUE_PORT, szPortValueName, _countof(szPortValueName));

    for (int i = 0; i < g_server_count; ++i)
    {
        WCHAR szSubKeyName[32];
        HKEY hSubKey;
        swprintf_s(szSubKeyName, _countof(szSubKeyName), L"Server%d", i);

        if (RegCreateKeyExW(hKey, szSubKeyName, 0, NULL, 0, KEY_WRITE, NULL, &hSubKey, NULL) == ERROR_SUCCESS)
        {

            RegSetValueExW(hSubKey, szIpValueName, 0, REG_SZ,
                           (const BYTE *)g_servers[i].ipAddress,
                           (DWORD)(wcslen(g_servers[i].ipAddress) + 1) * sizeof(WCHAR));

            RegSetValueExW(hSubKey, szPortValueName, 0, REG_DWORD,
                           (const BYTE *)&g_servers[i].port,
                           sizeof(g_servers[i].port));

            RegCloseKey(hSubKey);
        }
    }

    RegCloseKey(hKey);
    LeaveCriticalSection(&g_cs_servers);

    WCHAR logMsg[256];
    LoadStringW(g_hinst, IDS_LOG_SERVERS_SAVED, logMsg, _countof(logMsg));
    PostLogMessage(logMsg);
}

void LoadServerSettingsFromRegistry()
{
    HKEY hKey;
    WCHAR szServersPath[MAX_PATH];

    GetServersRegPath(szServersPath, _countof(szServersPath));

    if (RegOpenKeyExW(HKEY_CURRENT_USER, szServersPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        WCHAR logMsg[256];
        LoadStringW(g_hinst, IDS_LOG_NO_SERVERS_FOUND, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);
        return;
    }

    WCHAR szIpValueName[64], szPortValueName[64];
    LoadStringW(g_hinst, IDS_REG_VALUE_IP, szIpValueName, _countof(szIpValueName));
    LoadStringW(g_hinst, IDS_REG_VALUE_PORT, szPortValueName, _countof(szPortValueName));

    WCHAR szSubKeyName[256];
    DWORD dwIndex = 0;
    int loadedCount = 0;

    EnterCriticalSection(&g_cs_servers);

    while (RegEnumKeyW(hKey, dwIndex, szSubKeyName, _countof(szSubKeyName)) == ERROR_SUCCESS)
    {
        if (g_server_count >= MAX_SERVERS)
        {
            break;
        }

        HKEY hSubKey;
        if (RegOpenKeyExW(hKey, szSubKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
        {
            ServerInfo *newServer = &g_servers[g_server_count];
            DWORD dwType;
            DWORD dwIpSize = sizeof(newServer->ipAddress);
            DWORD dwPortSize = sizeof(newServer->port);

            if (RegQueryValueExW(hSubKey, szIpValueName, NULL, &dwType, (LPBYTE)newServer->ipAddress, &dwIpSize) == ERROR_SUCCESS &&
                RegQueryValueExW(hSubKey, szPortValueName, NULL, &dwType, (LPBYTE)&newServer->port, &dwPortSize) == ERROR_SUCCESS)
            {

                newServer->status = SERVER_STOPPED;
                newServer->listenSocket = INVALID_SOCKET;
                newServer->hThread = NULL;

                LVITEMW lvi = {0};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = g_server_count;
                lvi.iSubItem = 0;

                WCHAR statusText[64];
                LoadStringW(g_hinst, IDS_STATUS_STOPPED, statusText, _countof(statusText));
                lvi.pszText = statusText;

                int rowIndex = ListView_InsertItem(g_server_controls.hwndListServers, &lvi);
                ListView_SetItemText(g_server_controls.hwndListServers, rowIndex, 1, newServer->ipAddress);

                WCHAR port_wstr[16];
                _itow(newServer->port, port_wstr, 10);
                ListView_SetItemText(g_server_controls.hwndListServers, rowIndex, 2, port_wstr);

                g_server_count++;
                loadedCount++;
            }
            RegCloseKey(hSubKey);
        }
        dwIndex++;
    }

    LeaveCriticalSection(&g_cs_servers);
    RegCloseKey(hKey);

    if (loadedCount > 0)
    {
        AddLogMessage(L"Loaded %d server configuration(s) from registry.", loadedCount);
    }
    else
    {
        WCHAR logMsg[256];
        LoadStringW(g_hinst, IDS_LOG_NO_SERVERS_FOUND, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);
    }
}