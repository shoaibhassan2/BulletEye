#include <gui/tools/icon_changer_page.h>
#include <globals.h>
#include <resource.h>
#include <helpers/icon_changer.h>
#include <helpers/icon_extractor.h>
#include <commctrl.h>

static void BrowseForFile(HWND hParent, HWND hEdit, const WCHAR *filter, const WCHAR *title)
{
    WCHAR szFileName[MAX_PATH] = L"";
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hParent;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileNameW(&ofn))
    {
        SetWindowTextW(hEdit, szFileName);
    }
}

void CreateIconChangerPage(HWND hParent, RECT tabRect, IconChangerControls *pControls)
{
    WCHAR buffer[256];
    RECT editRect;
    const int PADDING = 15;
    const int LABEL_WIDTH = 100;
    const int EDIT_HEIGHT = 24;
    const int BUTTON_WIDTH = 80;

    int total_height = tabRect.bottom - tabRect.top + 10;
    int changer_group_height = total_height * 0.70;
    int extractor_group_height = total_height * 0.30;
    int changer_y = tabRect.top;
    int extractor_y = changer_y + changer_group_height;

    LoadStringW(g_hinst, IDS_TOOLS_ICON_GROUP, buffer, _countof(buffer));
    pControls->hwndIconGroup = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_GROUPBOX,
                                               tabRect.left, changer_y, tabRect.right - tabRect.left, changer_group_height,
                                               hParent, NULL, g_hinst, NULL);

    int current_y = changer_y + PADDING + 5;
    LoadStringW(g_hinst, IDS_TOOLS_ICON_PATH_LABEL, buffer, _countof(buffer));
    pControls->hwndIconPathLabel = CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | SS_RIGHT,
                                                   tabRect.left + PADDING, current_y + 4, LABEL_WIDTH, EDIT_HEIGHT, hParent, NULL, g_hinst, NULL);
    pControls->hwndIconPathEdit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                                  tabRect.left + PADDING + LABEL_WIDTH + 5, current_y,
                                                  tabRect.right - (tabRect.left + PADDING * 2 + LABEL_WIDTH + 5 + BUTTON_WIDTH), EDIT_HEIGHT, hParent, (HMENU)IDC_TOOLS_EDIT_ICON, g_hinst, NULL);
    LoadStringW(g_hinst, IDS_TOOLS_BROWSE_BUTTON, buffer, _countof(buffer));
    GetWindowRect(pControls->hwndIconPathEdit, &editRect);
    MapWindowPoints(NULL, hParent, (LPPOINT)&editRect, 2);
    pControls->hwndIconBrowseBtn = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_PUSHBUTTON,
                                                   editRect.right + 5, current_y, BUTTON_WIDTH, EDIT_HEIGHT, hParent, (HMENU)IDC_TOOLS_BTN_ICON_BROWSE, g_hinst, NULL);

    current_y += EDIT_HEIGHT + PADDING;
    LoadStringW(g_hinst, IDS_TOOLS_EXE_PATH_LABEL, buffer, _countof(buffer));
    pControls->hwndExePathLabel = CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | SS_RIGHT,
                                                  tabRect.left + PADDING, current_y + 4, LABEL_WIDTH, EDIT_HEIGHT, hParent, NULL, g_hinst, NULL);
    pControls->hwndExePathEdit = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                                                 tabRect.left + PADDING + LABEL_WIDTH + 5, current_y,
                                                 tabRect.right - (tabRect.left + PADDING * 2 + LABEL_WIDTH + 5 + BUTTON_WIDTH), EDIT_HEIGHT, hParent, (HMENU)IDC_TOOLS_EDIT_EXE, g_hinst, NULL);
    LoadStringW(g_hinst, IDS_TOOLS_BROWSE_BUTTON, buffer, _countof(buffer));
    GetWindowRect(pControls->hwndExePathEdit, &editRect);
    MapWindowPoints(NULL, hParent, (LPPOINT)&editRect, 2);
    pControls->hwndExeBrowseBtn = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_PUSHBUTTON,
                                                  editRect.right + 5, current_y, BUTTON_WIDTH, EDIT_HEIGHT, hParent, (HMENU)IDC_TOOLS_BTN_EXE_BROWSE, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_TOOLS_CHANGE_ICON_BUTTON, buffer, _countof(buffer));
    pControls->hwndChangeIconBtn = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_PUSHBUTTON,
                                                   tabRect.left + (tabRect.right - tabRect.left - 120) / 2, changer_y + changer_group_height - EDIT_HEIGHT - PADDING + 6, 120, EDIT_HEIGHT,
                                                   hParent, (HMENU)IDC_TOOLS_BTN_CHANGE_ICON, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_TOOLS_EXTRACT_GROUP, buffer, _countof(buffer));
    pControls->hwndExtractGroup = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_GROUPBOX,
                                                  tabRect.left, extractor_y, tabRect.right - tabRect.left, extractor_group_height,
                                                  hParent, NULL, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_TOOLS_EXTRACT_ICON_BUTTON, buffer, _countof(buffer));
    pControls->hwndExtractIconBtn = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | BS_PUSHBUTTON,
                                                    tabRect.left + (tabRect.right - tabRect.left - 120) / 2, extractor_y + (extractor_group_height - EDIT_HEIGHT) / 2 + 5, 120, EDIT_HEIGHT,
                                                    hParent, (HMENU)IDC_TOOLS_BTN_EXTRACT_ICON, g_hinst, NULL);
}

