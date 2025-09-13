#include <globals.h>
#include <windows.h>

#include <gui/log_page.h>
#include <resource.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gui/tooltip.h>

void CreateLogPage(HWND parent)
{
    WCHAR buf[128];

    const int MARGIN = 10;
    const int BUTTON_HEIGHT = 24;
    const int BUTTON_WIDTH = 80;
    const int PADDING = 6;
    const int TAB_HEADER_HEIGHT = 30;

    RECT parent_rect;
    GetClientRect(parent, &parent_rect);
    const int content_width = parent_rect.right;
    const int content_height = parent_rect.bottom;

    const int button_y = content_height - MARGIN - BUTTON_HEIGHT;
    const int clear_btn_x = MARGIN;
    const int autoscroll_chk_x = clear_btn_x + BUTTON_WIDTH + PADDING;
    const int copy_btn_x = content_width - MARGIN - BUTTON_WIDTH;
    const int save_btn_x = copy_btn_x - PADDING - BUTTON_WIDTH;

    const int list_x = MARGIN;
    const int list_y = MARGIN + TAB_HEADER_HEIGHT;
    const int list_width = content_width - (2 * MARGIN);
    const int list_height = button_y - list_y - MARGIN;

    g_log_controls.hwndListLog = CreateWindowW(WC_LISTVIEW, NULL,
                                               WS_CHILD | LVS_REPORT | WS_BORDER,
                                               list_x, list_y, list_width, list_height,
                                               parent, (HMENU)IDC_LOG_LISTVIEW, g_hinst, NULL);
    ListView_SetExtendedListViewStyle(g_log_controls.hwndListLog, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    LoadStringW(g_hinst, IDS_LOG_COLUMN_TIME, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 80;
    ListView_InsertColumn(g_log_controls.hwndListLog, 0, &lvc);
    LoadStringW(g_hinst, IDS_LOG_COLUMN_MESSAGE, buf, 128);
    lvc.pszText = buf;
    lvc.cx = 400;
    ListView_InsertColumn(g_log_controls.hwndListLog, 1, &lvc);

    LoadStringW(g_hinst, IDS_LOG_BUTTON_CLEAR, buf, 128);
    g_log_controls.hwndButtonClear = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_PUSHBUTTON,
                                                   clear_btn_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                                                   parent, (HMENU)IDC_LOG_CLEAR_BUTTON, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_LOG_CHECKBOX_AUTOSCROLL, buf, 128);
    g_log_controls.hwndCheckAutoscroll = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_AUTOCHECKBOX,
                                                       autoscroll_chk_x, button_y, BUTTON_WIDTH + 20, BUTTON_HEIGHT,
                                                       parent, (HMENU)IDC_LOG_AUTOSCROLL_CHECKBOX, g_hinst, NULL);
    CheckDlgButton(parent, IDC_LOG_AUTOSCROLL_CHECKBOX, BST_CHECKED);

    LoadStringW(g_hinst, IDS_LOG_BUTTON_SAVE, buf, 128);
    g_log_controls.hwndButtonSave = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_PUSHBUTTON,
                                                  save_btn_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                                                  parent, (HMENU)IDC_LOG_SAVE_BUTTON, g_hinst, NULL);

    LoadStringW(g_hinst, IDS_LOG_BUTTON_COPY, buf, 128);
    g_log_controls.hwndButtonCopy = CreateWindowW(WC_BUTTONW, buf, WS_CHILD | BS_PUSHBUTTON,
                                                  copy_btn_x, button_y, BUTTON_WIDTH, BUTTON_HEIGHT,
                                                  parent, (HMENU)IDC_LOG_COPY_BUTTON, g_hinst, NULL);

    SendMessageW(g_log_controls.hwndListLog, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_log_controls.hwndButtonClear, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_log_controls.hwndCheckAutoscroll, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_log_controls.hwndButtonSave, WM_SETFONT, (WPARAM)g_hFont, TRUE);
    SendMessageW(g_log_controls.hwndButtonCopy, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    AddTooltip(g_hTooltip, g_log_controls.hwndListLog, IDS_TOOLTIP_LOGLIST);
    AddTooltip(g_hTooltip, g_log_controls.hwndButtonClear, IDS_TOOLTIP_LOG_CLEAR);
    AddTooltip(g_hTooltip, g_log_controls.hwndCheckAutoscroll, IDS_TOOLTIP_LOG_AUTOSCROLL);
    AddTooltip(g_hTooltip, g_log_controls.hwndButtonSave, IDS_TOOLTIP_LOG_SAVE);
    AddTooltip(g_hTooltip, g_log_controls.hwndButtonCopy, IDS_TOOLTIP_LOG_COPY);
}

void AddLogMessage(const WCHAR *fmt, ...)
{
    HWND hList = g_log_controls.hwndListLog;
    if (!hList)
        return;

    WCHAR timeStr[16];
    SYSTEMTIME st;
    GetLocalTime(&st);
    wsprintfW(timeStr, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

    WCHAR buffer[1024];
    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buffer, 1024, fmt, args);
    va_end(args);

    LVITEMW item = {0};
    item.mask = LVIF_TEXT;
    item.pszText = timeStr;
    item.iItem = ListView_GetItemCount(hList);

    int index = ListView_InsertItem(hList, &item);
    if (index == -1)
        return;

    ListView_SetItemText(hList, index, 1, buffer);

    if (g_bLogAutoScroll)
    {
        ListView_EnsureVisible(hList, index, FALSE);
    }
}

void PostLogMessage(const WCHAR *fmt, ...)
{
    WCHAR *buffer = (WCHAR *)malloc(1024 * sizeof(WCHAR));
    if (!buffer)
        return;

    va_list args;
    va_start(args, fmt);
    _vsnwprintf(buffer, 1024, fmt, args);
    va_end(args);

    PostMessageW(g_hWnd, WM_APP_ADD_LOG, (WPARAM)0, (LPARAM)buffer);
}