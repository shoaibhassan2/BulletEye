#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <globals.h>

void CreateAboutWindow(HWND hParent);

LRESULT CALLBACK AboutWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif