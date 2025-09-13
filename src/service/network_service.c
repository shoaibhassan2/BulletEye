

#include <service/network_service.h>
#include <service/compression_service.h>
#include <service/encryption_service.h>
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>

#ifdef __TINYC__
#include <windows.h>
__declspec(dllimport) intptr_t _get_osfhandle(int fd);
static long long ftelli64_tcc(FILE *f)
{
    HANDLE h = (HANDLE)_get_osfhandle(_fileno(f));
    LARGE_INTEGER pos;
    pos.QuadPart = 0;
    SetFilePointerEx(h, pos, &pos, FILE_CURRENT);
    return pos.QuadPart;
}

#define _ftelli64(f) ftelli64_tcc(f)
#endif

static BOOL send_all(SOCKET s, const char *buf, int len)
{
    int total_sent = 0;
    while (total_sent < len)
    {
        int sent = send(s, buf + total_sent, len - total_sent, 0);
        if (sent == SOCKET_ERROR)
        {

            return FALSE;
        }
        total_sent += sent;
    }
    return TRUE;
}

static BOOL recv_all(SOCKET s, char *buf, int len)
{
    int total_received = 0;
    while (total_received < len)
    {
        int received = recv(s, buf + total_received, len - total_received, 0);
        if (received == SOCKET_ERROR)
        {

            return FALSE;
        }
        if (received == 0)
        {

            return FALSE;
        }
        total_received += received;
    }
    return TRUE;
}

BOOL SendJsonRequest(SOCKET s, const cJSON *json)
{
    char *json_string = cJSON_PrintUnformatted(json);
    if (!json_string)
        return FALSE;

    BYTE *compressed_data = NULL, *encrypted_data = NULL;
    DWORD compressed_size = 0, encrypted_size = 0;
    BOOL success = FALSE;

    if (!CompressData((BYTE *)json_string, (DWORD)strlen(json_string), &compressed_data, &compressed_size))
    {
        goto cleanup;
    }

    if (!AESEncrypt(compressed_data, compressed_size, &encrypted_data, &encrypted_size))
    {
        goto cleanup;
    }

    PacketHeader header;
    header.packet_type = htonl((uint32_t)PACKET_TYPE_JSON);
    header.json_size = htonl(encrypted_size);
    header.binary_size = 0;

    if (!send_all(s, (char *)&header, sizeof(header)))
    {
        goto cleanup;
    }

    if (!send_all(s, (char *)encrypted_data, encrypted_size))
    {
        goto cleanup;
    }

    success = TRUE;

cleanup:
    cJSON_free(json_string);
    if (compressed_data)
        free(compressed_data);
    if (encrypted_data)
        free(encrypted_data);
    return success;
}

BOOL SendFileRequest(SOCKET s, const cJSON *json, const WCHAR *file_path, FileProgressCallback callback, void *callback_data)
{
    BOOL success = FALSE;

    char *json_string = cJSON_PrintUnformatted(json);
    if (!json_string)
        return FALSE;

    BYTE *compressed_json = NULL, *encrypted_json = NULL;
    DWORD compressed_json_size = 0, encrypted_json_size = 0;

    if (!CompressData((BYTE *)json_string, (DWORD)strlen(json_string), &compressed_json, &compressed_json_size) ||
        !AESEncrypt(compressed_json, compressed_json_size, &encrypted_json, &encrypted_json_size))
    {
        cJSON_free(json_string);
        if (compressed_json)
            free(compressed_json);
        return FALSE;
    }
    cJSON_free(json_string);
    free(compressed_json);

    FILE *file = NULL;
    long long file_size = 0;
    BYTE *file_buffer = NULL;
    BYTE *compressed_file = NULL, *encrypted_file = NULL;
    DWORD compressed_file_size = 0, encrypted_file_size = 0;

    if (file_path && wcslen(file_path) > 0)
    {
        if (_wfopen_s(&file, file_path, L"rb") != 0 || !file)
        {
            goto cleanup;
        }

        _fseeki64(file, 0, SEEK_END);
        file_size = _ftelli64(file);
        rewind(file);

        if (file_size > (1024LL * 1024 * 500))
        {

            goto cleanup;
        }

        file_buffer = (BYTE *)malloc((size_t)file_size);
        if (!file_buffer)
        {
            goto cleanup;
        }

        if (file_size > 0 && fread(file_buffer, 1, (size_t)file_size, file) != file_size)
        {
            goto cleanup;
        }
        fclose(file);
        file = NULL;

        if (file_size > 0)
        {
            if (!CompressData(file_buffer, (DWORD)file_size, &compressed_file, &compressed_file_size) ||
                !AESEncrypt(compressed_file, compressed_file_size, &encrypted_file, &encrypted_file_size))
            {
                goto cleanup;
            }
        }
    }

    PacketHeader header;
    header.packet_type = htonl((uint32_t)PACKET_TYPE_BINARY_FILE);
    header.json_size = htonl(encrypted_json_size);
    header.binary_size = htonl(encrypted_file_size);

    if (!send_all(s, (char *)&header, sizeof(header)) ||
        !send_all(s, (char *)encrypted_json, encrypted_json_size))
    {
        goto cleanup;
    }

    const int CHUNK_SIZE = 4096;
    long long total_sent = 0;
    while (total_sent < encrypted_file_size)
    {
        int bytes_to_send = min(CHUNK_SIZE, (int)(encrypted_file_size - total_sent));
        if (!send_all(s, (char *)encrypted_file + total_sent, bytes_to_send))
        {
            goto cleanup;
        }
        total_sent += bytes_to_send;
        if (callback)
        {
            callback(total_sent, encrypted_file_size, callback_data);
        }
    }
    success = TRUE;

cleanup:
    if (file)
        fclose(file);
    if (file_buffer)
        free(file_buffer);
    if (compressed_file)
        free(compressed_file);
    if (encrypted_file)
        free(encrypted_file);
    if (encrypted_json)
        free(encrypted_json);
    return success;
}

