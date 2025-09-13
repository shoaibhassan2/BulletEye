#ifndef NETWORK_SETTINGS_WINDOW_H
#define NETWORK_SETTINGS_WINDOW_H

#include <globals.h>

void CreateNetworkSettingsWindow(HWND hParent);

LRESULT CALLBACK NetworkSettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif