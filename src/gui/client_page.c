#include <globals.h>
#include <gui/client_page.h>
#include <resource.h>
#include <commctrl.h>
#include <gui/tooltip.h>
#include <helpers/utils.h>
#include <windows.h>

void CreateClientPage(HWND parent)
{
    WCHAR buf[128];

    const int MARGIN = 10;
    const int TAB_HEADER_HEIGHT = 30;

    RECT parent_rect;
    GetClientRect(parent, &parent_rect);

    const int list_x = MARGIN;
    const int list_y = TAB_HEADER_HEIGHT + MARGIN;
    const int list_width = parent_rect.right - (2 * MARGIN);
    const int list_height = parent_rect.bottom - TAB_HEADER_HEIGHT - (2 * MARGIN);

    g_client_controls.hwndListClients = CreateWindowW(
        WC_LISTVIEW, NULL,
        WS_CHILD | LVS_REPORT | LVS_SINGLESEL | WS_BORDER,
        list_x, list_y, list_width, list_height,
        parent, NULL, g_hinst, NULL);

    HWND hList = g_client_controls.hwndListClients;

    ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    LVCOLUMNW lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt = LVCFMT_CENTER;

    lvc.cx = 80;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_IP, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 0, &lvc);

    lvc.cx = 80;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_COUNTRY, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 1, &lvc);

    lvc.cx = 140;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_ID, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 2, &lvc);

    lvc.cx = 140;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_USERNAME, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 3, &lvc);

    lvc.cx = 220;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_OS, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 4, &lvc);

    lvc.cx = 150;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_GROUP, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 5, &lvc);

    lvc.cx = 150;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_DATE, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 6, &lvc);

    lvc.cx = 80;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_UAC, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 7, &lvc);

    lvc.cx = 220;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_CPU, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 8, &lvc);

    lvc.cx = 220;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_GPU, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 9, &lvc);

    lvc.cx = 80;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_RAM, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 10, &lvc);

    lvc.cx = 150;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_ANTIVIRUS, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 11, &lvc);

    lvc.cx = 360;
    LoadStringW(g_hinst, IDS_LISTVIEW_CLIENT_ACTIVE_WINDOW, buf, 128);
    lvc.pszText = buf;
    ListView_InsertColumn(hList, 12, &lvc);

    SendMessageW(hList, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    AddTooltip(g_hTooltip, hList, IDS_TOOLTIP_CLIENTLIST);
}

void AddClientToListView(int clientIndex, const ClientInfo *clientInfo)
{
    if (!g_client_controls.hwndListClients || !clientInfo)
    {
        return;
    }

    HWND hList = g_client_controls.hwndListClients;

    LVITEMW lvi = {0};
    lvi.mask = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem = ListView_GetItemCount(hList);
    lvi.lParam = (LPARAM)clientIndex;

    WCHAR *ipW = ConvertCHARToWCHAR(clientInfo->ipAddress);
    WCHAR *countryW = ConvertCHARToWCHAR(clientInfo->country);
    WCHAR *idW = ConvertCHARToWCHAR(clientInfo->clientID);
    WCHAR *usernameW = ConvertCHARToWCHAR(clientInfo->username);
    WCHAR *osW = ConvertCHARToWCHAR(clientInfo->os);
    WCHAR *groupW = ConvertCHARToWCHAR(clientInfo->group);
    WCHAR *dateW = ConvertCHARToWCHAR(clientInfo->date);
    WCHAR *cpuW = ConvertCHARToWCHAR(clientInfo->cpu);
    WCHAR *gpuW = ConvertCHARToWCHAR(clientInfo->gpu);
    WCHAR *ramW = ConvertCHARToWCHAR(clientInfo->ram);
    WCHAR *antivirusW = ConvertCHARToWCHAR(clientInfo->antivirus);
    WCHAR *activeWindowW = ConvertCHARToWCHAR(clientInfo->active_window);

    WCHAR uacW[16];
    LoadStringW(g_hinst, clientInfo->UAC ? IDS_UAC_ADMIN : IDS_UAC_USER, uacW, _countof(uacW));

    lvi.iSubItem = 0;
    lvi.pszText = ipW;
    int insertedIndex = ListView_InsertItem(hList, &lvi);

    if (insertedIndex == -1)
    {

        free(ipW);
        free(countryW);
        free(idW);
        free(usernameW);
        free(osW);
        free(groupW);
        free(dateW);
        free(cpuW);
        free(gpuW);
        free(ramW);
        free(antivirusW);
        free(activeWindowW);
        return;
    }

    ListView_SetItemText(hList, insertedIndex, 1, countryW);
    ListView_SetItemText(hList, insertedIndex, 2, idW);
    ListView_SetItemText(hList, insertedIndex, 3, usernameW);
    ListView_SetItemText(hList, insertedIndex, 4, osW);
    ListView_SetItemText(hList, insertedIndex, 5, groupW);
    ListView_SetItemText(hList, insertedIndex, 6, dateW);
    ListView_SetItemText(hList, insertedIndex, 7, uacW);
    ListView_SetItemText(hList, insertedIndex, 8, cpuW);
    ListView_SetItemText(hList, insertedIndex, 9, gpuW);
    ListView_SetItemText(hList, insertedIndex, 10, ramW);
    ListView_SetItemText(hList, insertedIndex, 11, antivirusW);
    ListView_SetItemText(hList, insertedIndex, 12, activeWindowW);

    free(ipW);
    free(countryW);
    free(idW);
    free(usernameW);
    free(osW);
    free(groupW);
    free(dateW);
    free(cpuW);
    free(gpuW);
    free(ramW);
    free(antivirusW);
    free(activeWindowW);
}