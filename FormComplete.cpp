// FormComplete.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "FCWindow.h"

CComModule _Module;


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	CoInitialize(NULL);

	// Hotkey (un)registration doent work properly when
	// the atom is a class member variable. making it global 
	// seems to cure it, think its due to threading issues
	UINT hotKeyAtom = 0;

	_Module.Init(NULL, hInstance);
	CFCWindow MainWnd;
	
	RECT rcPos = { 0, 0, 400, 200 };

	MainWnd.Create(NULL, rcPos, _T("Test"));
	MainWnd.ShowWindow(SW_SHOW);
	hotKeyAtom = MainWnd.setupHotKey();

	// Win32 message loop
	MSG msg;
	
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	MainWnd.removeHotKey(hotKeyAtom);
	
	CoUninitialize();
	return 0;
}



