#include <gui/tools/builder_page.h>
#include <globals.h>
#include <commctrl.h>

void CreateBuilderPage(HWND hParent, RECT tabRect, BuilderControls *pControls)
{
    pControls->hwndBuilderLabel = CreateWindowExW(0, L"STATIC", L"Builder functionality coming soon...",
                                                  WS_CHILD | SS_CENTER,
                                                  tabRect.left, tabRect.top + 50, tabRect.right - tabRect.left, 20,
                                                  hParent, NULL, g_hinst, NULL);
}

void ShowBuilderPage(const BuilderControls *pControls, BOOL bShow)
{
    ShowWindow(pControls->hwndBuilderLabel, bShow ? SW_SHOW : SW_HIDE);
}