#ifndef SOCKET_INIT_H
#define SOCKET_INIT_H
#include <winsock2.h>
BOOL TryConnectToServer(const char* ip, int port, SOCKET* out_sock);
SOCKET GetBestServerSocket(char** out_ip, int* out_port);
#endif