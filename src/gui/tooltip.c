#include <gui/tooltip.h>
#include <globals.h>
#include <commctrl.h>
#include <globals.h>

static WCHAR g_tooltipBuffer[512];

HWND CreateTooltip(HWND hParent)
{

    HWND hTooltip = CreateWindowExW(
        0, TOOLTIPS_CLASSW, NULL,
        WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        hParent, NULL, g_hinst, NULL);

    if (hTooltip)
    {

        SendMessage(hTooltip, TTM_SETDELAYTIME, TTDT_AUTOMATIC, MAKELPARAM(500, 0));
    }

    return hTooltip;
}

void AddTooltip(HWND hTooltip, HWND hControl, int resourceId)
{
    if (!hTooltip || !hControl || IsWindow(hControl) == FALSE)
    {
        return;
    }

    int len = LoadStringW(g_hinst, resourceId, g_tooltipBuffer, _countof(g_tooltipBuffer));
    if (len == 0)
    {

        return;
    }

    TOOLINFOW ti = {0};
    ti.cbSize = sizeof(TOOLINFOW);
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = GetParent(hControl);
    ti.uId = (UINT_PTR)hControl;
    ti.lpszText = g_tooltipBuffer;

    SendMessageW(hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&ti);
}