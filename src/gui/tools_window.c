#include <globals.h>
#include <windows.h>
#include <commctrl.h>
#include <resource.h>
#include <gui/tools_window.h>
#include <DarkMode.h>

#include <gui/tools/icon_changer_page.h>
#include <gui/tools/builder_page.h>
#include <gui/tools/binder_page.h>

static HWND g_hToolsWnd = NULL;
static HWND g_hToolsTab = NULL;
static IconChangerControls g_iconChangerControls;
static BuilderControls g_builderControls;
static BinderControls g_binderControls;

static void SwitchToolsTab(int tabIndex)
{

    ShowIconChangerPage(&g_iconChangerControls, FALSE);
    ShowBuilderPage(&g_builderControls, FALSE);
    ShowBinderPage(&g_binderControls, FALSE);

    switch (tabIndex)
    {
    case 0:
        ShowIconChangerPage(&g_iconChangerControls, TRUE);
        break;
    case 1:
        ShowBuilderPage(&g_builderControls, TRUE);
        break;
    case 2:
        ShowBinderPage(&g_binderControls, TRUE);
        break;
    }
}

static BOOL CALLBACK SetChildControlsFontProc(HWND hChild, LPARAM lParam)
{
    HFONT hFont = (HFONT)lParam;
    SendMessage(hChild, WM_SETFONT, (WPARAM)hFont, TRUE);
    return TRUE;
}

LRESULT CALLBACK ToolsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        WCHAR buffer[256];
        RECT rcClient;
        GetClientRect(hwnd, &rcClient);

        g_hToolsTab = CreateWindowExW(0, WC_TABCONTROLW, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
                                      5, 5, rcClient.right - 10, rcClient.bottom - 15, hwnd, (HMENU)IDC_TOOLS_TAB, g_hinst, NULL);
        SendMessageW(g_hToolsTab, WM_SETFONT, (WPARAM)g_hFont, TRUE);

        TCITEMW tie = {0};
        tie.mask = TCIF_TEXT;
        LoadStringW(g_hinst, IDS_TOOLS_TAB_ICON_CHANGER, buffer, _countof(buffer));
        tie.pszText = buffer;
        TabCtrl_InsertItem(g_hToolsTab, 0, &tie);
        LoadStringW(g_hinst, IDS_TOOLS_TAB_BUILDER, buffer, _countof(buffer));
        tie.pszText = buffer;
        TabCtrl_InsertItem(g_hToolsTab, 1, &tie);
        LoadStringW(g_hinst, IDS_TOOLS_TAB_BINDER, buffer, _countof(buffer));
        tie.pszText = buffer;
        TabCtrl_InsertItem(g_hToolsTab, 2, &tie);

        RECT tabRect;
        GetWindowRect(g_hToolsTab, &tabRect);
        MapWindowPoints(NULL, hwnd, (LPPOINT)&tabRect, 2);
        TabCtrl_AdjustRect(g_hToolsTab, FALSE, &tabRect);

        CreateIconChangerPage(hwnd, tabRect, &g_iconChangerControls);
        CreateBuilderPage(hwnd, tabRect, &g_builderControls);
        CreateBinderPage(hwnd, tabRect, &g_binderControls);

        EnumChildWindows(hwnd, SetChildControlsFontProc, (LPARAM)g_hFont);

        SwitchToolsTab(0);

        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
            DarkMode_setTabCtrlSubclass(g_hToolsTab);
        }
        break;
    }

    case WM_COMMAND:
    {

        HandleIconChangerCommand(hwnd, LOWORD(wParam), &g_iconChangerControls);

        break;
    }

    case WM_NOTIFY:
    {
        LPNMHDR lpnmh = (LPNMHDR)lParam;
        if (lpnmh->hwndFrom == g_hToolsTab && lpnmh->code == TCN_SELCHANGE)
        {
            SwitchToolsTab(TabCtrl_GetCurSel(g_hToolsTab));
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
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        if (DarkMode_isEnabled())
        {
            break;
        }
        HDC hdc = (HDC)wParam;
        SetBkMode(hdc, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        EnableWindow(g_hWnd, TRUE);
        SetForegroundWindow(g_hWnd);
        g_hToolsWnd = NULL;
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void CreateToolsWindow(HWND hParent)
{
    if (g_hToolsWnd && IsWindow(g_hToolsWnd))
    {
        SetForegroundWindow(g_hToolsWnd);
        return;
    }

    WCHAR buffer[256];
    static BOOL isClassRegistered = FALSE;

    LoadStringW(g_hinst, IDS_TOOLS_CLASSNAME, buffer, _countof(buffer));

    if (!isClassRegistered)
    {
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = ToolsWndProc;
        wc.hInstance = g_hinst;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszClassName = buffer;
        wc.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));
        wc.hIconSm = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_ICON_MAIN));

        if (!RegisterClassExW(&wc))
        {
            WCHAR errBuffer[256];

            LoadStringW(g_hinst, IDS_ERR_REGISTER_FAIL_TOOLS, errBuffer, _countof(errBuffer));
            MessageBoxW(hParent, errBuffer, L"Error", MB_OK | MB_ICONERROR);
            return;
        }
        isClassRegistered = TRUE;
    }

    RECT rcParent;
    GetWindowRect(hParent, &rcParent);
    const int width = 500, height = 250;
    int x = rcParent.left + (rcParent.right - rcParent.left - width) / 2;
    int y = rcParent.top + (rcParent.bottom - rcParent.top - height) / 2;

    EnableWindow(hParent, FALSE);

    WCHAR captionBuffer[256];
    LoadStringW(g_hinst, IDS_TOOLS_CAPTION, captionBuffer, _countof(captionBuffer));

    g_hToolsWnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        buffer,
        captionBuffer,
        WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, width, height,
        hParent, NULL, g_hinst, NULL);

    if (!g_hToolsWnd)
    {
        EnableWindow(hParent, TRUE);
    }
}