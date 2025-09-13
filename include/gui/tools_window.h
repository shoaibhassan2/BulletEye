
#ifndef TOOLS_WINDOW_H
#define TOOLS_WINDOW_H

#include <globals.h>

void CreateToolsWindow(HWND hParent);

LRESULT CALLBACK ToolsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif