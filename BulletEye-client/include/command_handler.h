// include/command_handler.h
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <winsock2.h>
#include <cJSON/cJSON.h>
#include <stdint.h> // For uint32_t

// The main entry point for processing a received JSON command.
// Modified signature to include binary data
void HandleServerCommand(SOCKET sock, cJSON* commandJson, char* binaryData, uint32_t binarySize);

#endif // COMMAND_HANDLER_H