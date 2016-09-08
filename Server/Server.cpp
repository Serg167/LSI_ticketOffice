// Server.cpp
#include <windows.h>
#include "CWnd.h"
#include "resource.h"

#define TEXT1   110
#define TEXT2   111
#define EDIT1   112
#define EDIT2   113
#define BUTTON1 114
#define BUTTON2 115


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HACCEL hAccel;
	MSG msg;
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	CWnd mainWnd("Server. Ticket office", hInstance, nCmdShow, WndProc, MAKEINTRESOURCE(IDR_MENU1), 100, 100, 600, 250);	
	while (GetMessage(&msg, NULL, 0, 0))  {
		if (!TranslateAccelerator(mainWnd.GetHWnd(), hAccel, &msg)) { 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hMutex=NULL;
	HANDLE hFileMap=NULL;
	STARTUPINFO si;
	static PROCESS_INFORMATION pi;
	BOOL success1,success2;
	HINSTANCE hInst;
	HDC hDC;
	HICON hIcon;
	HICON hIconSm;
	PAINTSTRUCT ps;
	RECT rect;
	int userReply;
	static HMENU hMenu;
	int* amount;

	hMutex=CreateMutex(NULL,false,"Mutex");
		if (!hMutex) {
			userReply = MessageBox(hWnd, "Mutex hasn't been created! Application will be shut down=(",
			"Close Application", MB_OK | MB_ICONEXCLAMATION);
		if (IDOK == userReply)
			DestroyWindow(hWnd);
		}
		hFileMap=CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), "SharedMem");
		if (!hFileMap) {
			userReply = MessageBox(hWnd, "File map hasn't been created! Application will be shut down=(",
			"Close Application", MB_OK | MB_ICONEXCLAMATION);
		if (IDOK == userReply)
			DestroyWindow(hWnd);
		}
		amount=(int*)MapViewOfFile(hFileMap,FILE_MAP_WRITE,0,0,0);

	switch (uMsg)
	{
	case WM_CREATE:
		hInst = GetModuleHandle(NULL); 
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)); 
		hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); 
		SetClassLong(hWnd, GCL_HICON, (LONG)hIcon); 
		SetClassLong(hWnd, GCL_HICONSM, (LONG)hIconSm); 
		hMenu = GetMenu(hWnd); 
		SetMenuDefaultItem(GetSubMenu(hMenu, 0), -1, FALSE); 
		CreateWindow("STATIC", "Amount of available tickets:",  WS_CHILD | WS_VISIBLE,
			20, 20, 300, 20, hWnd, HMENU(TEXT1), GetModuleHandle(NULL), NULL);
		CreateWindow("STATIC", "Number of tickets to add for sale:",  WS_CHILD | WS_VISIBLE,
			20, 60, 300, 20, hWnd, HMENU(TEXT2), GetModuleHandle(NULL), NULL);
		CreateWindow("EDIT", "0",  ES_READONLY | ES_CENTER | WS_CHILD | WS_VISIBLE,
			350, 20, 50, 20, hWnd, HMENU(EDIT1), GetModuleHandle(NULL), NULL);
		CreateWindow("EDIT", "0",  ES_NUMBER | ES_CENTER | WS_CHILD | WS_VISIBLE,
			350, 60, 50, 20, hWnd, HMENU(EDIT2), GetModuleHandle(NULL), NULL);
		CreateWindow("BUTTON", "Add",  ES_CENTER | WS_CHILD | WS_VISIBLE,
			450, 55, 60, 30, hWnd, HMENU(BUTTON1), GetModuleHandle(NULL), NULL);
		CreateWindow("BUTTON", "Close Application",  ES_CENTER | WS_CHILD | WS_VISIBLE,
			380, 120, 180, 30, hWnd, HMENU(BUTTON2), GetModuleHandle(NULL), NULL);
	case WM_CTLCOLORSTATIC:
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockObject(NULL_BRUSH);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) 
		{ 
		case BUTTON1:
			*amount=*amount+GetDlgItemInt(hWnd, EDIT2, &success1, true);
			SetDlgItemInt(hWnd, EDIT1, *amount, true);
			break;
		case BUTTON2:			
			userReply = MessageBox(hWnd, "Are you sure?",
			"Close Application", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == userReply)
			DestroyWindow(hWnd);
			break;
		case ID_EXIT: 
			userReply = MessageBox(hWnd, "Are you sure?",
			"Close Application", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == userReply)
			DestroyWindow(hWnd);
			break;
		case ID_CREATECLIENTAPP:
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			success2 = CreateProcess("Client.exe", NULL, FALSE, FALSE, FALSE,
				DETACHED_PROCESS, NULL, NULL, &si, &pi);
			if (!success2)
				MessageBox(hWnd, "Error of CreateProcess", NULL, MB_OK);
			break;
		case ID_ABOUT:
			MessageBox(hWnd, "LSI CritSect_App by means of shared memory\nVersion 1.0\nCopyright. LSI. 2016.", 
								"About LSI CS_App", MB_OK); 
			break;
		default:
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE); 
		break;
	case WM_PAINT:
		SetDlgItemInt(hWnd, EDIT1, *amount, true);
		hDC = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rect);
		EndPaint(hWnd, &ps);
		break;

	case WM_CLOSE:
		userReply = MessageBox(hWnd, "Are you sure?",
			"Close Application", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == userReply)
			DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		CloseHandle(hMutex);
		UnmapViewOfFile(&amount);
		CloseHandle(hFileMap);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
