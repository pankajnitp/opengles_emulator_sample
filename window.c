/* 
 * window.c
 * Platform-specific window setup.
 */

#include "window.h"
#include "utility.h"
extern int runTransition;
/* 
 * create_window(): Set up Windows specific bits.
 *
 * uiWidth:	 Width of window to create.
 * uiHeight:	Height of window to create.
 *
 * Returns:	 Device specific window handle.
 */
HWND create_window(int uiWidth, int uiHeight) {
	WNDCLASS wc;
	RECT wRect;
	HWND sWindow;
	HINSTANCE hInstance;

	wRect.left = 0L;
	wRect.right = (long)uiWidth;
	wRect.top = 0L;
	wRect.bottom = (long)uiHeight;

	hInstance = GetModuleHandle(NULL);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)process_window;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "OGLES";

	RegisterClass(&wc);

	AdjustWindowRectEx(&wRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

	sWindow = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, "OGLES", "main", WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, uiWidth, uiHeight, NULL, NULL, hInstance, NULL);

	ShowWindow(sWindow, SW_SHOW);
	SetForegroundWindow(sWindow);
	SetFocus(sWindow);

	return sWindow;
}

/*
 * process_window(): This function handles Windows callbacks.
 */
LRESULT CALLBACK process_window(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
	switch(uiMsg) {
		case WM_CLOSE:
				PostQuitMessage(0);
				return 0;

		case WM_ACTIVATE:
		case WM_KEYDOWN:
			if (wParam == VK_SPACE)
			{
				runTransition = !runTransition;
				InvalidateRect(hWnd, NULL, FALSE);
			}
		case WM_KEYUP:
			break;
		case WM_LBUTTONDOWN:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			printf("Mouse clicked at (%d, %d)\n", x, y);
			SceneMouseClickedAt(x, y);
			break;
		}
		case WM_SIZE:
				return 0;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}
