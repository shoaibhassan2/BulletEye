#ifndef SETTINGS_H
#define SETTINGS_H

#include <globals.h>

void SaveWindowSettings(HWND hWnd);

BOOL LoadWindowSettings(WINDOWPLACEMENT *pwp);

#endif