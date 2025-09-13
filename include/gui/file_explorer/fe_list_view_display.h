#ifndef FE_LIST_VIEW_DISPLAY_H
#define FE_LIST_VIEW_DISPLAY_H

#include <gui/file_explorer/fe_main_window.h>
#include <cJSON/cJSON.h>

BOOL CreateListViewDisplay(HWND hParent, FileExplorerData *pData);

void UpdateListView(FileExplorerData *pData, cJSON *itemsArray);

#endif