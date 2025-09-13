
#include <gui/file_explorer/fe_tree_view_events.h>
#include <gui/file_explorer/fe_path_navigator.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <gui/file_explorer/fe_ui_manager.h>
#include <commctrl.h>
#include <gui/log_page.h>

void OnTreeViewSelChanged(FileExplorerData *pData, LPARAM lParam)
{
    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged entered. pData: %p", pData);

    if (!pData || !IsWindow(pData->hMain) || !IsWindow(pData->hTreeView))
    {
        PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: Invalid pData or main/treeview window handles. Returning.");
        return;
    }

    LPNMTREEVIEWW lpnmtd = (LPNMTREEVIEWW)lParam;
    HTREEITEM hItem = lpnmtd->itemNew.hItem;

    if (hItem == NULL)
    {
        PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: Received NULL hItem from notification. Returning.");
        return;
    }
    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: Processing hItem: %p", hItem);

    LPCWSTR pszPath = (LPCWSTR)TreeView_GetItemParam(pData->hTreeView, hItem);
    if (!pszPath || pszPath[0] == L'\0')
    {
        PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: TreeView_GetItemParam returned NULL or empty path for hItem: %p. Returning.", hItem);
        return;
    }
    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: Path obtained: '%ls'", pszPath);

    NavigateTo(pData, pszPath, TRUE);
    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged: NavigateTo completed for '%ls'.", pszPath);

    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewSelChanged exited (TreeView expansion disabled).");
}

void OnTreeViewItemExpanding(FileExplorerData *pData, LPARAM lParam)
{

    PostLogMessage(L"FE_TreeViewEvents: OnTreeViewItemExpanding called (functionality disabled).");

    if (pData)
    {
        SetExplorerControlsEnabled(pData, TRUE);
    }
}