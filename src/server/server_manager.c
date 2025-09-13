

#include <stdio.h>
#include <server/server_logic.h>
#include <windows.h>
#include <commctrl.h>
#include <server/server_manager.h>
#include <globals.h>
#include <gui/log_page.h>
#include <resource.h>

ServerInfo g_servers[MAX_SERVERS];
int g_server_count = 0;

void UpdateServerStatusInGUI(int serverIndex, ServerStatus status)
{
    if (serverIndex < 0 || serverIndex >= g_server_count)
        return;

    WCHAR statusBuffer[64];
    UINT statusStringId;

    switch (status)
    {
    case SERVER_LISTENING:
        statusStringId = IDS_STATUS_LISTENING;
        break;
    case SERVER_STARTING:
        statusStringId = IDS_STATUS_STARTING;
        break;
    case SERVER_ERROR:
        statusStringId = IDS_STATUS_ERROR;
        break;
    case SERVER_STOPPED:
    default:
        statusStringId = IDS_STATUS_STOPPED;
        break;
    }

    if (LoadStringW(g_hinst, statusStringId, statusBuffer, _countof(statusBuffer)))
    {
        ListView_SetItemText(g_server_controls.hwndListServers, serverIndex, 0, statusBuffer);
    }
    else
    {

        ListView_SetItemText(g_server_controls.hwndListServers, serverIndex, 0, L"Unknown");
    }
}

void StartServer(int serverIndex)
{
    EnterCriticalSection(&g_cs_servers);

    if (serverIndex < 0 || serverIndex >= g_server_count)
    {
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    ServerInfo *pServer = &g_servers[serverIndex];
    if (pServer->status == SERVER_LISTENING || pServer->status == SERVER_STARTING)
    {
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    pServer->status = SERVER_STARTING;

    PostMessage(g_hWnd, WM_APP_UPDATE_STATUS, (WPARAM)serverIndex, (LPARAM)SERVER_STARTING);
    AddLogMessage(L"Starting server on %s:%d...", pServer->ipAddress, pServer->port);

    pServer->hThread = CreateThread(NULL, 0, StartListenerThread, pServer, 0, NULL);
    if (!pServer->hThread)
    {

        pServer->status = SERVER_ERROR;
        PostMessage(g_hWnd, WM_APP_UPDATE_STATUS, (WPARAM)serverIndex, (LPARAM)SERVER_ERROR);
        AddLogMessage(L"Error: Failed to create listener thread for server %s:%d.", pServer->ipAddress, pServer->port);
    }

    LeaveCriticalSection(&g_cs_servers);
}

void StopServer(int serverIndex)
{
    HANDLE hThreadToWaitOn = NULL;
    ServerInfo serverCopy;

    EnterCriticalSection(&g_cs_servers);

    if (serverIndex < 0 || serverIndex >= g_server_count)
    {
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    ServerInfo *pServer = &g_servers[serverIndex];
    if (pServer->status != SERVER_LISTENING)
    {
        LeaveCriticalSection(&g_cs_servers);
        return;
    }

    pServer->status = SERVER_STOPPED;

    if (pServer->listenSocket != INVALID_SOCKET)
    {
        closesocket(pServer->listenSocket);
        pServer->listenSocket = INVALID_SOCKET;
    }

    hThreadToWaitOn = pServer->hThread;
    pServer->hThread = NULL;

    serverCopy = *pServer;

    LeaveCriticalSection(&g_cs_servers);

    if (hThreadToWaitOn != NULL)
    {

        WaitForSingleObject(hThreadToWaitOn, 2000);
        CloseHandle(hThreadToWaitOn);
    }

    PostMessage(g_hWnd, WM_APP_UPDATE_STATUS, (WPARAM)serverIndex, (LPARAM)SERVER_STOPPED);
    AddLogMessage(L"Server on %s:%d stopped.", serverCopy.ipAddress, serverCopy.port);
}