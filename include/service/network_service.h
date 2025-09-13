
#ifndef NETWORK_SERVICE_H
#define NETWORK_SERVICE_H

#include <globals.h>
#include <stdint.h>
#include <cJSON/cJSON.h>

typedef enum
{
    PACKET_TYPE_JSON = 1,
    PACKET_TYPE_BINARY_FILE = 2
} PacketType;

#pragma pack(push, 1)
typedef struct
{
    uint32_t packet_type;
    uint32_t json_size;
    uint32_t binary_size;
} PacketHeader;
#pragma pack(pop)

typedef void (*FileProgressCallback)(long long bytes_transferred, long long total_bytes, void *userData);

BOOL SendJsonRequest(SOCKET s, const cJSON *json);

BOOL SendFileRequest(SOCKET s, const cJSON *json, const WCHAR *file_path, FileProgressCallback callback, void *callback_data);

BOOL ReceiveRequest(SOCKET s, PacketType *packet_type_out, cJSON **json_out, char **binary_data_out, uint32_t *binary_size_out, FileProgressCallback callback, void *callback_data);

#endif