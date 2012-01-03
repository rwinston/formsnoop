// FormSnoop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FCWindow.h"

// Application code for FormSnoop


// Global variable definitions
CComModule	_Module;

UINT		g_uHotKeyRead		= 0;
UINT		g_uHotKeyWrite		= 0;
HANDLE		g_hMutex;

// global function definitions

bool IsAppAlreadyRunning();
void CleanUp();

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	CoInitialize(NULL);	
	_Module.Init(NULL, hInstance);

	if(IsAppAlreadyRunning()) {
		dprintf("App is running already, exiting...\n");
		return -1;
	}
	
	
	// Create our app window
	CFCWindow MainWnd(hInstance);
	RECT rcPos = { 0, 0, 200, 200 };
	MainWnd.Create(NULL, rcPos, _T("FormSnoop Application Window"));
	MainWnd.SetAppHotKeys(g_uHotKeyRead, g_uHotKeyWrite);
	MainWnd.ShowWindow(SW_HIDE);

	// Win32 message loop

	MSG msg;

	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Cleanup code
	MainWnd.RemoveHotKeys(g_uHotKeyRead, g_uHotKeyWrite);
	CleanUp();
	CoUninitialize();

	return 0;
}


bool IsAppAlreadyRunning()
{
	g_hMutex = CreateMutex(NULL, TRUE, APPNAME);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, "FormSnoop is already running on this machine.", "FormSnoop", MB_ICONINFORMATION | MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		return true;
	}
	else if (g_hMutex == INVALID_HANDLE_VALUE)
	{
		dprintf("Can't create mutex! Error = %d\n", GetLastError());
	}
	else
	{
		dprintf("Mutex created OK\n");
	}

	return false;

}

void CleanUp()
{
	dprintf("Releasing mutex...\n");
	ReleaseMutex(g_hMutex);
	CloseHandle(g_hMutex);
}


