
#include <gui/file_explorer/fe_list_view_display.h>
#include <gui/file_explorer/fe_data_caching.h>
#include <gui/file_explorer/fe_ui_manager.h>
#include <helpers/utils.h>
#include <resource.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <DarkMode.h>
#include <gui/log_page.h>

BOOL CreateListViewDisplay(HWND hParent, FileExplorerData *pData)
{
    pData->hListView = CreateWindowEx(0, WC_LISTVIEWW, NULL,
                                      WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS,
                                      0, 0, 0, 0, hParent, (HMENU)IDC_FE_LISTVIEW, g_hinst, NULL);
    SendMessage(pData->hListView, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    ListView_SetExtendedListViewStyle(pData->hListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    HIMAGELIST hImgListSmall;
    SHFILEINFOW sfi = {0};
    hImgListSmall = (HIMAGELIST)SHGetFileInfoW(
        L"C:\\",
        0,
        &sfi,
        sizeof(sfi),
        SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    if (hImgListSmall)
    {
        ListView_SetImageList(pData->hListView, hImgListSmall, LVSIL_SMALL);
    }
    else
    {
        PostLogMessage(L"FE: Failed to retrieve system image list for ListView.");
    }

    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    WCHAR buf[128];

    LoadStringW(g_hinst, IDS_LV_COL_NAME, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 250;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(pData->hListView, 0, &lvc);
    LoadStringW(g_hinst, IDS_LV_COL_MODIFIED, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 150;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(pData->hListView, 1, &lvc);
    LoadStringW(g_hinst, IDS_LV_COL_TYPE, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 120;
    lvc.fmt = LVCFMT_LEFT;
    ListView_InsertColumn(pData->hListView, 2, &lvc);
    LoadStringW(g_hinst, IDS_LV_COL_SIZE, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 100;
    lvc.fmt = LVCFMT_RIGHT;
    ListView_InsertColumn(pData->hListView, 3, &lvc);

    return TRUE;
}

void UpdateListView(FileExplorerData *pData, cJSON *response)
{
    if (!pData || pData->bShutdown || !IsWindow(pData->hListView))
    {
        if (response)
            cJSON_Delete(response);
        return;
    }

    SendMessageW(pData->hListView, WM_SETREDRAW, FALSE, 0);

    int count = ListView_GetItemCount(pData->hListView);
    for (int i = 0; i < count; i++)
    {
        LVITEMW lvi = {0};
        lvi.mask = LVIF_PARAM;
        lvi.iItem = i;
        ListView_GetItem(pData->hListView, &lvi);
        if (lvi.lParam)
            free((LV_ITEM_DATA *)lvi.lParam);
    }
    ListView_DeleteAllItems(pData->hListView);

    int itemCount = 0;
    if (response)
    {

        AddToCache(pData, pData->szCurrentPath, response);

        cJSON *items = cJSON_GetObjectItem(response, "items");
        if (cJSON_IsArray(items))
        {
            cJSON *entry;
            cJSON_ArrayForEach(entry, items)
            {
                cJSON *nameItem = cJSON_GetObjectItem(entry, "name");
                cJSON *typeItem = cJSON_GetObjectItem(entry, "type");
                cJSON *modifiedItem = cJSON_GetObjectItem(entry, "date_modified");

                if (!cJSON_IsString(nameItem) || !cJSON_IsString(typeItem))
                {
                    PostLogMessage(L"FE: Invalid item entry in JSON response (name/type missing or not string).");
                    continue;
                }

                wchar_t *nameW = ConvertCHARToWCHAR(nameItem->valuestring);
                if (!nameW)
                {
                    PostLogMessage(L"FE: Failed to convert item name to WCHAR.");
                    continue;
                }

                wchar_t *modifiedW = NULL;
                if (modifiedItem && cJSON_IsString(modifiedItem))
                {
                    modifiedW = ConvertCHARToWCHAR(modifiedItem->valuestring);
                }

                LV_ITEM_DATA *pItemData = (LV_ITEM_DATA *)malloc(sizeof(LV_ITEM_DATA));
                if (!pItemData)
                {
                    PostLogMessage(L"FE: Failed to allocate memory for LV_ITEM_DATA.");
                    free(nameW);
                    if (modifiedW)
                        free(modifiedW);
                    continue;
                }
                PathCombineW(pItemData->fullPath, pData->szCurrentPath, nameW);
                pItemData->isDirectory = (strcmp(typeItem->valuestring, "dir") == 0);

                SHFILEINFOW sfi = {0};
                DWORD dwFileAttributes = pItemData->isDirectory ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
                SHGetFileInfoW(
                    nameW,
                    dwFileAttributes,
                    &sfi,
                    sizeof(sfi),
                    SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);

                LVITEMW lvi = {0};
                lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
                lvi.iItem = itemCount++;
                lvi.pszText = nameW;
                lvi.lParam = (LPARAM)pItemData;
                lvi.iImage = sfi.iIcon;

                int idx = ListView_InsertItem(pData->hListView, &lvi);
                if (idx == -1)
                {
                    PostLogMessage(L"FE: Failed to insert ListView item for '%s'.", nameW);
                    free(nameW);
                    if (modifiedW)
                        free(modifiedW);
                    free(pItemData);
                    continue;
                }

                ListView_SetItemText(pData->hListView, idx, 1, modifiedW ? modifiedW : L"");

                if (pItemData->isDirectory)
                {
                    ListView_SetItemText(pData->hListView, idx, 2, L"File Folder");
                }
                else
                {
                    ListView_SetItemText(pData->hListView, idx, 2, sfi.szTypeName);
                }

                if (!pItemData->isDirectory)
                {
                    cJSON *sz = cJSON_GetObjectItem(entry, "size");
                    if (cJSON_IsNumber(sz))
                    {
                        WCHAR sizeStr[64];
                        StrFormatByteSizeW((LONGLONG)sz->valuedouble, sizeStr, ARRAYSIZE(sizeStr));
                        ListView_SetItemText(pData->hListView, idx, 3, sizeStr);
                    }
                    else
                    {
                        ListView_SetItemText(pData->hListView, idx, 3, L"");
                    }
                }

                free(nameW);
                if (modifiedW)
                    free(modifiedW);
            }
        }
        cJSON_Delete(response);
    }
    else
    {
        PostLogMessage(L"FE: No valid JSON response for ListView update or response is NULL.");
    }

    SendMessageW(pData->hListView, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(pData->hListView, NULL, TRUE);

    WCHAR statusText[256], fmt[128];
    LoadStringW(g_hinst, IDS_FE_STATUS_ITEMS_FMT, fmt, _countof(fmt));
    swprintf_s(statusText, ARRAYSIZE(statusText), fmt, pData->szCurrentPath, itemCount);
    SendMessageW(pData->hStatusBar, SB_SETTEXTW, 0, (LPARAM)statusText);

    SetExplorerControlsEnabled(pData, TRUE);
}