

#include <globals.h>
#include <server/server_manager.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <gui/main_proc.h>
#include <globals.h>
#include <service/soundplayer.h>
#include <resource.h>
#include <gui/main_controls.h>
#include <gui/registry_settings.h>
#include <gui/log_page.h>
#include <gui/log_handler.h>
#include <gui/settings.h>
#include <gui/about_window.h>
#include <gui/network_settings_window.h>
#include <gui/tools_window.h>
#include <gui/tooltip.h>
#include <DarkMode.h>
#include <gui/command_handler.h>
#include <service/notification.h>
#include <gui/contextmenu_handler.h>
#include <gui/client_page.h>
#include <gui/file_explorer/fe_main_window.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch (Message)
    {
    case WM_CREATE:
    {
        g_bIsRunning = TRUE;
        g_hWnd = hwnd;
        g_hTooltip = CreateTooltip(g_hWnd);
        HANDLE hwelThread = CreateThread(NULL, 0, PlaySoundThread, (LPVOID)(uintptr_t)IDR_WAVE_WELCOME, 0, NULL);
        if (hwelThread)
            CloseHandle(hwelThread);

        CreateMainGuiControls();
        WCHAR logMsg[128];
        LoadStringW(g_hinst, IDS_LOG_APP_STARTED, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);

        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
        }
        break;
    }

    case WM_COMMAND:
    {
        HandleMainWindowCommands(hwnd, wParam, lParam);
        break;
    }

    case WM_APP_UPDATE_STATUS:
    {
        int serverIndex = (int)wParam;
        ServerStatus status = (ServerStatus)lParam;
        UpdateServerStatusInGUI(serverIndex, status);
        return 0;
    }
    case WM_APP_ADD_CLIENT:
    {
        int clientIndex = (int)wParam;
        ClientInfo *pClientInfo = (ClientInfo *)lParam;
        if (pClientInfo)
        {

            AddClientToListView(clientIndex, pClientInfo);
            PostLogMessage(L"New client connected: %hs (Slot: %d)", pClientInfo->ipAddress, clientIndex);

            free(pClientInfo);
        }
        return 0;
    }
    case WM_APP_ADD_LOG:
    {
        WCHAR *logMessage = (WCHAR *)lParam;
        if (logMessage)
        {
            AddLogMessage(L"%s", logMessage);
            free(logMessage);
        }
        return 0;
    }
    case WM_APP_REMOVE_CLIENT:
    {
        int clientIndexToRemove = (int)wParam;

        HWND hList = g_client_controls.hwndListClients;
        int itemCount = ListView_GetItemCount(hList);
        for (int i = 0; i < itemCount; ++i)
        {
            LVITEMW lvi = {0};
            lvi.mask = LVIF_PARAM;
            lvi.iItem = i;
            if (ListView_GetItem(hList, &lvi))
            {
                int storedClientIndex = (int)lvi.lParam;
                if (storedClientIndex == clientIndexToRemove)
                {
                    ListView_DeleteItem(hList, i);
                    PostLogMessage(L"Client in slot %d removed from GUI.", clientIndexToRemove);
                    break;
                }
            }
        }
        return 0;
    }
    case WM_CONTEXTMENU:
    {
        HandleContextMenu(hwnd, wParam, lParam);
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
    case WM_NOTIFY:
    {
        if (((LPNMHDR)lParam)->hwndFrom == hwndTab &&
            ((LPNMHDR)lParam)->code == TCN_SELCHANGE)
        {
            int iSel = TabCtrl_GetCurSel(hwndTab);
            SwitchTab(iSel);
        }
        break;
    }
    case WM_ENTERSIZEMOVE:
    {
        SetLayeredWindowAttributes(hwnd, 0, 65, LWA_ALPHA);
        break;
    }

    case WM_EXITSIZEMOVE:
    {
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
        break;
    }
    case WM_DESTROY:
    {
        WCHAR logMsg[128];
        LoadStringW(g_hinst, IDS_LOG_APP_CLOSING, logMsg, _countof(logMsg));
        AddLogMessage(logMsg);

        g_bIsRunning = FALSE;
        SaveServerSettingsToRegistry();

        SaveWindowSettings(hwnd);
        DeleteObject(g_hFont);
        HANDLE hbyeThread = CreateThread(NULL, 0, PlaySoundThread, (LPVOID)(uintptr_t)IDR_WAVE_CLOSED, 0, NULL);
        if (hbyeThread)
        {
            WaitForSingleObject(hbyeThread, INFINITE);
            CloseHandle(hbyeThread);
        }
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
}