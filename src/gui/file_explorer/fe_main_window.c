
#define _WIN32_IE 0x0600

#include <gui/file_explorer/fe_main_window.h>
#include <gui/file_explorer/fe_ui_manager.h>
#include <gui/file_explorer/fe_tree_view_events.h>
#include <gui/file_explorer/fe_list_view_events.h>
#include <gui/file_explorer/fe_command_router.h>
#include <gui/file_explorer/fe_path_navigator.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <gui/file_explorer/fe_data_caching.h>
#include <gui/file_explorer/fe_custom_messages.h>
#include <gui/file_explorer/fe_toolbar_view.h>
#include <gui/file_explorer/fe_tree_view_display.h>
#include <gui/file_explorer/fe_list_view_display.h>
#include <DarkMode.h>
#include <resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <Shlwapi.h>
#include <gui/log_page.h>

void FreeTreeViewItemData(HWND hTree, HTREEITEM hItem)
{
    if (hItem == NULL)
        return;

    TVITEMW tvi = {0};
    tvi.mask = TVIF_PARAM;
    tvi.hItem = hItem;

    if (TreeView_GetItem(hTree, &tvi) && tvi.lParam)
    {
        free((void *)tvi.lParam);
    }

    HTREEITEM hChild = TreeView_GetChild(hTree, hItem);
    while (hChild != NULL)
    {
        HTREEITEM hNextSibling = TreeView_GetNextSibling(hTree, hChild);
        FreeTreeViewItemData(hTree, hChild);
        hChild = hNextSibling;
    }
}

void CreateFileExplorerWindow(HWND hParent, int clientIndex)
{

    EnterCriticalSection(&g_cs_clients);

    if (clientIndex >= 0 && clientIndex < MAX_CLIENTS && g_active_clients[clientIndex].isActive && g_active_clients[clientIndex].hExplorerWnd != NULL && IsWindow(g_active_clients[clientIndex].hExplorerWnd))
    {
        SetForegroundWindow(g_active_clients[clientIndex].hExplorerWnd);
        LeaveCriticalSection(&g_cs_clients);
        return;
    }
    LeaveCriticalSection(&g_cs_clients);

    WCHAR className[128];
    LoadStringW(g_hinst, IDS_FILE_EXPLORER_CLASSNAME, className, _countof(className));

    static BOOL isClassRegistered = FALSE;
    if (!isClassRegistered)
    {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = FileExplorerProc;
        wc.hInstance = g_hinst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = className;
        wc.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));
        if (!RegisterClassExW(&wc))
        {
            WCHAR msg[256], cap[64];
            LoadStringW(g_hinst, IDS_ERR_REGISTER_FAIL_FILE_EXPLORER, msg, _countof(msg));
            LoadStringW(g_hinst, IDS_ERR_CAPTION, cap, _countof(cap));
            MessageBoxW(g_hWnd, msg, cap, MB_ICONERROR);
            return;
        }
        isClassRegistered = TRUE;
    }

    FileExplorerData *pData = (FileExplorerData *)calloc(1, sizeof(FileExplorerData));
    if (!pData)
        return;

    pData->clientIndex = clientIndex;
    pData->iBackStackTop = -1;
    pData->iForwardStackTop = -1;
    InitializeCriticalSection(&pData->csCache);

    WCHAR windowTitle[256], titleFormat[128];
    LoadStringW(g_hinst, IDS_FILE_EXPLORER_CAPTION, titleFormat, _countof(titleFormat));

    EnterCriticalSection(&g_cs_clients);

    if (clientIndex >= 0 && clientIndex < MAX_CLIENTS && g_active_clients[clientIndex].isActive)
    {
        swprintf_s(windowTitle, _countof(windowTitle), L"%s - %hs", titleFormat, g_active_clients[clientIndex].info.ipAddress);
    }
    else
    {

#ifndef __TINYC__
        wcscpy_s(windowTitle, _countof(windowTitle), titleFormat);
#else
        snwprintf(windowTitle, sizeof(windowTitle) / sizeof(wchar_t), L"%ls", titleFormat);
#endif
    }
    LeaveCriticalSection(&g_cs_clients);

    HWND hExplorer;

    hExplorer = CreateWindowExW(WS_EX_ACCEPTFILES, className, windowTitle,
                                WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                                CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
                                g_hWnd, NULL, g_hinst, (LPVOID)pData);

    if (hExplorer)
    {

        EnterCriticalSection(&g_cs_clients);

        if (clientIndex >= 0 && clientIndex < MAX_CLIENTS && g_active_clients[clientIndex].isActive)
        {
            g_active_clients[clientIndex].hExplorerWnd = hExplorer;
        }
        LeaveCriticalSection(&g_cs_clients);

        EnableWindow(g_hWnd, FALSE);
        ShowWindow(hExplorer, SW_SHOW);
    }
    else
    {

        DeleteCriticalSection(&pData->csCache);
        free(pData);
        EnableWindow(g_hWnd, TRUE);
    }
}

