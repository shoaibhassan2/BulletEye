#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <server/server_logic.h>

void UpdateServerStatusInGUI(int serverIndex, ServerStatus status);
void StartServer(int serverIndex);
void StopServer(int serverIndex);

#endif