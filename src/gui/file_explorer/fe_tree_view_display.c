
#include <gui/file_explorer/fe_tree_view_display.h>
#include <gui/file_explorer/fe_ui_manager.h>
#include <helpers/utils.h>
#include <resource.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <DarkMode.h>
#include <gui/log_page.h>
#include <shlwapi.h>

BOOL CreateTreeViewDisplay(HWND hParent, FileExplorerData *pData)
{
    pData->hTreeView = CreateWindowEx(0, WC_TREEVIEWW, NULL,
                                      WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
                                      0, 0, 0, 0, hParent, (HMENU)IDC_FE_TREEVIEW, g_hinst, NULL);
    SendMessage(pData->hTreeView, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    return TRUE;
}

void UpdateDrivesInTreeView(FileExplorerData *pData, cJSON *drivesArray)
{
    if (!pData || pData->bShutdown || !IsWindow(pData->hTreeView))
    {
        if (drivesArray)
            cJSON_Delete(drivesArray);
        return;
    }

    if (!drivesArray || !cJSON_IsArray(drivesArray))
    {
        PostLogMessage(L"FE: Drives update received invalid or NULL array.");
        if (drivesArray)
            cJSON_Delete(drivesArray);
        SetExplorerControlsEnabled(pData, TRUE);
        return;
    }

    HTREEITEM hRoot = TreeView_GetRoot(pData->hTreeView);
    while (hRoot)
    {
        HTREEITEM hNextRoot = TreeView_GetNextSibling(pData->hTreeView, hRoot);
        FreeTreeViewItemData(pData->hTreeView, hRoot);
        hRoot = hNextRoot;
    }
    TreeView_DeleteAllItems(pData->hTreeView);
    PostLogMessage(L"FE: Cleared existing TreeView items before updating drives.");

    cJSON *item;
    cJSON_ArrayForEach(item, drivesArray)
    {
        if (!cJSON_IsString(item))
        {
            PostLogMessage(L"FE: Invalid drive item in JSON response (not a string).");
            continue;
        }
        wchar_t *driveW = ConvertCHARToWCHAR(item->valuestring);
        if (!driveW)
        {
            PostLogMessage(L"FE: Failed to convert drive string to WCHAR.");
            continue;
        }

        TVINSERTSTRUCTW tvs = {0};
        tvs.hParent = TVI_ROOT;
        tvs.hInsertAfter = TVI_SORT;
        tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
        tvs.item.pszText = driveW;
        tvs.item.lParam = (LPARAM)_wcsdup(driveW);
        if (!tvs.item.lParam)
        {
            PostLogMessage(L"FE: Failed to _wcsdup path '%ls' for drive. Skipping.", driveW);
            free(driveW);
            continue;
        }
        tvs.item.cChildren = 0;

        if (TreeView_InsertItem(pData->hTreeView, &tvs) == NULL)
        {
            PostLogMessage(L"FE: Failed to insert drive '%s' into TreeView.", driveW);
            free((void *)tvs.item.lParam);
        }
        free(driveW);
    }
    cJSON_Delete(drivesArray);

    SetExplorerControlsEnabled(pData, TRUE);
}

void UpdateTreeChildren(FileExplorerData *pData, HTREEITEM hParent, cJSON *itemsArray)
{

    PostLogMessage(L"FE: UpdateTreeChildren called (Treeview expansion is disabled). Cleaning up JSON.");
    if (itemsArray)
        cJSON_Delete(itemsArray);

    if (pData)
        SetExplorerControlsEnabled(pData, TRUE);
}