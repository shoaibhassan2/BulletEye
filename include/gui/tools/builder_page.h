#ifndef BUILDER_PAGE_H
#define BUILDER_PAGE_H

#include <globals.h>

typedef struct
{
    HWND hwndBuilderLabel;
} BuilderControls;

void CreateBuilderPage(HWND hParent, RECT tabRect, BuilderControls *pControls);
void ShowBuilderPage(const BuilderControls *pControls, BOOL bShow);

#endif