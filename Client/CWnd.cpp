// CWnd.cpp
#include "CWnd.h"


CWnd::CWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
				   LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
				   LPCTSTR menuName, int x, int y, int width, int height,
				   UINT classStyle, DWORD windowStyle, HWND hParent)
{
	char szClassName[] = "CWndClass";

	wc.cbSize        = sizeof(wc);
	wc.style         = classStyle;
	wc.lpfnWndProc   = pWndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName  = menuName;
	wc.lpszClassName = szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	// Register the window class
	if (!RegisterClassEx(&wc)) {
		char msg[100] = "Cannot register class: ";
		strcat(msg, szClassName);
		MessageBox(NULL, msg, "Error", MB_OK);
		return;
	}
	
	// Create window
	hWnd = CreateWindow(szClassName, windowName, windowStyle,
		x, y, width, height, hParent, (HMENU)NULL, hInst, NULL);       
	
	if (!hWnd) {
		char text[100] = "Cannot create window: ";
		strcat(text, windowName);
		MessageBox(NULL, text, "Error", MB_OK);
		return;
	}

	// Show window
	ShowWindow(hWnd, cmdShow); 
}
