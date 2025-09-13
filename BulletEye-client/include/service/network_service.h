// include/service/network_service.h
#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H

#include <winsock2.h>
#include <windows.h>
#include <stdint.h>
#include <cJSON/cJSON.h>

// --- Network Protocol Definition ---
typedef enum {
    PACKET_TYPE_JSON = 1,
    PACKET_TYPE_BINARY_FILE = 2
} PacketType;

#pragma pack(push, 1)
typedef struct {
    uint32_t packet_type;
    uint32_t json_size;
    uint32_t binary_size;
} PacketHeader;
#pragma pack(pop)

// Removed ProgressInfo struct from here, as it's specific to the server's GUI.
// The FileProgressCallback will use a generic void* userData.

// --- Public API Functions ---
// Modified signature to include void* userData
typedef void (*FileProgressCallback)(long long bytes_transferred, long long total_bytes, void* userData);


// --- Public API Functions (UPDATED SIGNATURES) ---

BOOL SendJsonRequest(SOCKET s, const cJSON* json);

/**
 * @brief Sends a request containing a JSON object and a binary file with progress reporting.
 *
 * @param s The connected socket to send to.
 * @param json The cJSON object to send as metadata.
 * @param file_path The full path to the binary file to send.
 * @param callback An optional function pointer to be called with progress updates. Can be NULL.
 * @param callback_data User-defined data to pass to the callback function.
 * @return TRUE on success, FALSE on failure.
 */
BOOL SendFileRequest(SOCKET s, const cJSON* json, const WCHAR* file_path, FileProgressCallback callback, void* callback_data);

/**
 * @brief Receives and parses an incoming request with progress reporting for binary data.
 *
 * @param s The connected socket to receive from.
 * @param packet_type_out Pointer to store the received PacketType.
 * @param json_out Pointer to a cJSON* that will be allocated and populated.
 * @param binary_data_out Pointer to a char* buffer that will be allocated for the binary data.
 * @param binary_size_out Pointer to store the size of the received binary data.
 * @param callback An optional function pointer to be called with progress updates. Can be NULL.
 * @param callback_data User-defined data to pass to the callback function.
 * @return TRUE on success, FALSE on failure.
 */
BOOL ReceiveRequest(SOCKET s, PacketType* packet_type_out, cJSON** json_out, char** binary_data_out, uint32_t* binary_size_out, FileProgressCallback callback, void* callback_data);

#endif // NETWORK_SERVICE_H