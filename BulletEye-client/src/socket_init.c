#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <socket_init.h>
#include <debug_print.h>

#define fallback_ip     "127.0.0.1"
#define fallback_port   50423
BOOL TryConnectToServer(const char* ip, int port, SOCKET* out_sock) {
    struct sockaddr_in servAddr;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) return FALSE;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = inet_addr(ip);

    if (servAddr.sin_addr.s_addr == INADDR_NONE) {
        struct hostent* he = gethostbyname(ip);
        if (!he) {
            closesocket(s);
            return FALSE;
        }
        servAddr.sin_addr = *(struct in_addr*)he->h_addr;
    }

    DEBUG_PRINT("Trying to connect to %s:%d...", ip, port);
    if (connect(s, (struct sockaddr*)&servAddr, sizeof(servAddr)) == 0) {
        *out_sock = s;
        DEBUG_PRINT("Connected to %s:%d", ip, port);
        return TRUE;
    }

    closesocket(s);
    return FALSE;
}

// BOOL ParseConfigIP(cJSON* obj, const char* key, char** ip_out, int* port_out) {
//     cJSON* entry = cJSON_GetObjectItem(obj, key);
//     if (!cJSON_IsObject(entry)) return FALSE;

//     cJSON* ip = cJSON_GetObjectItem(entry, "server_ip");
//     cJSON* port = cJSON_GetObjectItem(entry, "server_port");
//     if (!cJSON_IsString(ip) || !cJSON_IsNumber(port)) return FALSE;

//     *ip_out = _strdup(ip->valuestring);
//     *port_out = port->valueint;
//     return TRUE;
// }

SOCKET GetBestServerSocket(char** out_ip, int* out_port) {
    // const char* fallback_ip = SERVER_IP;
    // int fallback_port = SERVER_PORT;
    // char* json = GetJsonResponse("https://pastebin.com/raw/aprchZB0");

    SOCKET connected_sock = INVALID_SOCKET;
    // *out_ip = NULL;
    // *out_port = 0;

    // if (json) {
    //     cJSON* root = cJSON_Parse(json);


    //     free(json);

    //     if (root) {
    //         char *temp_ip = NULL, *perm_ip = NULL;
    //         int temp_port = 0, perm_port = 0;

    //         ParseConfigIP(root, "temporary", &temp_ip, &temp_port);
    //         ParseConfigIP(root, "permanent", &perm_ip, &perm_port);

    //         if (temp_ip && TryConnectToServer(temp_ip, temp_port, &connected_sock)) {
    //             *out_ip = temp_ip;
    //             *out_port = temp_port;
    //             if (perm_ip) free(perm_ip);
    //             cJSON_Delete(root);
    //             return connected_sock;
    //         }
    //         if (perm_ip && TryConnectToServer(perm_ip, perm_port, &connected_sock)) {
    //             *out_ip = perm_ip;
    //             *out_port = perm_port;
    //             if (temp_ip) free(temp_ip);
    //             cJSON_Delete(root);
    //             return connected_sock;
    //         }

    //         if (temp_ip) free(temp_ip);
    //         if (perm_ip) free(perm_ip);
    //         cJSON_Delete(root);
    //     }
    // }

    DEBUG_PRINT("Trying fallback hardcoded IP %s:%d", fallback_ip, fallback_port);
    if (TryConnectToServer(fallback_ip, fallback_port, &connected_sock)) {
        *out_ip = _strdup(fallback_ip);
        *out_port = fallback_port;
        return connected_sock;
    }

    return INVALID_SOCKET;
}