
#ifndef FE_MAIN_WINDOW_H
#define FE_MAIN_WINDOW_H

#include <globals.h>
#include <commctrl.h>
#include <cJSON/cJSON.h>
#include <gui/file_explorer/fe_data_types.h>

typedef struct
{
    int clientIndex;
    HWND hMain;
    HWND hTreeView;
    HWND hListView;
    HWND hStatusBar;
    HWND hToolBar;
    WCHAR szCurrentPath[MAX_PATH];
    WCHAR szBackHistory[MAX_HISTORY][MAX_PATH];
    WCHAR szForwardHistory[MAX_HISTORY][MAX_PATH];
    int iBackStackTop;
    int iForwardStackTop;
    BOOL bShowHidden;
    CRITICAL_SECTION csCache;
    CacheNode *pCacheHead;
    BOOL bShutdown;
} FileExplorerData;

void CreateFileExplorerWindow(HWND hParent, int clientIndex);

LRESULT CALLBACK FileExplorerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void FreeTreeViewItemData(HWND hTree, HTREEITEM hItem);

#endif