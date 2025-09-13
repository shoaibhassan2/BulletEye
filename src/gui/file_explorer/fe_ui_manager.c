
#include <gui/file_explorer/fe_ui_manager.h>
#include <gui/file_explorer/fe_toolbar_view.h>
#include <gui/file_explorer/fe_list_view_display.h>
#include <gui/file_explorer/fe_tree_view_display.h>
#include <gui/file_explorer/fe_path_navigator.h>
#include <resource.h>
#include <DarkMode.h>
#include <gui/log_page.h>
#include <uxtheme.h>
#include <commctrl.h>

BOOL CreateExplorerControls(HWND hWnd, FileExplorerData *pData)
{

    if (!CreateToolbarView(hWnd, pData))
        return FALSE;

    if (!CreateTreeViewDisplay(hWnd, pData))
        return FALSE;

    if (!CreateListViewDisplay(hWnd, pData))
        return FALSE;

    pData->hStatusBar = CreateWindowEx(0, STATUSCLASSNAMEW, NULL,
                                       SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
                                       0, 0, 0, 0, hWnd, (HMENU)IDC_FE_STATUSBAR, g_hinst, NULL);
    SendMessage(pData->hStatusBar, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    return TRUE;
}

void SetExplorerControlsEnabled(FileExplorerData *pData, BOOL bEnabled)
{
    if (!pData || !IsWindow(pData->hMain))
        return;

    if (bEnabled)
    {
        PostLogMessage(L"FE_UI: SetExplorerControlsEnabled called to ENABLE controls.");
    }
    else
    {
        PostLogMessage(L"FE_UI: SetExplorerControlsEnabled called to DISABLE controls.");
        if (pData->hStatusBar && IsWindow(pData->hStatusBar))
        {
            SendMessage(pData->hStatusBar, SB_SETTEXT, 0, (LPARAM)L"");
        }
    }

    EnableWindow(pData->hTreeView, bEnabled);
    EnableWindow(pData->hListView, bEnabled);
    EnableWindow(pData->hToolBar, bEnabled);

    if (bEnabled)
    {
        UpdateNavButtonsState(pData);
    }
    else
    {
        SendMessage(pData->hToolBar, TB_SETSTATE, IDC_FE_NAV_BACK, 0);
        SendMessage(pData->hToolBar, TB_SETSTATE, IDC_FE_NAV_FORWARD, 0);
    }
}

void ResizeExplorerControls(HWND hWnd)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    FileExplorerData *pData = (FileExplorerData *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (!pData)
        return;

    int toolbarH = 40, statusH = 22, treeW = 250;

    SetWindowPos(pData->hToolBar, NULL, 0, 0, rcClient.right, toolbarH, SWP_NOZORDER);
    int mainY = toolbarH;
    int mainH = rcClient.bottom - mainY - statusH;

    SetWindowPos(pData->hTreeView, NULL, 0, mainY, treeW, mainH, SWP_NOZORDER);
    SetWindowPos(pData->hListView, NULL, treeW, mainY, rcClient.right - treeW, mainH, SWP_NOZORDER);
    SetWindowPos(pData->hStatusBar, NULL, 0, rcClient.bottom - statusH, rcClient.right, statusH, SWP_NOZORDER);
    SendMessage(pData->hStatusBar, WM_SIZE, 0, 0);
}

LPARAM TreeView_GetItemParam(HWND hTree, HTREEITEM hItem)
{
    TVITEMW tvi = {0};
    tvi.mask = TVIF_PARAM;
    tvi.hItem = hItem;
    if (TreeView_GetItem(hTree, &tvi))
    {
        return tvi.lParam;
    }
    return 0;
}