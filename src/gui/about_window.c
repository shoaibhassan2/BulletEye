#include <globals.h>
#include <gui/about_window.h>
#include <resource.h>
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <gui/tooltip.h>
#include <DarkMode.h>

HWND g_hAboutWnd = NULL;

LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HFONT hFontBold = NULL;
    static HBITMAP hBmpGithub = NULL;
    static HBITMAP hBmpTelegram = NULL;
    static HWND hTooltip = NULL;

    switch (msg)
    {
    case WM_CREATE:
    {
        hTooltip = CreateTooltip(hwnd);
        WCHAR buffer[1024];

        hFontBold = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Segoe UI");

        hBmpGithub = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_GITHUB));
        hBmpTelegram = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_TELEGRAM));

        const int MARGIN = 15;
        const int PADDING = 10;
        const int WINDOW_WIDTH = 380;
        int current_y = MARGIN;

        HWND hCtrl;

        hCtrl = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_ICON, MARGIN, current_y + 4, 21, 20, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessageW(hCtrl, STM_SETICON, (WPARAM)LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN)), 0);
        LoadStringW(g_hinst, IDS_ABOUT_TITLE, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, MARGIN + 40, current_y + 2, 200, 20, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)hFontBold, TRUE);
        LoadStringW(g_hinst, IDS_ABOUT_AUTHOR, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, MARGIN + 40, current_y + 22, 250, 20, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        current_y += 55;

        hCtrl = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, MARGIN + 8, current_y + 6, 32, 32, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessage(hCtrl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpGithub);
        LoadStringW(g_hinst, IDS_ABOUT_SYSLINK_GITHUB, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"SysLink", buffer, WS_CHILD | WS_VISIBLE, MARGIN + 40, current_y + 8, 280, 20, hwnd, (HMENU)IDC_ABOUT_SYSLINK_GITHUB, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        current_y += 40;

        hCtrl = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP, MARGIN + 8, current_y - 2, 32, 32, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessage(hCtrl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpTelegram);
        LoadStringW(g_hinst, IDS_ABOUT_SYSLINK_TELEGRAM, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"SysLink", buffer, WS_CHILD | WS_VISIBLE, MARGIN + 40, current_y - 4, 280, 20, hwnd, (HMENU)IDC_ABOUT_SYSLINK_TELEGRAM, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        current_y += 40;

        LoadStringW(g_hinst, IDS_ABOUT_GROUPBOX_TITLE, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, MARGIN, current_y, WINDOW_WIDTH - (2 * MARGIN), 130, hwnd, (HMENU)-1, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        LoadStringW(g_hinst, IDS_ABOUT_DISCLAIMER_TEXT, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"EDIT", buffer, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY, MARGIN + PADDING, current_y + 20, WINDOW_WIDTH - (2 * MARGIN) - (2 * PADDING), 100, hwnd, (HMENU)IDC_ABOUT_DISCLAIMER_EDIT, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        current_y += 138;

        LoadStringW(g_hinst, IDS_ABOUT_OK_BUTTON, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, (WINDOW_WIDTH - 80) / 2, current_y, 80, 25, hwnd, (HMENU)ID_ABOUT_OK_BTN, g_hinst, NULL);
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_ABOUT_OK);
        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
        }
        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR pnmh = (LPNMHDR)lParam;
        if (pnmh->code == NM_CLICK)
        {
            if (pnmh->idFrom == IDC_ABOUT_SYSLINK_GITHUB)
            {
                ShellExecuteW(hwnd, L"open", L"https://github.com/shoaibhassan2", NULL, NULL, SW_SHOWNORMAL);
            }
            else if (pnmh->idFrom == IDC_ABOUT_SYSLINK_TELEGRAM)
            {
                ShellExecuteW(hwnd, L"open", L"https://t.me/ReverserPython", NULL, NULL, SW_SHOWNORMAL);
            }
        }
        break;
    }

    case WM_COMMAND:
    {

        if (LOWORD(wParam) == ID_ABOUT_OK_BTN)
        {
            DestroyWindow(hwnd);
            EnableWindow(g_hWnd, TRUE);
            SetForegroundWindow(g_hWnd);
        }
        break;
    }
    case WM_ERASEBKGND:
    {
        if (DarkMode_isEnabled())
        {
            HDC hdc = (HDC)wParam;
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            FillRect(hdc, &clientRect, DarkMode_getBackgroundBrush());
            return 1;
        }
        else
        {
            break;
        }
    }
    case WM_CTLCOLORSTATIC:
    {
        if (DarkMode_isEnabled())
        {
            break;
        }
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)GetStockObject(COLOR_BTNFACE);
    }
    case WM_CLOSE:
    {
        DestroyWindow(hwnd);
        EnableWindow(g_hWnd, TRUE);
        SetForegroundWindow(g_hWnd);
        break;
    }

    case WM_DESTROY:
    {
        DeleteObject(hFontBold);
        DeleteObject(hBmpGithub);
        DeleteObject(hBmpTelegram);
        EnableWindow(g_hWnd, TRUE);
        SetForegroundWindow(g_hWnd);
        g_hAboutWnd = NULL;
        break;
    }

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateAboutWindow(HWND hParent)
{
    if (g_hAboutWnd && IsWindow(g_hAboutWnd))
    {
        SetForegroundWindow(g_hAboutWnd);
        return;
    }

    WCHAR buffer[256];
    static BOOL isClassRegistered = FALSE;
    if (!isClassRegistered)
    {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = AboutWndProc;
        wc.hInstance = g_hinst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        LoadStringW(g_hinst, IDS_ABOUT_CLASSNAME, buffer, _countof(buffer));
        wc.lpszClassName = buffer;
        wc.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));
        wc.hIconSm = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));
        if (!RegisterClassExW(&wc))
        {
            LoadStringW(g_hinst, IDS_ERR_REGISTER_FAIL_ABOUT, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        isClassRegistered = TRUE;
    }

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    const int width = 380;
    const int height = 350;
    int x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    WCHAR caption[256];
    LoadStringW(g_hinst, IDS_ABOUT_CAPTION, caption, _countof(caption));
    WCHAR className[256];
    LoadStringW(g_hinst, IDS_ABOUT_CLASSNAME, className, _countof(className));

    g_hAboutWnd = CreateWindowExW(WS_EX_DLGMODALFRAME, className, caption, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, x, y, width, height, hParent, NULL, g_hinst, NULL);

    if (g_hAboutWnd)
    {
        ShowWindow(g_hAboutWnd, SW_SHOW);
        UpdateWindow(g_hAboutWnd);
    }
}