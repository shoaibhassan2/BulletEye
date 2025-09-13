#ifndef FE_TOOLBAR_VIEW_H
#define FE_TOOLBAR_VIEW_H

#include <gui/file_explorer/fe_main_window.h>

BOOL CreateToolbarView(HWND hParent, FileExplorerData *pData);

void UpdateNavButtonsState(FileExplorerData *pData);

#endif