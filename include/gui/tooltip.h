#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <globals.h>

HWND CreateTooltip(HWND hParent);

void AddTooltip(HWND hTooltip, HWND hControl, int resourceId);

#endif