#include <gui/file_explorer/fe_command_router.h>
#include <gui/file_explorer/fe_list_view_events.h>
#include <gui/file_explorer/fe_path_navigator.h>
#include <gui/file_explorer/fe_preferences_dialog.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <resource.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include <wchar.h>
#include <gui/file_explorer/fe_custom_messages.h>

void OnContextMenuCommand(FileExplorerData *pData, int id)
{

    switch (id)
    {
    case IDC_FE_NAV_BACK:
        OnNavBack(pData);
        return;
    case IDC_FE_NAV_FORWARD:
        OnNavForward(pData);
        return;
    case IDC_FE_SHOW_SETTINGS:
        ShowSettingsDialog(pData);
        return;
    case IDM_FE_VIEW_REFRESH:
        RequestDirectoryListing(pData, pData->szCurrentPath);
        return;
    case IDM_FE_FILE_EXIT:
        DestroyWindow(pData->hMain);
        return;
    case IDM_FE_FILE_UPLOAD:
    {

        WCHAR szLocalPath[MAX_PATH] = {0};
        OPENFILENAMEW ofn = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = pData->hMain;
        ofn.lpstrFile = szLocalPath;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_EXPLORER;

        if (GetOpenFileNameW(&ofn))
        {
            RequestFileUpload(pData, szLocalPath, pData->szCurrentPath);
        }
        return;
    }
    case IDM_FE_FILE_UPLOAD_DRAG_DROP:
    {

        return;
    }
    case IDM_FE_HELP_ABOUT:
    {
        WCHAR text[128], caption[128];
        LoadStringW(g_hinst, IDS_FE_ABOUT_TEXT, text, 128);
        LoadStringW(g_hinst, IDS_FE_ABOUT_CAPTION, caption, 128);
        MessageBoxW(pData->hMain, text, caption, MB_OK | MB_ICONINFORMATION);
        return;
    }
    }

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

    WCHAR msg[256], cap[64];
    switch (id)
    {
    case IDM_FE_CONTEXT_OPEN:

        if (pItemData->isDirectory)
        {
            NavigateTo(pData, pItemData->fullPath, TRUE);
        }
        else
        {
            WCHAR execMsg[256], execCap[64];
            LoadStringW(g_hinst, IDS_CONFIRM_EXEC_TEXT, execMsg, _countof(execMsg));
            LoadStringW(g_hinst, IDS_CONFIRM_EXEC_CAPTION, execCap, _countof(execCap));
            if (MessageBoxW(pData->hMain, execMsg, execCap, MB_YESNO | MB_ICONWARNING) == IDYES)
            {
                RequestFileExecution(pData, pItemData->fullPath);
            }
        }
        break;
    case IDM_FE_CONTEXT_DELETE:
        LoadStringW(g_hinst, IDS_CONFIRM_DELETE_TEXT, msg, _countof(msg));
        LoadStringW(g_hinst, IDS_CONFIRM_DELETE_CAPTION, cap, _countof(cap));
        if (MessageBoxW(pData->hMain, msg, cap, MB_YESNO | MB_ICONWARNING) == IDYES)
        {
            RequestFileDeletion(pData, pItemData->fullPath);
        }
        break;
    case IDM_FE_CONTEXT_RENAME:
        ListView_EditLabel(pData->hListView, iItem);
        break;
    case IDM_FE_CONTEXT_DOWNLOAD:
    {
        if (pItemData->isDirectory)
            break;

        WCHAR szLocalDestPath[MAX_PATH] = {0};
        OPENFILENAMEW ofn = {0};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = pData->hMain;
        ofn.lpstrFile = szLocalDestPath;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = L"All Files (*.*)\0*.*\0";
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_EXPLORER;

        LPCWSTR remoteFileName = PathFindFileNameW(pItemData->fullPath);

        WCHAR initialDir[MAX_PATH] = {0};
        if (SHGetSpecialFolderPathW(pData->hMain, initialDir, CSIDL_MYDOCUMENTS, FALSE))
        {
            ofn.lpstrInitialDir = initialDir;
        }

        if (remoteFileName)
        {
#ifndef __TINYC__
            wcscpy_s(szLocalDestPath, MAX_PATH, remoteFileName);
#else
            snwprintf(szLocalDestPath, MAX_PATH, L"%ls", remoteFileName);
#endif
        }

        if (GetSaveFileNameW(&ofn))
        {
            RequestFileDownload(pData, pItemData->fullPath, szLocalDestPath);
        }
        break;
    }
    case IDM_FE_CONTEXT_ENCRYPT:
    {
        LoadStringW(g_hinst, IDS_CONFIRM_ENCRYPT_TEXT, msg, _countof(msg));
        LoadStringW(g_hinst, IDS_CONFIRM_ENCRYPT_CAPTION, cap, _countof(cap));
        if (MessageBoxW(pData->hMain, msg, cap, MB_YESNO | MB_ICONWARNING) == IDYES)
        {
            RequestFileEncryption(pData, pItemData->fullPath);
        }
        break;
    }
    case IDM_FE_CONTEXT_DECRYPT:
    {
        LoadStringW(g_hinst, IDS_CONFIRM_DECRYPT_TEXT, msg, _countof(msg));
        LoadStringW(g_hinst, IDS_CONFIRM_DECRYPT_CAPTION, cap, _countof(cap));
        if (MessageBoxW(pData->hMain, msg, cap, MB_YESNO | MB_ICONWARNING) == IDYES)
        {
            RequestFileDecryption(pData, pItemData->fullPath);
        }
        break;
    }
    }
}