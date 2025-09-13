

#include <server/server_listview.h>
#include <server/server_logic.h>
#include <globals.h>
#include <helpers/utils.h>
#include <windows.h>
#include <commctrl.h>
#include <gui/log_page.h>
#include <resource.h>

void AddServerToList()
{
    EnterCriticalSection(&g_cs_servers);
    if (g_server_count >= MAX_SERVERS)
    {
        WCHAR msg[256], caption[64];
        LoadStringW(g_hinst, IDS_ERR_MAX_SERVERS_REACHED, msg, _countof(msg));
        LoadStringW(g_hinst, IDS_ERR_CAPTION, caption, _countof(caption));
        MessageBoxW(NULL, msg, caption, MB_OK);
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    DWORD ip;
    SendMessageW(g_server_controls.hwndIpAddress, IPM_GETADDRESS, 0, (LPARAM)&ip);
    WCHAR port_wstr[16];
    GetWindowTextW(g_server_controls.hwndEditPort, port_wstr, 16);
    int port = _wtoi(port_wstr);

    ServerInfo *newServer = &g_servers[g_server_count];
    wsprintfW(newServer->ipAddress, L"%d.%d.%d.%d", FIRST_IPADDRESS(ip), SECOND_IPADDRESS(ip), THIRD_IPADDRESS(ip), FOURTH_IPADDRESS(ip));
    newServer->port = port;
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
    ListView_SetItemText(g_server_controls.hwndListServers, rowIndex, 2, port_wstr);

    AddLogMessage(L"Added server configuration for %s:%d.", newServer->ipAddress, newServer->port);

    g_server_count++;
    LeaveCriticalSection(&g_cs_servers);
}