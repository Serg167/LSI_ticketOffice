// Client.cpp
#include <windows.h>
#include "CWnd.h"
#include "resource.h"
#include <iostream>



#define TEXT1   110
#define EDIT1   111
#define BUTTON1 112
#define BUTTON2 113


//Function which imitates CRITICAL SECTION
int CritSect(HWND,HINSTANCE);

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HACCEL hAccel;
	MSG msg;
	hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	CWnd mainWnd("Client. Ticket office", hInstance, nCmdShow, WndProc, MAKEINTRESOURCE(IDR_MENU1), 150, 150, 600, 250);	
	while (GetMessage(&msg, NULL, 0, 0))  {
		if (!TranslateAccelerator(mainWnd.GetHWnd(), hAccel, &msg)) { 
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
	int* amount;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hFileMap;
	BOOL success1;
	HINSTANCE hInst;
	HDC hDC;
	HICON hIcon;
	HICON hIconSm;
	PAINTSTRUCT ps;
	RECT rect;
	int userReply;
	static HMENU hMenu;
	hFileMap=OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "SharedMem");
	if (!hFileMap) {
		userReply = MessageBox(hWnd, "File map hasn't been opened! Application will be shut down=(",
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
		CreateWindow("BUTTON", "Buy tickets",  ES_CENTER | WS_CHILD | WS_VISIBLE,
			200, 55, 200, 30, hWnd, HMENU(BUTTON1), GetModuleHandle(NULL), NULL);
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
			hInst = GetModuleHandle(NULL); 
			CritSect(hWnd,hInst);
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
		UnmapViewOfFile(amount);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}



int CritSect(HWND hWnd, HINSTANCE hInst)
{
	int userReply;
	HANDLE hMutex;
	hMutex=OpenMutex(SYNCHRONIZE,false,"Mutex");
	if (!hMutex) {
		userReply = MessageBox(hWnd, "Mutex hasn't been opened! ",
		"Close Application", MB_OK | MB_ICONEXCLAMATION);
	if (IDOK == userReply)
		DestroyWindow(hWnd);
	}
	if (WaitForSingleObject(hMutex,1)!=WAIT_OBJECT_0) {
		MessageBox(hWnd, "Someone is buying tickets right now! =( ",
			"Please wait!", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// Call dialog
	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc); 
	ReleaseMutex(hMutex);	
	CloseHandle(hMutex);
	return 0;		
}


BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL success1;
	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_EDIT2, *amount, true);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON1:
			if (*amount<GetDlgItemInt(hDlg, IDC_EDIT1, &success1, true)) {
				MessageBox(hDlg, "There are fewer tickets available than you want to buy! =( ",
					"Sorry!", MB_OK | MB_ICONEXCLAMATION);
				break;
			} 
			else {
				*amount=*amount-GetDlgItemInt(hDlg, IDC_EDIT1, &success1, true);
				char text[256];
				sprintf(text, "You have just bought %i tickets!", GetDlgItemInt(hDlg, IDC_EDIT1, &success1, true));
				MessageBox(hDlg, text, "Congrats!", MB_OK | MB_ICONEXCLAMATION);
				HWND hServ=FindWindow(NULL,"Server. Ticket office");
				HWND hClient=FindWindow(NULL,"Client. Ticket office");
				SendMessage(hClient,WM_PAINT, NULL, NULL);
				SendMessage(hServ,WM_PAINT, NULL, NULL);
				EndDialog(hDlg, 0);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}