#pragma once
// ÑWnd.h
#include <windows.h>
#include "msvcr100d.dll"

class CWnd {
public:	
	CWnd(LPCTSTR windowName, HINSTANCE hInst, int cmdShow,
		LRESULT (WINAPI *pWndProc)(HWND,UINT,WPARAM,LPARAM),
		LPCTSTR menuName = NULL,
		int x = CW_USEDEFAULT, int y = 0,
		int width = CW_USEDEFAULT, int height = 0,
		UINT classStyle =  CS_HREDRAW | CS_VREDRAW,
		DWORD windowStyle = WS_OVERLAPPEDWINDOW,
		HWND hParent = NULL);

	HWND GetHWnd() { return hWnd; }
protected:
	HWND hWnd;  
	WNDCLASSEX wc;
};
