
#include <gui/file_explorer/fe_toolbar_view.h>
#include <resource.h>
#include <commctrl.h>
#include <DarkMode.h>
#include <gui/log_page.h>

BOOL CreateToolbarView(HWND hParent, FileExplorerData *pData)
{
    pData->hToolBar = CreateWindowEx(0, TOOLBARCLASSNAMEW, NULL,
                                     WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_NODIVIDER,
                                     0, 0, 0, 0, hParent, (HMENU)IDC_FE_TOOLBAR, g_hinst, NULL);
    SendMessage(pData->hToolBar, TB_SETBUTTONSIZE, 0, MAKELONG(36, 36));
    SendMessage(pData->hToolBar, WM_SETFONT, (WPARAM)g_hFont, TRUE);

    HIMAGELIST hImgList = ImageList_Create(24, 24, ILC_COLOR32 | ILC_MASK, 3, 0);
    SetWindowLongPtr(pData->hToolBar, GWLP_USERDATA, (LONG_PTR)hImgList);

    HBITMAP hBmpBack = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BACKWARD));
    HBITMAP hBmpFwd = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_FORWARD));
    HBITMAP hBmpSettings = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_SETTINGS));
    int iBack = ImageList_Add(hImgList, hBmpBack, NULL);
    int iFwd = ImageList_Add(hImgList, hBmpFwd, NULL);
    int iGear = ImageList_Add(hImgList, hBmpSettings, NULL);
    DeleteObject(hBmpBack);
    DeleteObject(hBmpFwd);
    DeleteObject(hBmpSettings);
    SendMessage(pData->hToolBar, TB_SETIMAGELIST, 0, (LPARAM)hImgList);

    TBBUTTON tbButtons[] = {
        {iBack, IDC_FE_NAV_BACK, TBSTATE_ENABLED, BTNS_BUTTON},
        {iFwd, IDC_FE_NAV_FORWARD, TBSTATE_ENABLED, BTNS_BUTTON},
        {0, 0, 0, BTNS_SEP},
        {iGear, IDC_FE_SHOW_SETTINGS, TBSTATE_ENABLED, BTNS_BUTTON}};
    SendMessage(pData->hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(pData->hToolBar, TB_ADDBUTTONS, (WPARAM)ARRAYSIZE(tbButtons), (LPARAM)&tbButtons);

    return TRUE;
}

void UpdateNavButtonsState(FileExplorerData *pData)
{
    if (!pData || !IsWindow(pData->hToolBar))
        return;
    SendMessage(pData->hToolBar, TB_SETSTATE, IDC_FE_NAV_BACK, MAKELONG(pData->iBackStackTop > -1 ? TBSTATE_ENABLED : 0, 0));
    SendMessage(pData->hToolBar, TB_SETSTATE, IDC_FE_NAV_FORWARD, MAKELONG(pData->iForwardStackTop > -1 ? TBSTATE_ENABLED : 0, 0));
}