BOOL ReceiveRequest(SOCKET s, PacketType *packet_type_out, cJSON **json_out, char **binary_data_out, uint32_t *binary_size_out, FileProgressCallback callback, void *callback_data)
{

    *json_out = NULL;
    *binary_data_out = NULL;
    *binary_size_out = 0;
    *packet_type_out = 0;
    BOOL success = FALSE;

    BYTE *encrypted_buffer = NULL, *decrypted_buffer = NULL, *decompressed_buffer = NULL;

    PacketHeader header;
    if (!recv_all(s, (char *)&header, sizeof(header)))
        return FALSE;

    header.packet_type = ntohl(header.packet_type);
    header.json_size = ntohl(header.json_size);
    header.binary_size = ntohl(header.binary_size);
    *packet_type_out = (PacketType)header.packet_type;

    if (header.json_size > 0)
    {
        DWORD decrypted_size = 0, decompressed_size = 0;
        encrypted_buffer = (BYTE *)malloc(header.json_size);
        if (!encrypted_buffer || !recv_all(s, (char *)encrypted_buffer, header.json_size))
        {
            goto cleanup;
        }

        if (!AESDecrypt(encrypted_buffer, header.json_size, &decrypted_buffer, &decrypted_size) ||
            !DecompressData(decrypted_buffer, decrypted_size, &decompressed_buffer, &decompressed_size))
        {
            goto cleanup;
        }

        char *final_json_string = (char *)realloc(decompressed_buffer, decompressed_size + 1);
        if (!final_json_string)
        {
            decompressed_buffer = NULL;
            goto cleanup;
        }
        final_json_string[decompressed_size] = '\0';
        decompressed_buffer = NULL;

        *json_out = cJSON_Parse(final_json_string);
        free(final_json_string);
        if (*json_out == NULL)
            goto cleanup;

        free(encrypted_buffer);
        encrypted_buffer = NULL;
        free(decrypted_buffer);
        decrypted_buffer = NULL;
    }

    if (header.binary_size > 0)
    {
        encrypted_buffer = (BYTE *)malloc(header.binary_size);
        if (!encrypted_buffer)
            goto cleanup;

        const int CHUNK_SIZE = 4096;
        long long total_received = 0;
        while (total_received < header.binary_size)
        {
            int bytes_to_receive = min(CHUNK_SIZE, (int)(header.binary_size - total_received));
            int received = recv(s, (char *)encrypted_buffer + total_received, bytes_to_receive, 0);
            if (received == SOCKET_ERROR || received == 0)
            {
                goto cleanup;
            }
            total_received += received;
            if (callback)
            {
                callback(total_received, header.binary_size, callback_data);
            }
        }

        DWORD decrypted_size = 0, decompressed_size = 0;
        if (!AESDecrypt(encrypted_buffer, header.binary_size, &decrypted_buffer, &decrypted_size) ||
            !DecompressData(decrypted_buffer, decrypted_size, &decompressed_buffer, &decompressed_size))
        {
            goto cleanup;
        }

        *binary_data_out = (char *)decompressed_buffer;
        *binary_size_out = decompressed_size;
        decompressed_buffer = NULL;

        free(encrypted_buffer);
        encrypted_buffer = NULL;
        free(decrypted_buffer);
        decrypted_buffer = NULL;
    }

    success = TRUE;

cleanup:

    if (encrypted_buffer)
        free(encrypted_buffer);
    if (decrypted_buffer)
        free(decrypted_buffer);
    if (decompressed_buffer)
        free(decompressed_buffer);

    if (!success)
    {
        if (*json_out)
        {
            cJSON_Delete(*json_out);
            *json_out = NULL;
        }
        if (*binary_data_out)
        {
            free(*binary_data_out);
            *binary_data_out = NULL;
        }
    }
    return success;
}