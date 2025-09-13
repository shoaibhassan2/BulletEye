#ifndef FE_UI_MANAGER_H
#define FE_UI_MANAGER_H

#include <gui/file_explorer/fe_main_window.h>
#include <cJSON/cJSON.h>

BOOL CreateExplorerControls(HWND hWnd, FileExplorerData *pData);

void SetExplorerControlsEnabled(FileExplorerData *pData, BOOL bEnabled);

void ResizeExplorerControls(HWND hWnd);

LPARAM TreeView_GetItemParam(HWND hTree, HTREEITEM hItem);

#endif