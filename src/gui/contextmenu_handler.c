
#include <server/server_logic.h>
#include <resource.h>
#include <globals.h>
#include <windows.h>
#include <gui/contextmenu_handler.h>
#include <commctrl.h>

void HandleContextMenu(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    POINT pt;
    GetCursorPos(&pt);
    HWND hClickedWnd = (HWND)wParam;

    if (hClickedWnd == g_server_controls.hwndListServers)
    {
        LVHITTESTINFO lvhti = {0};
        lvhti.pt = pt;
        ScreenToClient(g_server_controls.hwndListServers, &lvhti.pt);
        ListView_HitTest(g_server_controls.hwndListServers, &lvhti);

        if (lvhti.flags & LVHT_ONITEM)
        {

            g_serverContextMenuIndex = lvhti.iItem;

            HMENU hMenu = CreatePopupMenu();
            ServerStatus status = g_servers[g_serverContextMenuIndex].status;
            UINT startFlags = (status == SERVER_STOPPED || status == SERVER_ERROR) ? MF_STRING : MF_STRING | MF_GRAYED;
            UINT stopFlags = (status == SERVER_LISTENING) ? MF_STRING : MF_STRING | MF_GRAYED;

            WCHAR buffer[128];
            LoadStringW(g_hinst, IDS_MENU_START_SERVER, buffer, _countof(buffer));
            AppendMenuW(hMenu, startFlags, IDM_SERVER_START, buffer);
            LoadStringW(g_hinst, IDS_MENU_STOP_SERVER, buffer, _countof(buffer));
            AppendMenuW(hMenu, stopFlags, IDM_SERVER_STOP, buffer);
            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
            LoadStringW(g_hinst, IDS_MENU_REMOVE_SERVER, buffer, _countof(buffer));
            AppendMenuW(hMenu, MF_STRING, IDM_SERVER_REMOVE, buffer);

            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
    }
    else if (hClickedWnd == g_client_controls.hwndListClients)
    {
        LVHITTESTINFO lvhti = {0};
        lvhti.pt = pt;
        ScreenToClient(g_client_controls.hwndListClients, &lvhti.pt);
        ListView_HitTest(g_client_controls.hwndListClients, &lvhti);

        if (lvhti.flags & LVHT_ONITEM)
        {
            g_clientContextMenuIndex = lvhti.iItem;

            HMENU hMenu = CreatePopupMenu();
            HMENU hConnectionMenu = CreatePopupMenu();
            HMENU hKeyloggerMenu = CreatePopupMenu();
            WCHAR buffer[128];

            // === File Explorer ===
            LoadStringW(g_hinst, IDS_MENU_FILE_EXPLORER, buffer, _countof(buffer));
            AppendMenuW(hMenu, MF_STRING, IDM_CLIENT_FILE_EXPLORER, buffer);

            AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);

            // === Connection Submenu ===
            LoadStringW(g_hinst, IDS_MENU_DISCONNECT_CLIENT, buffer, _countof(buffer));
            AppendMenuW(hConnectionMenu, MF_STRING, IDM_CLIENT_DISCONNECT, buffer);

            LoadStringW(g_hinst, IDS_MENU_RESTART_CONNECTION, buffer, _countof(buffer));
            AppendMenuW(hConnectionMenu, MF_STRING, IDM_CLIENT_RESTART, buffer);

            LoadStringW(g_hinst, IDS_MENU_EXIT_CLIENT, buffer, _countof(buffer));
            AppendMenuW(hConnectionMenu, MF_STRING, IDM_CLIENT_EXIT, buffer);

            LoadStringW(g_hinst, IDS_SUBMENU_CONNECTION, buffer, _countof(buffer));
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hConnectionMenu, buffer);

            // === Keylogger Submenu ===
            LoadStringW(g_hinst, IDS_MENU_START_KEYLOGGER, buffer, _countof(buffer));
            AppendMenuW(hKeyloggerMenu, MF_STRING, IDM_CLIENT_KEYLOGGER_START, buffer);

            LoadStringW(g_hinst, IDS_MENU_STOP_KEYLOGGER, buffer, _countof(buffer));
            AppendMenuW(hKeyloggerMenu, MF_STRING, IDM_CLIENT_KEYLOGGER_STOP, buffer);

            LoadStringW(g_hinst, IDS_MENU_DWN_KEYLOG, buffer, _countof(buffer));
            AppendMenuW(hKeyloggerMenu, MF_STRING, IDM_CLIENT_KEYLOGGER_DOWNLOAD, buffer);

            LoadStringW(g_hinst, IDS_MENU_DEL_KEYLOG, buffer, _countof(buffer));
            AppendMenuW(hKeyloggerMenu, MF_STRING, IDM_CLIENT_KEYLOGGER_DELETE, buffer);

            LoadStringW(g_hinst, IDS_SUBMENU_KEYLOGGER, buffer, _countof(buffer));
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hKeyloggerMenu, buffer);

            // === Show menu ===
            TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

            // Cleanup
            DestroyMenu(hMenu);
            DestroyMenu(hConnectionMenu);
            DestroyMenu(hKeyloggerMenu);
        }
    }
}