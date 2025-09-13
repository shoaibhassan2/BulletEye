#include <gui/network_settings_window.h>
#include <globals.h>
#include <resource.h>
#include <gui/log_page.h>
#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <gui/tooltip.h>
#include <DarkMode.h>

HWND g_hNetSettingsWnd = NULL;

void UpdateBandwidthControls(HWND hParent, BOOL bEnable)
{
    EnableWindow(GetDlgItem(hParent, IDC_NET_STATIC_UPLOAD), bEnable);
    EnableWindow(GetDlgItem(hParent, IDC_NET_EDIT_UPLOAD), bEnable);
    EnableWindow(GetDlgItem(hParent, IDC_NET_STATIC_DOWNLOAD), bEnable);
    EnableWindow(GetDlgItem(hParent, IDC_NET_EDIT_DOWNLOAD), bEnable);
}

void LoadSettingsToWindow(HWND hwnd)
{
    SetDlgItemInt(hwnd, IDC_NET_EDIT_MAX_CONN, g_networkSettings.max_connections, FALSE);
    SetDlgItemInt(hwnd, IDC_NET_EDIT_TIMEOUT, g_networkSettings.connection_timeout, FALSE);
    SetDlgItemInt(hwnd, IDC_NET_EDIT_SENDBUFF, g_networkSettings.send_buffer_kb, FALSE);
    SetDlgItemInt(hwnd, IDC_NET_EDIT_RECVBUFF, g_networkSettings.recv_buffer_kb, FALSE);
    SetDlgItemInt(hwnd, IDC_NET_EDIT_UPLOAD, g_networkSettings.max_upload_kbps, FALSE);
    SetDlgItemInt(hwnd, IDC_NET_EDIT_DOWNLOAD, g_networkSettings.max_download_kbps, FALSE);
    UpdateBandwidthControls(hwnd, g_networkSettings.limit_bandwidth);
}

void SaveSettingsFromWindow(HWND hwnd)
{
    g_networkSettings.max_connections = GetDlgItemInt(hwnd, IDC_NET_EDIT_MAX_CONN, NULL, FALSE);
    g_networkSettings.connection_timeout = GetDlgItemInt(hwnd, IDC_NET_EDIT_TIMEOUT, NULL, FALSE);
    g_networkSettings.send_buffer_kb = GetDlgItemInt(hwnd, IDC_NET_EDIT_SENDBUFF, NULL, FALSE);
    g_networkSettings.recv_buffer_kb = GetDlgItemInt(hwnd, IDC_NET_EDIT_RECVBUFF, NULL, FALSE);
    g_networkSettings.max_upload_kbps = GetDlgItemInt(hwnd, IDC_NET_EDIT_UPLOAD, NULL, FALSE);
    g_networkSettings.max_download_kbps = GetDlgItemInt(hwnd, IDC_NET_EDIT_DOWNLOAD, NULL, FALSE);

    WCHAR logBuffer[256];
    swprintf_s(logBuffer, _countof(logBuffer), L"Network settings saved. Max Conns: %d", g_networkSettings.max_connections);
    AddLogMessage(logBuffer);
}

BOOL CALLBACK SetChildFontProc(HWND hChild, LPARAM lParam)
{
    HFONT hFont = (HFONT)lParam;
    SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, TRUE);
    return TRUE;
}