void ShowIconChangerPage(const IconChangerControls *pControls, BOOL bShow)
{
    int cmd = bShow ? SW_SHOW : SW_HIDE;
    ShowWindow(pControls->hwndIconGroup, cmd);
    ShowWindow(pControls->hwndIconPathLabel, cmd);
    ShowWindow(pControls->hwndIconPathEdit, cmd);
    ShowWindow(pControls->hwndIconBrowseBtn, cmd);
    ShowWindow(pControls->hwndExePathLabel, cmd);
    ShowWindow(pControls->hwndExePathEdit, cmd);
    ShowWindow(pControls->hwndExeBrowseBtn, cmd);
    ShowWindow(pControls->hwndChangeIconBtn, cmd);

    ShowWindow(pControls->hwndExtractGroup, cmd);
    ShowWindow(pControls->hwndExtractIconBtn, cmd);
}

void HandleIconChangerCommand(HWND hParent, WORD commandID, const IconChangerControls *pControls)
{
    switch (commandID)
    {
    case IDC_TOOLS_BTN_ICON_BROWSE:
        BrowseForFile(hParent, pControls->hwndIconPathEdit, L"Icon Files (*.ico)\0*.ico\0All Files (*.*)\0*.*\0", L"Select an Icon File");
        break;

    case IDC_TOOLS_BTN_EXE_BROWSE:
        BrowseForFile(hParent, pControls->hwndExePathEdit, L"Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0", L"Select an Executable File");
        break;

    case IDC_TOOLS_BTN_CHANGE_ICON:
    {
        WCHAR buffer[256];
        WCHAR iconPathW[MAX_PATH], exePathW[MAX_PATH];
        GetWindowTextW(pControls->hwndIconPathEdit, iconPathW, MAX_PATH);
        GetWindowTextW(pControls->hwndExePathEdit, exePathW, MAX_PATH);

        if (wcslen(iconPathW) == 0 || wcslen(exePathW) == 0)
        {
            LoadStringW(g_hinst, IDS_TOOLS_MSG_EMPTY_PATHS, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Input Error", MB_OK | MB_ICONWARNING);
            break;
        }

        char iconPathA[MAX_PATH], exePathA[MAX_PATH];
        WideCharToMultiByte(CP_ACP, 0, iconPathW, -1, iconPathA, MAX_PATH, NULL, NULL);
        WideCharToMultiByte(CP_ACP, 0, exePathW, -1, exePathA, MAX_PATH, NULL, NULL);

        if (change_executable_icon(iconPathA, exePathA))
        {
            LoadStringW(g_hinst, IDS_TOOLS_MSG_ICON_SUCCESS, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Success", MB_OK | MB_ICONINFORMATION);
        }
        else
        {
            LoadStringW(g_hinst, IDS_TOOLS_MSG_ICON_FAIL, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Error", MB_OK | MB_ICONERROR);
        }
        break;
    }
    case IDC_TOOLS_BTN_EXTRACT_ICON:
    {
        WCHAR buffer[256];
        WCHAR srcFile[MAX_PATH] = {0};
        WCHAR dstFile[MAX_PATH] = {0};

        OPENFILENAMEW ofn_src = {0};
        ofn_src.lStructSize = sizeof(ofn_src);
        ofn_src.hwndOwner = hParent;
        ofn_src.lpstrFilter = L"Executable Files (*.exe;*.dll)\0*.exe;*.dll\0All Files\0*.*\0";
        ofn_src.lpstrFile = srcFile;
        ofn_src.nMaxFile = MAX_PATH;
        ofn_src.lpstrTitle = L"Select File to Extract Icon From";
        ofn_src.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (!GetOpenFileNameW(&ofn_src))
            return;

        OPENFILENAMEW ofn_dst = {0};
        ofn_dst.lStructSize = sizeof(ofn_dst);
        ofn_dst.hwndOwner = hParent;
        ofn_dst.lpstrFilter = L"Icon Files (*.ico)\0*.ico\0";
        ofn_dst.lpstrFile = dstFile;
        ofn_dst.nMaxFile = MAX_PATH;
        ofn_dst.lpstrDefExt = L"ico";
        ofn_dst.lpstrTitle = L"Save Icon As...";
        ofn_dst.Flags = OFN_OVERWRITEPROMPT;
        if (!GetSaveFileNameW(&ofn_dst))
            return;

        if (ExtractFirstIcon(srcFile, dstFile))
        {
            LoadStringW(g_hinst, IDS_TOOLS_MSG_EXTRACT_SUCCESS, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Success", MB_OK | MB_ICONINFORMATION);

            SetWindowTextW(pControls->hwndIconPathEdit, dstFile);
        }
        else
        {
            LoadStringW(g_hinst, IDS_TOOLS_MSG_EXTRACT_FAIL, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Error", MB_OK | MB_ICONERROR);
        }
        break;
    }
    }
}