LRESULT CALLBACK FileExplorerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FileExplorerData *pData = (FileExplorerData *)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message)
    {
    case WM_CREATE:
    {
        if (g_hWnd)
        {
            RECT rcParent, rcChild;
            GetWindowRect(g_hWnd, &rcParent);
            GetWindowRect(hWnd, &rcChild);

            int parentWidth = rcParent.right - rcParent.left;
            int parentHeight = rcParent.bottom - rcParent.top;

            int childWidth = rcChild.right - rcChild.left;
            int childHeight = rcChild.bottom - rcChild.top;

            int x = rcParent.left + (parentWidth - childWidth) / 2;
            int y = rcParent.top + (parentHeight - childHeight) / 2;

            SetWindowPos(hWnd, NULL, x, y, 0, 0,
                         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
        pData = (FileExplorerData *)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pData);
        pData->hMain = hWnd;

        if (!CreateExplorerControls(hWnd, pData))
        {
            DestroyWindow(hWnd);
            return -1;
        }
        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hWnd);
            DarkMode_setWindowEraseBgSubclass(hWnd);
            DarkMode_setWindowMenuBarSubclass(hWnd);
        }

        RequestDrives(pData);
        UpdateNavButtonsState(pData);
        break;
    }

    case WM_SIZE:
        if (pData)
            ResizeExplorerControls(hWnd);
        break;
    case WM_DROPFILES:
        if (pData)
            OnFileDrop(pData, wParam);
        break;
    case WM_NOTIFY:
    {
        if (!pData)
            break;
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        if (lpnmh->hwndFrom == pData->hTreeView)
        {

            if (lpnmh->code == TVN_SELCHANGEDW)
                OnTreeViewSelChanged(pData, lParam);
        }
        else if (lpnmh->hwndFrom == pData->hListView)
        {
            if (lpnmh->code == NM_DBLCLK)
                OnListViewDblClick(pData, lParam);
            else if (lpnmh->code == LVN_ENDLABELEDITW)
                OnListViewEndLabelEdit(pData, lParam);

            else if (lpnmh->code == NM_RCLICK)
            {
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(pData->hListView, &pt);
                ShowExplorerContextMenu(pData, pt);
            }
        }
        break;
    }
    case WM_CONTEXTMENU:
    {
    }
    break;
    case WM_COMMAND:
        if (pData)
            OnContextMenuCommand(pData, LOWORD(wParam));
        break;
    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);

        if (DarkMode_isEnabled())
        {
            FillRect(hdc, &rc, DarkMode_getBackgroundBrush());
            return 1;
        }
        else
        {

            FillRect(hdc, &rc, GetSysColorBrush(COLOR_WINDOW));
            return 1;
        }
    }
    case WM_CLOSE:
    {

        EnableWindow(g_hWnd, TRUE);
        SetForegroundWindow(g_hWnd);
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY:
    {

        EnableWindow(g_hWnd, TRUE);
        SetForegroundWindow(g_hWnd);

        if (pData)
        {
            pData->bShutdown = TRUE;

            Sleep(100);

            EnterCriticalSection(&g_cs_clients);

            if (pData->clientIndex >= 0 && pData->clientIndex < MAX_CLIENTS && g_active_clients[pData->clientIndex].isActive && g_active_clients[pData->clientIndex].hExplorerWnd == hWnd)
            {
                g_active_clients[pData->clientIndex].hExplorerWnd = NULL;
            }
            LeaveCriticalSection(&g_cs_clients);

            HIMAGELIST hImgList = (HIMAGELIST)GetWindowLongPtr(pData->hToolBar, GWLP_USERDATA);
            if (hImgList)
            {
                ImageList_Destroy(hImgList);
                SetWindowLongPtr(pData->hToolBar, GWLP_USERDATA, (LONG_PTR)NULL);
            }

            int count = ListView_GetItemCount(pData->hListView);
            for (int i = 0; i < count; i++)
            {
                LVITEMW lvi = {0};
                lvi.mask = LVIF_PARAM;
                lvi.iItem = i;
                if (ListView_GetItem(pData->hListView, &lvi) && lvi.lParam)
                {
                    free((LV_ITEM_DATA *)lvi.lParam);
                }
            }
            ListView_DeleteAllItems(pData->hListView);

            HTREEITEM hRoot = TreeView_GetRoot(pData->hTreeView);
            while (hRoot)
            {
                HTREEITEM hNextRoot = TreeView_GetNextSibling(pData->hTreeView, hRoot);
                FreeTreeViewItemData(pData->hTreeView, hRoot);
                hRoot = hNextRoot;
            }
            TreeView_DeleteAllItems(pData->hTreeView);

            ClearCache(pData);
            DeleteCriticalSection(&pData->csCache);

            free(pData);

            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)NULL);
        }
        break;
    }
    case WM_APP_FE_CLIENT_DISCONNECTED:
    {
        MessageBoxW(hWnd, L"The client has disconnected. This file explorer window will now close.", L"Connection Lost", MB_OK | MB_ICONINFORMATION);
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return 0;
    }
    case WM_APP_FE_UPDATE_DRIVES:
        if (pData)
            UpdateDrivesInTreeView(pData, (cJSON *)lParam);
        else if (lParam)
            cJSON_Delete((cJSON *)lParam);
        PostLogMessage(L"FE: Drives update complete. Re-enabling controls.");

        break;
    case WM_APP_FE_UPDATE_LISTVIEW:
        if (pData)
            UpdateListView(pData, (cJSON *)lParam);
        else if (lParam)
            cJSON_Delete((cJSON *)lParam);

        PostLogMessage(L"FE: ListView update complete. Re-enabling controls.");
        break;
    case WM_APP_FE_UPDATE_TREE_CHILDREN:
    {

        PostLogMessage(L"FE: WM_APP_FE_UPDATE_TREE_CHILDREN received (should not happen if TreeView expansion is disabled).");
        if (lParam)
        {
            cJSON *data = (cJSON *)lParam;
            cJSON *items = cJSON_GetObjectItem(data, "items");
            if (items)
                cJSON_Delete(items);
            cJSON_Delete(data);
        }

        if (pData)
            SetExplorerControlsEnabled(pData, TRUE);
        break;
    }
    case WM_APP_FE_REFRESH_VIEW:
        if (pData)
            RequestDirectoryListing(pData, pData->szCurrentPath);

        PostLogMessage(L"FE: Refresh view initiated. Controls will re-enable after listing.");
        break;
    case WM_APP_FE_TRANSFER_PROGRESS:
    {
        FileTransferProgressInfo *progress = (FileTransferProgressInfo *)lParam;
        if (pData && progress)
        {
            WCHAR statusText[512], fmt[128];
            double percentage = (double)progress->current * 100.0 / progress->total;
            UINT fmt_id = progress->isUpload ? IDS_FE_STATUS_UPLOADING_FMT : IDS_FE_STATUS_DOWNLOADING_FMT;
            LoadStringW(g_hinst, fmt_id, fmt, _countof(fmt));
            swprintf_s(statusText, ARRAYSIZE(statusText), fmt,
                       progress->fileName, progress->current, progress->total, percentage);
            SendMessageW(pData->hStatusBar, SB_SETTEXTW, 0, (LPARAM)statusText);
        }
        if (progress)
            free(progress);
        return 0;
    }
    case WM_APP_FE_TRANSFER_COMPLETE:
    {
        FileTransferProgressInfo *progress = (FileTransferProgressInfo *)lParam;
        if (pData && progress)
        {
            WCHAR statusText[256], fmt[128];
            UINT fmt_id;
            if (progress->total > 0)
            {
                fmt_id = progress->isUpload ? IDS_FE_STATUS_UPLOAD_COMPLETE_FMT : IDS_FE_STATUS_DOWNLOAD_COMPLETE_FMT;
                LoadStringW(g_hinst, fmt_id, fmt, _countof(fmt));
                swprintf_s(statusText, ARRAYSIZE(statusText), fmt, progress->fileName);
            }
            else
            {
                fmt_id = IDS_FE_STATUS_TRANSFER_FAIL_FMT;
                LoadStringW(g_hinst, fmt_id, fmt, _countof(fmt));
                swprintf_s(statusText, ARRAYSIZE(statusText), fmt, progress->fileName);
            }
            SendMessageW(pData->hStatusBar, SB_SETTEXTW, 0, (LPARAM)statusText);

            if (progress->isUpload)
            {
                RemoveFromCache(pData, pData->szCurrentPath);

                RequestDirectoryListing(pData, pData->szCurrentPath);
                PostLogMessage(L"FE: Upload complete. Refreshing listing. Controls will re-enable after listing.");
            }
            else
            {

                SetExplorerControlsEnabled(pData, TRUE);

                if (pData->szCurrentPath[0] != L'\0')
                {
                    RequestDirectoryListing(pData, pData->szCurrentPath);
                }
                PostLogMessage(L"FE: Download complete. Re-enabling controls and refreshing status.");
            }
        }
        else
        {
            PostLogMessage(L"FE: Transfer complete message received with NULL progress data.");
            SetExplorerControlsEnabled(pData, TRUE);

            if (pData && pData->szCurrentPath[0] != L'\0')
            {
                RequestDirectoryListing(pData, pData->szCurrentPath);
            }
        }
        if (progress)
            free(progress);
        return 0;
    }
    case WM_APP_FE_COMMAND_COMPLETE:
    {
        if (pData)
        {
            PostLogMessage(L"FE: Generic command complete. Re-enabling controls.");
            SetExplorerControlsEnabled(pData, TRUE);

            if (pData->szCurrentPath[0] != L'\0')
            {
                RequestDirectoryListing(pData, pData->szCurrentPath);
            }
        }
        else
        {
            PostLogMessage(L"FE: Generic command complete message received with NULL pData.");
        }
        return 0;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}