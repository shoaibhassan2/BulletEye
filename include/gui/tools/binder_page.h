#ifndef BINDER_PAGE_H
#define BINDER_PAGE_H

#include <globals.h>

typedef struct
{
    HWND hwndBinderLabel;
} BinderControls;

void CreateBinderPage(HWND hParent, RECT tabRect, BinderControls *pControls);
void ShowBinderPage(const BinderControls *pControls, BOOL bShow);

#endif