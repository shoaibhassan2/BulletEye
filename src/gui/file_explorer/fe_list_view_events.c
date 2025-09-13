
#include <gui/file_explorer/fe_list_view_events.h>
#include <gui/file_explorer/fe_path_navigator.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <gui/file_explorer/fe_data_types.h>
#include <gui/file_explorer/fe_preferences_dialog.h>
#include <resource.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <commctrl.h>
#include <gui/file_explorer/fe_custom_messages.h>

void OnListViewDblClick(FileExplorerData *pData, LPARAM lParam)
{
    LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lParam;
    if (lpnmia->iItem == -1)
        return;

    LVITEMW lvi = {0};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = lpnmia->iItem;
    ListView_GetItem(pData->hListView, &lvi);

    LV_ITEM_DATA *pItemData = (LV_ITEM_DATA *)lvi.lParam;
    if (!pItemData)
        return;

    if (pItemData->isDirectory)
    {

        NavigateTo(pData, pItemData->fullPath, TRUE);
    }
    else
    {

        WCHAR msg[256], cap[64];
        LoadStringW(g_hinst, IDS_CONFIRM_EXEC_TEXT, msg, _countof(msg));
        LoadStringW(g_hinst, IDS_CONFIRM_EXEC_CAPTION, cap, _countof(cap));
        if (MessageBoxW(pData->hMain, msg, cap, MB_YESNO | MB_ICONWARNING) == IDYES)
        {
            RequestFileExecution(pData, pItemData->fullPath);
        }
    }
}

void OnListViewEndLabelEdit(FileExplorerData *pData, LPARAM lParam)
{
    NMLVDISPINFOW *pdi = (NMLVDISPINFOW *)lParam;
    if (pdi->item.pszText == NULL || pdi->item.pszText[0] == L'\0')
        return;

    LVITEMW lvi = {0};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = pdi->item.iItem;
    ListView_GetItem(pData->hListView, &lvi);

    LV_ITEM_DATA *pItemData = (LV_ITEM_DATA *)lvi.lParam;
    if (!pItemData)
        return;

    WCHAR szNewPath[MAX_PATH];
    PathCombineW(szNewPath, pData->szCurrentPath, pdi->item.pszText);

    RequestFileRename(pData, pItemData->fullPath, szNewPath);
}

void OnFileDrop(FileExplorerData *pData, WPARAM wParam)
{
    HDROP hDrop = (HDROP)wParam;
    WCHAR szFilePath[MAX_PATH];
    UINT nFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

    for (UINT i = 0; i < nFiles; ++i)
    {
        if (DragQueryFileW(hDrop, i, szFilePath, MAX_PATH))
        {

            RequestFileUpload(pData, szFilePath, pData->szCurrentPath);
        }
    }

    DragFinish(hDrop);
}

void ShowExplorerContextMenu(FileExplorerData *pData, POINT pt)
{
    int iItem = ListView_GetNextItem(pData->hListView, -1, LVNI_SELECTED);
    if (iItem == -1)
        return;

    LVITEMW lvi = {0};
    lvi.mask = LVIF_PARAM;
    lvi.iItem = iItem;
    ListView_GetItem(pData->hListView, &lvi);
    LV_ITEM_DATA *pItemData = (LV_ITEM_DATA *)lvi.lParam;
    if (!pItemData)
        return;

    HMENU hMenu = CreatePopupMenu();
    WCHAR buf[128];

    LoadStringW(g_hinst, IDS_FE_MENU_OPEN, buf, 128);
    AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_OPEN, buf);
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

    if (!pItemData->isDirectory)
    {
        LoadStringW(g_hinst, IDS_FE_CONTEXT_ENCRYPT, buf, 128);
        AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_ENCRYPT, buf);
        LoadStringW(g_hinst, IDS_FE_CONTEXT_DECRYPT, buf, 128);
        AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_DECRYPT, buf);
        AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    }

    LoadStringW(g_hinst, IDS_FE_CONTEXT_RENAME, buf, 128);
    AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_RENAME, buf);
    LoadStringW(g_hinst, IDS_FE_CONTEXT_DELETE, buf, 128);
    AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_DELETE, buf);

    if (!pItemData->isDirectory)
    {
        LoadStringW(g_hinst, IDS_FE_CONTEXT_DOWNLOAD, buf, 128);
        AppendMenuW(hMenu, MF_STRING, IDM_FE_CONTEXT_DOWNLOAD, buf);
    }

    ClientToScreen(pData->hListView, &pt);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, pData->hMain, NULL);
    DestroyMenu(hMenu);
}