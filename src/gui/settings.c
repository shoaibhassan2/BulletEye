#include <gui/settings.h>
#include <gui/log_page.h>
#include <resource.h>

void SaveWindowSettings(HWND hWnd)
{
    WINDOWPLACEMENT wp = {0};
    wp.length = sizeof(WINDOWPLACEMENT);
    WCHAR regKeyPath[256];
    LoadStringW(g_hinst, IDS_REG_KEY_PATH, regKeyPath, _countof(regKeyPath));

    if (GetWindowPlacement(hWnd, &wp))
    {
        HKEY hKey;

        if (RegCreateKeyExW(HKEY_CURRENT_USER, regKeyPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
        {

            RegSetValueExW(hKey, L"WindowPlacement", 0, REG_BINARY, (const BYTE *)&wp, sizeof(wp));
            RegCloseKey(hKey);
            AddLogMessage(L"Window position saved to registry.");
        }
        else
        {
            AddLogMessage(L"Failed to open or create registry key for saving settings.");
        }
    }
    else
    {
        AddLogMessage(L"GetWindowPlacement failed; could not save window position.");
    }
}

BOOL LoadWindowSettings(WINDOWPLACEMENT *pwp)
{
    HKEY hKey;
    WCHAR regKeyPath[256];
    LoadStringW(g_hinst, IDS_REG_KEY_PATH, regKeyPath, _countof(regKeyPath));

    if (RegOpenKeyExW(HKEY_CURRENT_USER, regKeyPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwType = REG_BINARY;
        DWORD dwSize = sizeof(WINDOWPLACEMENT);

        if (RegQueryValueExW(hKey, L"WindowPlacement", NULL, &dwType, (LPBYTE)pwp, &dwSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            AddLogMessage(L"Window position loaded from registry.");
            return TRUE;
        }
        RegCloseKey(hKey);
    }

    AddLogMessage(L"No saved window position found. Using default size.");
    return FALSE;
}