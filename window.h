
#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

HWND create_window(int uiWidth, int uiHeight);
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

#endif
