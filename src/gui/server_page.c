#include <globals.h>
#include <windows.h>

#include <gui/server_page.h>
#include <gui/log_page.h>
#include <resource.h>
#include <commctrl.h>
#include <gui/tooltip.h>
#include <DarkMode.h>
#include <stdio.h>
#include <uxtheme.h>
#include <ctype.h>

static WCHAR g_lastValidPort[16] = L"54321";

LRESULT CALLBACK PortEditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {
    case WM_NCDESTROY:
    {

        RemoveWindowSubclass(hWnd, PortEditSubclassProc, uIdSubclass);
        break;
    }

    case WM_CHAR:
    {

        if (!iswdigit((wint_t)wParam) && wParam != VK_BACK)
        {
            return 0;
        }
        break;
    }

    case WM_KEYUP:
    {
        WCHAR currentText[16];
        GetWindowTextW(hWnd, currentText, _countof(currentText));

        if (wcslen(currentText) > 0)
        {
            long port = _wtol(currentText);
            if (port > 65535)
            {

                SetWindowTextW(hWnd, g_lastValidPort);
                SendMessage(hWnd, EM_SETSEL, (WPARAM)-1, (LPARAM)-1);

                WCHAR warningMsg[256];
                LoadStringW(g_hinst, IDS_TOOLTIP_PORT_EXCEED, warningMsg, _countof(warningMsg));

                TOOLINFOW ti = {0};
                ti.cbSize = sizeof(ti);
                ti.hwnd = GetParent(hWnd);
                ti.uId = (UINT_PTR)hWnd;
                ti.lpszText = warningMsg;

                SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
                RECT rc;
                GetWindowRect(hWnd, &rc);
                SendMessageW(g_hTooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(rc.left, rc.bottom + 2));
                SendMessageW(g_hTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);

                SetTimer(GetParent(hWnd), (UINT_PTR)hWnd, 3000, NULL);

                MessageBeep(MB_ICONEXCLAMATION);
            }
            else
            {

#ifndef __TINYC__
                wcscpy_s(g_lastValidPort, _countof(g_lastValidPort), currentText);
#else
                snwprintf(g_lastValidPort, sizeof(g_lastValidPort) / sizeof(wchar_t), L"%ls", currentText);
#endif
            }
        }
        else
        {
#ifndef __TINYC__
            wcscpy_s(g_lastValidPort, _countof(g_lastValidPort), L"");
#else
            snwprintf(g_lastValidPort, sizeof(g_lastValidPort) / sizeof(wchar_t), L"%ls", L"");
#endif
        }
        break;
    }

    case WM_TIMER:
    {

        if (wParam == (UINT_PTR)hWnd)
        {
            KillTimer(GetParent(hWnd), (UINT_PTR)hWnd);
            TOOLINFOW ti = {0};
            ti.cbSize = sizeof(ti);
            ti.hwnd = GetParent(hWnd);
            ti.uId = (UINT_PTR)hWnd;
            SendMessageW(g_hTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);

            WCHAR originalTooltip[256];
            LoadStringW(g_hinst, IDS_TOOLTIP_PORT, originalTooltip, _countof(originalTooltip));
            ti.lpszText = originalTooltip;
            SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
        }
        break;
    }

    case WM_KILLFOCUS:
    {

        KillTimer(GetParent(hWnd), (UINT_PTR)hWnd);
        TOOLINFOW ti = {0};
        ti.cbSize = sizeof(ti);
        ti.hwnd = GetParent(hWnd);
        ti.uId = (UINT_PTR)hWnd;
        SendMessageW(g_hTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef struct
{
    HWND hwnd;
    LONG left;
} ChildWindowInfo;

typedef struct
{
    ChildWindowInfo *windows;
    int count;
} EnumContext;

static int CompareChildWindows(const void *a, const void *b)
{
    ChildWindowInfo *infoA = (ChildWindowInfo *)a;
    ChildWindowInfo *infoB = (ChildWindowInfo *)b;
    return infoA->left - infoB->left;
}

static BOOL CALLBACK EnumAndStoreChildrenProc(HWND hWnd, LPARAM lParam)
{
    EnumContext *context = (EnumContext *)lParam;
    if (context->count < 4)
    {
        RECT rect;
        GetWindowRect(hWnd, &rect);
        context->windows[context->count].hwnd = hWnd;
        context->windows[context->count].left = rect.left;
        context->count++;
        SetWindowTheme(hWnd, L"Explorer", L"");
    }
    return TRUE;
}

static LRESULT CALLBACK IPAddressContainerSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    (void)uIdSubclass;
    (void)dwRefData;
    if (DarkMode_isEnabled())
    {
        switch (uMsg)
        {
        case WM_CHAR:
        case WM_KEYDOWN:
        {
            return 0;
        }
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wParam;
            return DarkMode_onCtlColorCtrl(hdc);
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            FillRect(hdc, &ps.rcPaint, DarkMode_getBackgroundBrush());

            ChildWindowInfo childWindows[4];
            EnumContext context = {childWindows, 0};
            EnumChildWindows(hWnd, EnumAndStoreChildrenProc, (LPARAM)&context);

            qsort(childWindows, context.count, sizeof(ChildWindowInfo), CompareChildWindows);

            for (int i = 0; i < context.count; ++i)
            {
                RedrawWindow(childWindows[i].hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }

            SetTextColor(hdc, DarkMode_getTextColor());
            SetBkMode(hdc, TRANSPARENT);

            RECT rcPrev, rcCurr, rcDot;
            for (int i = 1; i < context.count; ++i)
            {
                GetWindowRect(childWindows[i - 1].hwnd, &rcPrev);
                GetWindowRect(childWindows[i].hwnd, &rcCurr);

                MapWindowPoints(NULL, hWnd, (LPPOINT)&rcPrev, 2);
                MapWindowPoints(NULL, hWnd, (LPPOINT)&rcCurr, 2);

                rcDot.left = rcPrev.right;
                rcDot.right = rcCurr.left;
                rcDot.top = rcPrev.top;
                rcDot.bottom = rcPrev.bottom;

                if (rcDot.right > rcDot.left)
                {
                    FillRect(hdc, &rcDot, DarkMode_getCtrlBackgroundBrush());

                    DrawTextW(hdc, L".", 1, &rcDot, DT_CENTER | DT_SINGLELINE);
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_COMMAND:
        {
            WORD notificationCode = HIWORD(wParam);
            if (notificationCode == EN_SETFOCUS || notificationCode == EN_KILLFOCUS)
            {
                RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW);
            }
            break;
        }
        case WM_NCDESTROY:
        {
            RemoveWindowSubclass(hWnd, IPAddressContainerSubclassProc, 0);
            break;
        }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ReadOnlyEditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                  UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (msg)
    {
    case WM_CHAR:
    case WM_KEYDOWN:
        return 0;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK EnumEditProc(HWND hwnd, LPARAM lParam)
{
    wchar_t className[32];
    GetClassNameW(hwnd, className, 32);

    if (wcscmp(className, L"Edit") == 0)
    {
        SetWindowSubclass(hwnd, ReadOnlyEditProc, 1, 0);
    }
    return TRUE;
}

void DarkMode_themeIPAddressCtrl(HWND hIpCtrl)
{
    if (!hIpCtrl)
        return;

    if (DarkMode_isEnabled())
    {
        SetWindowSubclass(hIpCtrl, IPAddressContainerSubclassProc, 0, 0);
        RedrawWindow(hIpCtrl, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
    }
    EnumChildWindows(hIpCtrl, EnumEditProc, 0);
}

void CreateServerPage(HWND parent)
{
    WCHAR buf[128];
    HWND hwnd;
    HBITMAP hBitmap;

    RECT display_rect;
    GetClientRect(parent, &display_rect);
    TabCtrl_AdjustRect(hwndTab, FALSE, &display_rect);

    const int MARGIN = 10;
    const int PADDING = 10;
    const int CONTROL_HEIGHT = 24;
    const int content_left = display_rect.left + MARGIN;
    const int content_top = display_rect.top;
    const int content_width = display_rect.right - display_rect.left - (2 * MARGIN);
    int current_y = content_top;
    const int TOOLBAR_HEIGHT = 50;
    const int toolbar_x = display_rect.left;
    const int toolbar_y = current_y;
    const int toolbar_width = display_rect.right - display_rect.left;
    current_y += TOOLBAR_HEIGHT + PADDING;
    const int CONTROLS_GROUP_HEIGHT = 95;
    const int controls_group_x = content_left;
    const int controls_group_y = current_y;
    int inner_y1 = controls_group_y + PADDING + 10;
    int inner_x = controls_group_x + PADDING;
    const int IP_LABEL_WIDTH = 85;
    const int IP_EDIT_WIDTH = 376;
    const int ip_label_x = inner_x;
    const int ip_edit_x = ip_label_x + IP_LABEL_WIDTH;
    int inner_y2 = inner_y1 + CONTROL_HEIGHT + PADDING;
    const int PORT_LABEL_WIDTH = 85;
    const int BUTTON_WIDTH = 90;
    const int port_label_x = inner_x;
    const int port_edit_x = port_label_x + PORT_LABEL_WIDTH;
    const int add_btn_x = controls_group_x + content_width - PADDING - BUTTON_WIDTH;
    const int port_edit_width = add_btn_x - port_edit_x - PADDING;
    current_y += CONTROLS_GROUP_HEIGHT + MARGIN;
    const int STATS_GROUP_HEIGHT = 65;
    const int stats_group_x = content_left;
    const int stats_group_y = current_y;
    int stats_inner_y = stats_group_y + PADDING + 18;
    const int section_width = (content_width - (2 * PADDING)) / 3;
    int stats_current_x = stats_group_x + PADDING;
    const int STAT_LABEL_WIDTH = 110;
    const int STAT_VALUE_WIDTH = section_width - STAT_LABEL_WIDTH - 5;
    current_y += STATS_GROUP_HEIGHT + MARGIN;
    const int list_y = current_y;
    const int list_height = display_rect.bottom - list_y - MARGIN;

    g_server_controls.hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
                                                   WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NORESIZE,
                                                   toolbar_x, toolbar_y, toolbar_width, TOOLBAR_HEIGHT, parent, NULL, g_hinst, NULL);

    HIMAGELIST hImageList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 5, 5);

    hBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_WORLD));
    ImageList_Add(hImageList, hBitmap, NULL);
    DeleteObject(hBitmap);
    hBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_TOOLS));
    ImageList_Add(hImageList, hBitmap, NULL);
    DeleteObject(hBitmap);
    hBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_LOGS));
    ImageList_Add(hImageList, hBitmap, NULL);
    DeleteObject(hBitmap);
    hBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_SETTINGS_ALT));
    ImageList_Add(hImageList, hBitmap, NULL);
    DeleteObject(hBitmap);
    hBitmap = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_INFO));
    ImageList_Add(hImageList, hBitmap, NULL);
    DeleteObject(hBitmap);

    SendMessage(g_server_controls.hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);

    WCHAR tbStrings[5][32];
    LoadStringW(g_hinst, IDS_TOOLBAR_NETWORK, tbStrings[0], 32);
    LoadStringW(g_hinst, IDS_TOOLBAR_TOOLS, tbStrings[1], 32);
    LoadStringW(g_hinst, IDS_TOOLBAR_LOGS, tbStrings[2], 32);
    LoadStringW(g_hinst, IDS_TOOLBAR_SETTINGS, tbStrings[3], 32);
    LoadStringW(g_hinst, IDS_TOOLBAR_ABOUT, tbStrings[4], 32);

    TBBUTTON tbButtons[] = {
        {0, IDC_BTN_WORLD, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tbStrings[0]},
        {1, IDC_BTN_TOOLS, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tbStrings[1]},
        {2, IDC_BTN_LOGS, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tbStrings[2]},
        {3, IDC_BTN_SETTINGS_ALT, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tbStrings[3]},
        {4, IDC_BTN_INFO, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)tbStrings[4]}};

    SendMessage(
        g_server_controls.hwndToolbar,
        TB_ADDBUTTONS,
        ARRAYSIZE(tbButtons),
        (LPARAM)&tbButtons);

    LoadStringW(g_hinst, IDS_GROUPBOX_SERVER_CONTROLS, buf, 128);
    g_server_controls.hwndGroupControls = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_GROUPBOX,
                                                        controls_group_x, controls_group_y, content_width, CONTROLS_GROUP_HEIGHT, parent, NULL, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_STATIC_SERVER_IPADDRESS, buf, 128);
    g_server_controls.hwndStaticIP = CreateWindowW(WC_STATICW, buf, WS_CHILD, ip_label_x, inner_y1, IP_LABEL_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    g_server_controls.hwndIpAddress = CreateWindowW(WC_IPADDRESSW, NULL, WS_CHILD | WS_TABSTOP | WS_BORDER, ip_edit_x, inner_y1, IP_EDIT_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    SendMessage(g_server_controls.hwndIpAddress, IPM_SETADDRESS, 0, MAKEIPADDRESS(0, 0, 0, 0));
    LONG exStyle = GetWindowLongW(g_server_controls.hwndIpAddress, GWL_EXSTYLE);
    exStyle &= ~WS_EX_CLIENTEDGE;
    SetWindowLongW(g_server_controls.hwndIpAddress, GWL_EXSTYLE, exStyle);

    SetWindowPos(g_server_controls.hwndIpAddress, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    DarkMode_themeIPAddressCtrl(g_server_controls.hwndIpAddress);

    LoadStringW(g_hinst, IDS_STATIC_SERVER_PORT, buf, 128);
    g_server_controls.hwndStaticPort = CreateWindowW(WC_STATICW, buf, WS_CHILD, port_label_x, inner_y2, PORT_LABEL_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    g_server_controls.hwndEditPort = CreateWindowW(WC_EDITW, L"54321", WS_CHILD | WS_BORDER | ES_NUMBER | ES_CENTER, port_edit_x, inner_y2, port_edit_width, CONTROL_HEIGHT, parent, (HMENU)IDC_PORT_EDIT, g_hinst, NULL);

    SetWindowSubclass(g_server_controls.hwndEditPort, PortEditSubclassProc, 0, 0);

    LoadStringW(g_hinst, IDS_BUTTON_SERVER_ADD, buf, 128);
    g_server_controls.hwndButtonAdd = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_PUSHBUTTON, add_btn_x, inner_y2, BUTTON_WIDTH, CONTROL_HEIGHT, parent, (HMENU)IDC_BTN_ADD_SERVER, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_GROUPBOX_SERVER_STATS, buf, 128);
    g_server_controls.hwndGroupStats = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_GROUPBOX,
                                                     stats_group_x, stats_group_y, content_width, STATS_GROUP_HEIGHT, parent, NULL, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_STATIC_CLIENTS_CONNECTED, buf, 128);
    g_server_controls.hwndStaticClients = CreateWindowW(WC_STATICW, buf, WS_CHILD, stats_current_x, stats_inner_y, STAT_LABEL_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    g_server_controls.hwndStaticClientsVal = CreateWindowW(WC_STATICW, L"0", WS_CHILD | SS_LEFT, stats_current_x + STAT_LABEL_WIDTH, stats_inner_y, STAT_VALUE_WIDTH, CONTROL_HEIGHT, parent, (HMENU)IDC_STATIC_CLIENTS_VALUE, g_hinst, NULL);
    stats_current_x += section_width;
    LoadStringW(g_hinst, IDS_STATIC_BYTES_RECEIVED, buf, 128);
    g_server_controls.hwndStaticReceived = CreateWindowW(WC_STATICW, buf, WS_CHILD, stats_current_x, stats_inner_y, STAT_LABEL_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    g_server_controls.hwndStaticReceivedVal = CreateWindowW(WC_STATICW, L"0", WS_CHILD | SS_LEFT, stats_current_x + STAT_LABEL_WIDTH, stats_inner_y, STAT_VALUE_WIDTH, CONTROL_HEIGHT, parent, (HMENU)IDC_STATIC_RECEIVED_VALUE, g_hinst, NULL);
    stats_current_x += section_width;
    LoadStringW(g_hinst, IDS_STATIC_BYTES_SENT, buf, 128);
    g_server_controls.hwndStaticSent = CreateWindowW(WC_STATICW, buf, WS_CHILD, stats_current_x, stats_inner_y, STAT_LABEL_WIDTH, CONTROL_HEIGHT, parent, NULL, g_hinst, NULL);
    g_server_controls.hwndStaticSentVal = CreateWindowW(WC_STATICW, L"0", WS_CHILD | SS_LEFT, stats_current_x + STAT_LABEL_WIDTH, stats_inner_y, STAT_VALUE_WIDTH, CONTROL_HEIGHT, parent, (HMENU)IDC_STATIC_SENT_VALUE, g_hinst, NULL);

    g_server_controls.hwndListServers = CreateWindowW(WC_LISTVIEW, NULL, WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
                                                      content_left, list_y, content_width, list_height, parent, NULL, g_hinst, NULL);
    ListView_SetExtendedListViewStyle(g_server_controls.hwndListServers, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    LoadStringW(g_hinst, IDS_LISTVIEW_SERVER_STATUS, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 100;
    ListView_InsertColumn(g_server_controls.hwndListServers, 0, &lvc);
    LoadStringW(g_hinst, IDS_LISTVIEW_SERVER_IPADDRESS, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 200;
    ListView_InsertColumn(g_server_controls.hwndListServers, 1, &lvc);
    LoadStringW(g_hinst, IDS_LISTVIEW_SERVER_PORT, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 100;
    ListView_InsertColumn(g_server_controls.hwndListServers, 2, &lvc);

    SendMessageW(g_server_controls.hwndToolbar, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndGroupControls, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticIP, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndIpAddress, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticPort, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndEditPort, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndButtonAdd, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndGroupStats, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticClients, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticClientsVal, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticReceived, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticReceivedVal, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticSent, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndStaticSentVal, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_server_controls.hwndListServers, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    AddTooltip(g_hTooltip, g_server_controls.hwndIpAddress, IDS_TOOLTIP_IPADDRESS);
    AddTooltip(g_hTooltip, g_server_controls.hwndEditPort, IDS_TOOLTIP_PORT);
    AddTooltip(g_hTooltip, g_server_controls.hwndButtonAdd, IDS_TOOLTIP_ADDSERVER);
    AddTooltip(g_hTooltip, g_server_controls.hwndListServers, IDS_TOOLTIP_SERVERLIST);
}