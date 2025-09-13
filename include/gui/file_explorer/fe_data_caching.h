#ifndef FE_DATA_CACHING_H
#define FE_DATA_CACHING_H

#include <gui/file_explorer/fe_main_window.h>
#include <cJSON/cJSON.h>

void ClearCache(FileExplorerData *pData);

void RemoveFromCache(FileExplorerData *pData, LPCWSTR path);

void AddToCache(FileExplorerData *pData, LPCWSTR path, cJSON *json);

cJSON *GetFromCache(FileExplorerData *pData, LPCWSTR path);

#endif