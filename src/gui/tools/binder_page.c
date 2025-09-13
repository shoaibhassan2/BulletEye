#include <gui/tools/binder_page.h>
#include <globals.h>
#include <commctrl.h>

void CreateBinderPage(HWND hParent, RECT tabRect, BinderControls *pControls)
{
    pControls->hwndBinderLabel = CreateWindowExW(0, L"STATIC", L"Binder functionality coming soon...",
                                                 WS_CHILD | SS_CENTER,
                                                 tabRect.left, tabRect.top + 50, tabRect.right - tabRect.left, 20,
                                                 hParent, NULL, g_hinst, NULL);
}

void ShowBinderPage(const BinderControls *pControls, BOOL bShow)
{
    ShowWindow(pControls->hwndBinderLabel, bShow ? SW_SHOW : SW_HIDE);
}