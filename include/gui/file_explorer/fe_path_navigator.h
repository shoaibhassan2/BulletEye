#ifndef FE_PATH_NAVIGATOR_H
#define FE_PATH_NAVIGATOR_H

#include <gui/file_explorer/fe_main_window.h>

void NavigateTo(FileExplorerData *pData, LPCWSTR pszPath, BOOL bAddToHistory);

void OnNavBack(FileExplorerData *pData);

void OnNavForward(FileExplorerData *pData);

#endif