LRESULT CALLBACK NetworkSettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hBmpToggleOn = NULL, hBmpToggleOff = NULL;
    static HWND hTooltip = NULL;

    switch (msg)
    {
    case WM_CREATE:
    {
        hTooltip = CreateTooltip(hwnd);
        WCHAR buffer[256];
        hBmpToggleOn = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_TOGGLE_ON));
        hBmpToggleOff = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_TOGGLE_OFF));

        const int MARGIN = 10, PADDING = 15;
        const int LABEL_WIDTH = 140, EDIT_WIDTH = 70, CONTROL_HEIGHT = 23;
        const int GROUP_WIDTH = 264;
        const int EDIT_X = MARGIN + PADDING + LABEL_WIDTH + 20;
        const int LABEL_X = MARGIN + PADDING;
        const int ROW_SPACING = 28;
        int y_pos = MARGIN;
        HWND hCtrl, hCtrlUpDown, hCtrlToggle;

        LoadStringW(g_hinst, IDS_NET_GROUP_CONN, buffer, _countof(buffer));
        CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, MARGIN, y_pos, GROUP_WIDTH, 70, hwnd, NULL, g_hinst, NULL);
        LoadStringW(g_hinst, IDS_NET_LABEL_MAX_CONN, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + 3, LABEL_WIDTH, 16, hwnd, NULL, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_MAX_CONN, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_MAXCONN);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(256, 1));
        LoadStringW(g_hinst, IDS_NET_LABEL_TIMEOUT, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + ROW_SPACING + 3, LABEL_WIDTH, 16, hwnd, NULL, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING + ROW_SPACING, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_TIMEOUT, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_TIMEOUT);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(120, 10));
        y_pos += 65 + MARGIN;

        LoadStringW(g_hinst, IDS_NET_GROUP_PERF, buffer, _countof(buffer));
        CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, MARGIN, y_pos, GROUP_WIDTH, 70, hwnd, NULL, g_hinst, NULL);
        LoadStringW(g_hinst, IDS_NET_LABEL_SENDBUFF, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + 3, LABEL_WIDTH, 16, hwnd, NULL, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_SENDBUFF, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_SENDBUFF);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(10, 1));
        LoadStringW(g_hinst, IDS_NET_LABEL_RECVBUFF, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + ROW_SPACING + 3, LABEL_WIDTH, 16, hwnd, NULL, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING + ROW_SPACING, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_RECVBUFF, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_RECVBUFF);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(10, 1));
        y_pos += 65 + MARGIN;

        LoadStringW(g_hinst, IDS_NET_GROUP_THROTTLE, buffer, _countof(buffer));
        CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, MARGIN, y_pos, GROUP_WIDTH, 110, hwnd, NULL, g_hinst, NULL);
        LoadStringW(g_hinst, IDS_NET_LABEL_THROTTLE, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X + 50, y_pos + PADDING + 8, LABEL_WIDTH + 50, 16, hwnd, NULL, g_hinst, NULL);
        hCtrlToggle = CreateWindowExW(0, L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, LABEL_X, y_pos + PADDING, 40, 20, hwnd, (HMENU)IDC_NET_CHK_THROTTLE, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrlToggle, IDS_TOOLTIP_NET_THROTTLE);
        SendMessage(hCtrlToggle, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(g_networkSettings.limit_bandwidth ? hBmpToggleOn : hBmpToggleOff));
        LoadStringW(g_hinst, IDS_NET_LABEL_UPLOAD, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + ROW_SPACING + 10, LABEL_WIDTH - PADDING + 30, 16, hwnd, (HMENU)IDC_NET_STATIC_UPLOAD, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING + ROW_SPACING + 7, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_UPLOAD, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_UPLOAD);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(10240, 0));
        LoadStringW(g_hinst, IDS_NET_LABEL_DOWNLOAD, buffer, _countof(buffer));
        CreateWindowExW(0, L"STATIC", buffer, WS_CHILD | WS_VISIBLE, LABEL_X, y_pos + PADDING + (2 * ROW_SPACING) + 10, LABEL_WIDTH - PADDING + 30, 16, hwnd, (HMENU)IDC_NET_STATIC_DOWNLOAD, g_hinst, NULL);
        hCtrl = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_LEFT, EDIT_X, y_pos + PADDING + (2 * ROW_SPACING) + 7, EDIT_WIDTH, CONTROL_HEIGHT - 3, hwnd, (HMENU)IDC_NET_EDIT_DOWNLOAD, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_DOWNLOAD);
        hCtrlUpDown = CreateWindowExW(0, UPDOWN_CLASS, NULL, WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS, 0, 0, 0, 0, hwnd, NULL, g_hinst, NULL);
        SendMessage(hCtrlUpDown, UDM_SETBUDDY, (WPARAM)hCtrl, 0);
        SendMessage(hCtrlUpDown, UDM_SETRANGE, 0, MAKELPARAM(10240, 0));
        y_pos += 95 + MARGIN;

        LoadStringW(g_hinst, IDS_NET_SAVE_BUTTON, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, PADDING - 6, y_pos + 10, 120, 25, hwnd, (HMENU)ID_NET_SAVE_BTN, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_SAVE);
        LoadStringW(g_hinst, IDS_NET_CANCEL_BUTTON, buffer, _countof(buffer));
        hCtrl = CreateWindowExW(0, L"BUTTON", buffer, WS_CHILD | WS_VISIBLE, GROUP_WIDTH - 110, y_pos + 10, 120, 25, hwnd, (HMENU)ID_NET_CANCEL_BTN, g_hinst, NULL);
        AddTooltip(hTooltip, hCtrl, IDS_TOOLTIP_NET_CANCEL);

        EnumChildWindows(hwnd, SetChildFontProc, (LPARAM)g_hFont);
        LoadSettingsToWindow(hwnd);
        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
        }
        break;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_NET_SAVE_BTN:
            SaveSettingsFromWindow(hwnd);
            DestroyWindow(hwnd);
            break;
        case ID_NET_CANCEL_BTN:
            DestroyWindow(hwnd);
            break;
        case IDC_NET_CHK_THROTTLE:
        {
            g_networkSettings.limit_bandwidth = !g_networkSettings.limit_bandwidth;
            HWND hToggleButton = GetDlgItem(hwnd, IDC_NET_CHK_THROTTLE);
            HBITMAP hBmpNew = g_networkSettings.limit_bandwidth ? hBmpToggleOn : hBmpToggleOff;
            SendMessage(hToggleButton, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBmpNew);
            UpdateBandwidthControls(hwnd, g_networkSettings.limit_bandwidth);
            break;
        }
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
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
    {
        if (g_hWnd)
        {
            EnableWindow(g_hWnd, TRUE);
            SetForegroundWindow(g_hWnd);
        }
        DeleteObject(hBmpToggleOn);
        DeleteObject(hBmpToggleOff);
        g_hNetSettingsWnd = NULL;
        break;
    }
    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateNetworkSettingsWindow(HWND hParent)
{
    if (g_hNetSettingsWnd && IsWindow(g_hNetSettingsWnd))
    {
        SetForegroundWindow(g_hNetSettingsWnd);
        return;
    }
    WCHAR buffer[256];
    static BOOL isClassRegistered = FALSE;
    if (!isClassRegistered)
    {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = NetworkSettingsWndProc;
        wc.hInstance = g_hinst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        LoadStringW(g_hinst, IDS_NET_CLASSNAME, buffer, _countof(buffer));
        wc.lpszClassName = buffer;
        if (!RegisterClassExW(&wc))
        {
            LoadStringW(g_hinst, IDS_ERR_REGISTER_FAIL_NET, buffer, _countof(buffer));
            MessageBoxW(hParent, buffer, L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        isClassRegistered = TRUE;
    }

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    const int width = 290, height = 336;
    int x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    LoadStringW(g_hinst, IDS_NET_CAPTION, buffer, _countof(buffer));
    WCHAR className[256];
    LoadStringW(g_hinst, IDS_NET_CLASSNAME, className, _countof(className));

    EnableWindow(hParent, FALSE);
    g_hNetSettingsWnd = CreateWindowExW(WS_EX_DLGMODALFRAME, className, buffer, WS_CAPTION | WS_SYSMENU | WS_VISIBLE, x, y, width, height, hParent, NULL, g_hinst, NULL);
    if (!g_hNetSettingsWnd)
        EnableWindow(hParent, TRUE);
}