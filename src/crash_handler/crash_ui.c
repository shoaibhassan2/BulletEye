

#include <stdio.h>
#include <globals.h>
#include <crash_handler/crash_ui.h>
#include <crash_handler/crash_handler.h>
#include <DarkMode.h>
#include <windows.h>
#include <commctrl.h>
static HWND g_hCrashWnd;
static char g_CrashReport[16384];

LRESULT CALLBACK CrashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ShowCrashWindow(const char *report)
{
    strncpy(g_CrashReport, report, sizeof(g_CrashReport) - 1);
    g_CrashReport[sizeof(g_CrashReport) - 1] = 0;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = CrashWndProc;
    wc.hInstance = g_hinst;
    wc.lpszClassName = L"CrashReportWindow";
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    g_hCrashWnd = CreateWindowEx(
        WS_EX_DLGMODALFRAME,
        wc.lpszClassName,
        L"Application Crash Detected",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 700, 500,
        NULL, NULL, g_hinst, NULL);

    ShowWindow(g_hCrashWnd, SW_SHOW);
    UpdateWindow(g_hCrashWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!IsDialogMessage(g_hCrashWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

LRESULT CALLBACK CrashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit, hBtnClose, hBtnRestart, hBtnSave;

    switch (msg)
    {
    case WM_CREATE:
        CreateWindowEx(0, L"STATIC", L"Oops! The program has crashed.",
                       WS_CHILD | WS_VISIBLE, 20, 10, 400, 20, hwnd, NULL, g_hinst, NULL);

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
                               WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL |
                                   ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                               20, 40, 640, 360,
                               hwnd, NULL, g_hinst, NULL);
        SendMessageA(hEdit, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        SetWindowTextA(hEdit, g_CrashReport);

        hBtnClose = CreateWindowEx(0, L"BUTTON", L"Close",
                                   WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                                   200, 420, 80, 30, hwnd, (HMENU)1, g_hinst, NULL);
        hBtnRestart = CreateWindowEx(0, L"BUTTON", L"Restart",
                                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     300, 420, 80, 30, hwnd, (HMENU)2, g_hinst, NULL);
        hBtnSave = CreateWindowEx(0, L"BUTTON", L"Save Report",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  400, 420, 100, 30, hwnd, (HMENU)3, g_hinst, NULL);

        SendMessage(hBtnClose, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        SendMessage(hBtnRestart, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        SendMessage(hBtnSave, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        if (DarkMode_isEnabled())
        {
            DarkMode_setDarkWndNotifySafe_Default(hwnd);
            DarkMode_setWindowEraseBgSubclass(hwnd);
        }
        break;
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
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            ExitProcess(1);
            break; // Close
        case 2:
        { // Restart
            wchar_t path[MAX_PATH];
            GetModuleFileNameW(NULL, path, MAX_PATH);
            ShellExecuteW(NULL, L"open", path, NULL, NULL, SW_SHOWNORMAL);
            ExitProcess(0);
            break;
        }
        case 3:
        { // Save
            HANDLE hFile = CreateFileW(L"crash_report.txt", GENERIC_WRITE, 0, NULL,
                                       CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                DWORD written;
                WriteFile(hFile, g_CrashReport, (DWORD)strlen(g_CrashReport), &written, NULL);
                CloseHandle(hFile);
                MessageBoxW(hwnd, L"Crash report saved as crash_report.txt", L"Saved", MB_OK);
            }
            break;
        }
        }
        break;

    case WM_CLOSE:
        ExitProcess(1);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
