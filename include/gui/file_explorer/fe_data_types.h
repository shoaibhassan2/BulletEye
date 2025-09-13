#ifndef FE_DATA_TYPES_H
#define FE_DATA_TYPES_H

#include <globals.h>
#include <cJSON/cJSON.h>

#define MAX_HISTORY 50

typedef struct
{
    WCHAR fullPath[MAX_PATH];
    BOOL isDirectory;
} LV_ITEM_DATA;

typedef struct CacheNode
{
    WCHAR *path;
    cJSON *jsonResponse;
    struct CacheNode *next;
} CacheNode;

typedef struct
{
    long long current;
    long long total;
    int clientIndex;
    BOOL isUpload;
    WCHAR fileName[MAX_PATH];
    HWND hNotifyWnd;
} FileTransferProgressInfo;

#endif