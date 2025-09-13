#ifndef FE_LIST_VIEW_EVENTS_H
#define FE_LIST_VIEW_EVENTS_H

#include <gui/file_explorer/fe_main_window.h>

void OnListViewDblClick(FileExplorerData *pData, LPARAM lParam);

void OnListViewEndLabelEdit(FileExplorerData *pData, LPARAM lParam);

void OnFileDrop(FileExplorerData *pData, WPARAM wParam);

void ShowExplorerContextMenu(FileExplorerData *pData, POINT pt);

#endif