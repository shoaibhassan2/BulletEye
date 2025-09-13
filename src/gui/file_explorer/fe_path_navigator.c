

#include <gui/file_explorer/fe_path_navigator.h>
#include <gui/file_explorer/fe_toolbar_view.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <shlwapi.h>
#include <stdio.h>
#include <gui/log_page.h>
#include <wchar.h>

void NavigateTo(FileExplorerData *pData, LPCWSTR pszPath, BOOL bAddToHistory)
{
    if (!pData || !pszPath || wcslen(pszPath) == 0)
    {
        PostLogMessage(L"FE_PathNav: NavigateTo called with invalid/empty pszPath. Path: '%ls'", (pszPath ? pszPath : L"(null)"));
        UpdateNavButtonsState(pData);
        return;
    }

    PostLogMessage(L"FE_PathNav: Navigating to: '%ls'", pszPath);

    WCHAR szCleanPath[MAX_PATH];
#ifndef __TINYC__
    wcscpy_s(szCleanPath, MAX_PATH, pszPath);
#else
    snwprintf(szCleanPath, MAX_PATH, L"%ls", pszPath);
#endif

    size_t len = wcslen(szCleanPath);

    if (len == 2 && szCleanPath[1] == L':')
    {
        if (szCleanPath[len - 1] != L'\\')
        {
            wcsncat(szCleanPath, L"\\", MAX_PATH - wcslen(szCleanPath) - 1);
        }
    }

    else if (len > 3 && szCleanPath[len - 1] == L'\\')
    {
        PathRemoveBackslashW(szCleanPath);
    }

    if (len == 2 && szCleanPath[1] == L':')
    {
        if (szCleanPath[len] != L'\\')
        {
#ifndef __TINYC__
            wcscat_s(szCleanPath, MAX_PATH, L"\\");
#else
            snwprintf(szCleanPath + wcslen(szCleanPath),
                      MAX_PATH - wcslen(szCleanPath),
                      L"%ls", L"\\");
#endif
        }
    }

    len = wcslen(szCleanPath);
    if (len == 2 && szCleanPath[1] == L':')
    {
#ifndef __TINYC__
        wcscat_s(szCleanPath, MAX_PATH, L"\\");
#else
        snwprintf(szCleanPath + wcslen(szCleanPath),
                  MAX_PATH - wcslen(szCleanPath),
                  L"%ls", L"\\");
#endif
    }
    else
    {
        PathRemoveBackslashW(szCleanPath);
    }

    if (_wcsicmp(pData->szCurrentPath, szCleanPath) == 0 && pData->szCurrentPath[0] != L'\0')
    {
        PostLogMessage(L"FE_PathNav: Already at path: '%ls'. Refreshing.", pData->szCurrentPath);
        RequestDirectoryListing(pData, pData->szCurrentPath);
        return;
    }

    if (bAddToHistory && pData->szCurrentPath[0] != L'\0')
    {
        if (pData->iBackStackTop < MAX_HISTORY - 1)
        {
            pData->iBackStackTop++;
#ifndef __TINYC__
            wcscpy_s(pData->szBackHistory[pData->iBackStackTop], MAX_PATH, pData->szCurrentPath);
#else
            snwprintf(pData->szBackHistory[pData->iBackStackTop], MAX_PATH, L"%ls", pData->szCurrentPath);
#endif
            PostLogMessage(L"FE_PathNav: Added '%ls' to back history (top: %d)", pData->szCurrentPath, pData->iBackStackTop);
        }
        else
        {
            PostLogMessage(L"FE_PathNav: Back history full, cannot add '%ls'", pData->szCurrentPath);
        }
        pData->iForwardStackTop = -1;
    }

#ifndef __TINYC__
    wcscpy_s(pData->szCurrentPath, MAX_PATH, szCleanPath);
#else
    snwprintf(pData->szCurrentPath, MAX_PATH, L"%ls", szCleanPath);
#endif

    RequestDirectoryListing(pData, pData->szCurrentPath);
    UpdateNavButtonsState(pData);
}

void OnNavBack(FileExplorerData *pData)
{
    if (!pData || !IsWindow(pData->hMain))
        return;
    PostLogMessage(L"FE_PathNav: OnNavBack called. Current backTop: %d", pData->iBackStackTop);

    if (pData->iBackStackTop > -1)
    {
        if (pData->iForwardStackTop < MAX_HISTORY - 1)
        {
            pData->iForwardStackTop++;
#ifndef __TINYC__
            wcscpy_s(pData->szForwardHistory[pData->iForwardStackTop], MAX_PATH, pData->szCurrentPath);
#else
            snwprintf(pData->szForwardHistory[pData->iForwardStackTop], MAX_PATH, L"%ls", pData->szCurrentPath);
#endif
            PostLogMessage(L"FE_PathNav: Pushed '%ls' to forward history (top: %d)", pData->szCurrentPath, pData->iForwardStackTop);
        }
        else
        {
            PostLogMessage(L"FE_PathNav: Forward history full, cannot push '%ls'", pData->szCurrentPath);
        }

        LPCWSTR pszPath = pData->szBackHistory[pData->iBackStackTop];
        PostLogMessage(L"FE_PathNav: Retrieving from back history (index %d): '%ls'", pData->iBackStackTop, pszPath);
        pData->iBackStackTop--;
        NavigateTo(pData, pszPath, FALSE);
    }
    else
    {
        PostLogMessage(L"FE_PathNav: Back history is empty. Cannot go back.");
    }
}

void OnNavForward(FileExplorerData *pData)
{
    if (!pData || !IsWindow(pData->hMain))
        return;
    PostLogMessage(L"FE_PathNav: OnNavForward called. Current forwardTop: %d", pData->iForwardStackTop);

    if (pData->iForwardStackTop > -1)
    {

        if (pData->iBackStackTop < MAX_HISTORY - 1)
        {
            pData->iBackStackTop++;
#ifndef __TINYC__
            wcscpy_s(pData->szBackHistory[pData->iBackStackTop], MAX_PATH, pData->szCurrentPath);
#else
            snwprintf(pData->szBackHistory[pData->iBackStackTop], MAX_PATH, L"%ls", pData->szCurrentPath);
#endif
            PostLogMessage(L"FE_PathNav: Pushed '%ls' to back history (top: %d)", pData->szCurrentPath, pData->iBackStackTop);
        }
        else
        {
            PostLogMessage(L"FE_PathNav: Back history full, cannot push '%ls'", pData->szCurrentPath);
        }

        LPCWSTR pszPath = pData->szForwardHistory[pData->iForwardStackTop];
        PostLogMessage(L"FE_PathNav: Retrieving from forward history (index %d): '%ls'", pData->iForwardStackTop, pszPath);
        pData->iForwardStackTop--;
        NavigateTo(pData, pszPath, FALSE);
    }
    else
    {
        PostLogMessage(L"FE_PathNav: Forward history is empty. Cannot go forward.");
    }
}