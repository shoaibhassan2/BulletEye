
#include <gui/file_explorer/fe_data_caching.h>
#include <helpers/utils.h>
#include <gui/log_page.h>
#include <stdlib.h>

void ClearCache(FileExplorerData *pData)
{
    EnterCriticalSection(&pData->csCache);
    CacheNode *current = pData->pCacheHead;
    while (current != NULL)
    {
        CacheNode *next = current->next;
        free(current->path);
        cJSON_Delete(current->jsonResponse);
        free(current);
        current = next;
    }
    pData->pCacheHead = NULL;
    PostLogMessage(L"FE: Cache cleared.");
    LeaveCriticalSection(&pData->csCache);
}

void RemoveFromCache(FileExplorerData *pData, LPCWSTR path)
{
    EnterCriticalSection(&pData->csCache);
    CacheNode **pp = &pData->pCacheHead;
    while (*pp)
    {
        CacheNode *entry = *pp;
        if (_wcsicmp(entry->path, path) == 0)
        {
            *pp = entry->next;
            free(entry->path);
            cJSON_Delete(entry->jsonResponse);
            PostLogMessage(L"FE: Removed '%s' from cache.", path);
            free(entry);
            break;
        }
        pp = &entry->next;
    }
    LeaveCriticalSection(&pData->csCache);
}

void AddToCache(FileExplorerData *pData, LPCWSTR path, cJSON *json)
{

    if (pData->bShutdown)
    {
        cJSON_Delete(json);
        PostLogMessage(L"FE: AddToCache aborted, window shutting down.");
        return;
    }

    RemoveFromCache(pData, path);
    EnterCriticalSection(&pData->csCache);

    if (pData->bShutdown)
    {
        LeaveCriticalSection(&pData->csCache);
        cJSON_Delete(json);
        PostLogMessage(L"FE: AddToCache aborted after lock, window shutting down.");
        return;
    }

    CacheNode *newNode = (CacheNode *)malloc(sizeof(CacheNode));
    if (newNode)
    {
        newNode->path = _wcsdup(path);

        newNode->jsonResponse = cJSON_Duplicate(json, 1);
        newNode->next = pData->pCacheHead;
        if (newNode->jsonResponse == NULL && json != NULL)
        {
            PostLogMessage(L"FE: Failed to duplicate JSON for cache entry '%s'.", path);
        }
        pData->pCacheHead = newNode;
        PostLogMessage(L"FE: Added '%s' to cache.", path);
    }
    else
    {
        PostLogMessage(L"FE: Failed to allocate memory for cache entry '%s'.", path);
    }
    LeaveCriticalSection(&pData->csCache);
}

cJSON *GetFromCache(FileExplorerData *pData, LPCWSTR path)
{
    EnterCriticalSection(&pData->csCache);
    CacheNode *current = pData->pCacheHead;
    while (current != NULL)
    {
        if (_wcsicmp(current->path, path) == 0)
        {
            PostLogMessage(L"FE: Retrieving '%s' from cache.", path);

            cJSON *result = cJSON_Duplicate(current->jsonResponse, 1);
            LeaveCriticalSection(&pData->csCache);
            return result;
        }
        current = current->next;
    }
    LeaveCriticalSection(&pData->csCache);
    return NULL;
}