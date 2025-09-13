#ifndef FE_TREE_VIEW_EVENTS_H
#define FE_TREE_VIEW_EVENTS_H

#include <gui/file_explorer/fe_main_window.h>

void OnTreeViewSelChanged(FileExplorerData *pData, LPARAM lParam);

void OnTreeViewItemExpanding(FileExplorerData *pData, LPARAM lParam);

#endif