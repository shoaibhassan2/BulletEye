#ifndef FE_TREE_VIEW_DISPLAY_H
#define FE_TREE_VIEW_DISPLAY_H

#include <gui/file_explorer/fe_main_window.h>
#include <cJSON/cJSON.h>

BOOL CreateTreeViewDisplay(HWND hParent, FileExplorerData *pData);

void UpdateDrivesInTreeView(FileExplorerData *pData, cJSON *drivesArray);

void UpdateTreeChildren(FileExplorerData *pData, HTREEITEM hParent, cJSON *itemsArray);

#endif