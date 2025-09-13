#include <globals.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <globals.h>
#include <gui/main_controls.h>
#include <resource.h>
#include <gui/server_page.h>
#include <gui/client_page.h>
#include <gui/log_page.h>

void CreateMainGuiControls()
{
    g_hFont = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    CreateTabControls();
}

void CreateTabControls()
{
    WCHAR buf[128];
    RECT rect;
    GetClientRect(g_hWnd, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    hwndTab = CreateWindowExW(
        0, WC_TABCONTROLW, L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, width, height,
        g_hWnd, (HMENU)IDC_TAB, g_hinst, NULL);

    SendMessageW(hwndTab, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    TCITEMW tie = {0};
    tie.mask = TCIF_TEXT;

    LoadStringW(g_hinst, IDS_TAB_SERVER, buf, 128);
    tie.pszText = buf;
    TabCtrl_InsertItem(hwndTab, 0, &tie);

    LoadStringW(g_hinst, IDS_TAB_CLIENT, buf, 128);
    tie.pszText = buf;
    TabCtrl_InsertItem(hwndTab, 1, &tie);

    LoadStringW(g_hinst, IDS_TAB_LOG, buf, 128);
    tie.pszText = buf;
    TabCtrl_InsertItem(hwndTab, 2, &tie);

    CreateServerPage(g_hWnd);
    CreateClientPage(g_hWnd);
    CreateLogPage(g_hWnd);
    SwitchTab(0);
}

void HideAllControls()
{

    ShowWindow(g_server_controls.hwndToolbar, SW_HIDE);
    ShowWindow(g_server_controls.hwndGroupControls, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticIP, SW_HIDE);
    ShowWindow(g_server_controls.hwndIpAddress, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticPort, SW_HIDE);
    ShowWindow(g_server_controls.hwndEditPort, SW_HIDE);
    ShowWindow(g_server_controls.hwndButtonAdd, SW_HIDE);
    ShowWindow(g_server_controls.hwndGroupStats, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticClients, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticClientsVal, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticReceived, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticReceivedVal, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticSent, SW_HIDE);
    ShowWindow(g_server_controls.hwndStaticSentVal, SW_HIDE);
    ShowWindow(g_server_controls.hwndListServers, SW_HIDE);

    ShowWindow(g_client_controls.hwndListClients, SW_HIDE);

    ShowWindow(g_log_controls.hwndListLog, SW_HIDE);
    ShowWindow(g_log_controls.hwndButtonClear, SW_HIDE);
    ShowWindow(g_log_controls.hwndCheckAutoscroll, SW_HIDE);
    ShowWindow(g_log_controls.hwndButtonSave, SW_HIDE);
    ShowWindow(g_log_controls.hwndButtonCopy, SW_HIDE);
}

void SwitchTab(int index)
{
    HideAllControls();

    switch (index)
    {
    case 0:
        ShowWindow(g_server_controls.hwndToolbar, SW_SHOW);
        ShowWindow(g_server_controls.hwndGroupControls, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticIP, SW_SHOW);
        ShowWindow(g_server_controls.hwndIpAddress, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticPort, SW_SHOW);
        ShowWindow(g_server_controls.hwndEditPort, SW_SHOW);
        ShowWindow(g_server_controls.hwndButtonAdd, SW_SHOW);
        ShowWindow(g_server_controls.hwndGroupStats, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticClients, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticClientsVal, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticReceived, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticReceivedVal, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticSent, SW_SHOW);
        ShowWindow(g_server_controls.hwndStaticSentVal, SW_SHOW);
        ShowWindow(g_server_controls.hwndListServers, SW_SHOW);
        break;
    case 1:
        ShowWindow(g_client_controls.hwndListClients, SW_SHOW);
        break;
    case 2:
        ShowWindow(g_log_controls.hwndListLog, SW_SHOW);
        ShowWindow(g_log_controls.hwndButtonClear, SW_SHOW);
        ShowWindow(g_log_controls.hwndCheckAutoscroll, SW_SHOW);
        ShowWindow(g_log_controls.hwndButtonSave, SW_SHOW);
        ShowWindow(g_log_controls.hwndButtonCopy, SW_SHOW);
        break;
    }
    TabCtrl_SetCurSel(hwndTab, index);
}