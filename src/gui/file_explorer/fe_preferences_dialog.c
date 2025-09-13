

#include <globals.h>
#include <gui/file_explorer/fe_remote_operations.h>
#include <gui/file_explorer/fe_preferences_dialog.h>
#include <resource.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <DarkMode.h>
// forward declarations
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND g_hSettingsWnd = NULL;

// ============================================================
// Create the Settings window
// ============================================================
void ShowSettingsDialog(FileExplorerData *pData)
{
    if (g_hSettingsWnd)
    {
        ShowWindow(g_hSettingsWnd, SW_SHOW);
        SetForegroundWindow(g_hSettingsWnd);
        return;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = SettingsWndProc;
    wc.hInstance = g_hinst;
    wc.lpszClassName = L"SettingsWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));
    RegisterClass(&wc);

    g_hSettingsWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"Settings",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 150,
        pData->hMain, NULL, g_hinst, pData);

    ShowWindow(g_hSettingsWnd, SW_SHOW);
    EnableWindow(pData->hMain, FALSE);
    UpdateWindow(g_hSettingsWnd);
}

// ============================================================
// Window Procedure
// ============================================================
LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static FileExplorerData *pData = NULL;

    switch (msg)
    {
    case WM_CREATE:
    {

        CREATESTRUCT *cs = (CREATESTRUCT *)lParam;
        pData = (FileExplorerData *)cs->lpCreateParams;

        if (pData->hMain)
        {
            RECT rcParent, rcChild;
            GetWindowRect(pData->hMain, &rcParent);
            GetWindowRect(hwnd, &rcChild);

            int parentWidth = rcParent.right - rcParent.left;
            int parentHeight = rcParent.bottom - rcParent.top;

            int childWidth = rcChild.right - rcChild.left;
            int childHeight = rcChild.bottom - rcChild.top;

            int x = rcParent.left + (parentWidth - childWidth) / 2;
            int y = rcParent.top + (parentHeight - childHeight) / 2;

            SetWindowPos(hwnd, NULL, x, y, 0, 0,
                         SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        // Checkbox
        HWND hCheckbox = CreateWindowEx(0, L"BUTTON", L"Show hidden files and folders",
                                        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                                        20, 20, 250, 20,
                                        hwnd, (HMENU)IDC_SETTINGS_CHECK_HIDDEN, g_hinst, NULL);

        if (pData->bShowHidden)
        {
            CheckDlgButton(hwnd, IDC_SETTINGS_CHECK_HIDDEN, BST_CHECKED);
        }

        // OK button
        HWND okBtn = CreateWindowEx(0, WC_BUTTONW, L"OK",
                                    WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                    60, 70, 70, 25,
                                    hwnd, (HMENU)IDOK, g_hinst, NULL);

        // Cancel button
        HWND cancelBtn = CreateWindowEx(0, WC_BUTTONW, L"Cancel",
                                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                        150, 70, 70, 25,
                                        hwnd, (HMENU)IDCANCEL, g_hinst, NULL);

        SendMessage(hCheckbox, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        SendMessage(okBtn, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        SendMessage(cancelBtn, WM_SETFONT, (WPARAM)g_hFont, TRUE);

        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
            DarkMode_setWindowMenuBarSubclass(hwnd);
        }

        break;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (pData)
            {
                pData->bShowHidden = (IsDlgButtonChecked(hwnd, IDC_SETTINGS_CHECK_HIDDEN) == BST_CHECKED);
                RequestDirectoryListing(pData, pData->szCurrentPath);
            }
            DestroyWindow(hwnd);
            break;

        case IDCANCEL:
            DestroyWindow(hwnd);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        g_hSettingsWnd = NULL;
        EnableWindow(pData->hMain, TRUE);
        SetForegroundWindow(pData->hMain);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}
