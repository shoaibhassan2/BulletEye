#ifndef ICON_CHANGER_PAGE_H
#define ICON_CHANGER_PAGE_H

#include <globals.h>

typedef struct
{
    HWND hwndIconGroup;
    HWND hwndIconPathLabel;
    HWND hwndIconPathEdit;
    HWND hwndIconBrowseBtn;
    HWND hwndExePathLabel;
    HWND hwndExePathEdit;
    HWND hwndExeBrowseBtn;
    HWND hwndChangeIconBtn;
    HWND hwndExtractGroup;
    HWND hwndExtractIconBtn;
} IconChangerControls;

void CreateIconChangerPage(HWND hParent, RECT tabRect, IconChangerControls *pControls);

void ShowIconChangerPage(const IconChangerControls *pControls, BOOL bShow);

void HandleIconChangerCommand(HWND hParent, WORD commandID, const IconChangerControls *pControls);

#endif