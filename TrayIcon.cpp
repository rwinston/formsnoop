// This class was originally written by Don Metzner
// Modified by Rory Winston

#include "stdafx.h"
#include "TrayIcon.h"


CTrayIcon::CTrayIcon(HINSTANCE hInstance)
{
   m_szTooltip[0] = '\0';
   m_hIcon = NULL;
   m_uID = 0;
   m_hWnd = NULL;
   m_bTray = false;

   // We are not using MFC, so we need to use the Win32 Loadicon()
   // function. Hence we need to be passed a HINSTANCE for the app
   m_hInstance = hInstance;
}

CTrayIcon::~CTrayIcon()
{
   RemoveTrayIcon();
}

void CTrayIcon::SetID(UINT uID)
{
   m_uID = uID;
}

void CTrayIcon::SetCallbackWnd(HWND hWnd)
{
	ATLASSERT( hWnd != NULL);
	m_hWnd = hWnd;
}

void CTrayIcon::SetIcon(UINT uiResID, bool bShow)
{
   HICON	hIcon;

   hIcon = LoadIcon(m_hInstance, (LPCTSTR)MAKEINTRESOURCE(uiResID));
   
   if (hIcon != NULL)
      SetIcon(hIcon, bShow);
   else
	   dprintf(_T("Could not load Tray Icon\n"));
}

void CTrayIcon::SetIcon(HICON hIcon, bool bShow)
{
   dprintf(_T("Calling CTrayIcon::SetIcon()\n"));
   
   m_hIcon = hIcon;

   if (!bShow)
      return;

   if (!m_bTray)
      AddTrayIcon();
   else
      ModifyTrayState();
}

void CTrayIcon::Enable(bool bEnable)
{
}

void CTrayIcon::Show(bool bShow)
{
   if (bShow)
      AddTrayIcon();
   else
      RemoveTrayIcon();
}

void CTrayIcon::SetTooltip(char *szTooltip)
{
   strcpy(m_szTooltip, szTooltip);
}

void CTrayIcon::AddTrayIcon()
{
   if (!m_hIcon)
      return;

   if (!m_bTray)
   {
	   dprintf(_T("Sending NIM_ADD message...\n"));
	   TrayMessage(NIM_ADD);
	   m_bTray = true;
   }
}

void CTrayIcon::RemoveTrayIcon()
{
	
   if (m_bTray)
   {
      dprintf(_T("Sending NIM_REMOVE message...\n"));
	  TrayMessage(NIM_DELETE);
	  m_bTray = false;
   }
}

void CTrayIcon::ModifyTrayState()
{
   if (m_bTray)
      TrayMessage(NIM_MODIFY);
}

void CTrayIcon::NotifyLeftButton(POINT& pt, CTrayIcon* pThis)
{
}

void CTrayIcon::NotifyRightButton(POINT& pt, CTrayIcon* pThis)
{
  HMENU hMenu;

  hMenu = CreatePopupMenu();

  AppendMenu(hMenu, MF_STRING, 1000, "&Exit FormSnoop");
  AppendMenu(hMenu, MF_STRING, 1001, "&Load Configuration");
  AppendMenu(hMenu, MF_STRING, 1002, "&Save Configuration");

  // we need to make the invisible command handler window
  // the foreground window, or the popup menu will not close
  SetForegroundWindow(pThis->m_hWnd);

  int nCmd = TrackPopupMenuEx(hMenu, TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
	  pt.x, pt.y, pThis->m_hWnd, NULL);

  switch(nCmd){
	  
  case 1000:
	  SendMessage(pThis->m_hWnd, WM_CLOSE, 0, 0);
	  break;

  case 1001:
	  // Load
	  SendMessage(pThis->m_hWnd, WM_LOAD_DATA, 0, 0);
	  break;

  case 1002:
	  // Save
	  SendMessage(pThis->m_hWnd, WM_SAVE_DATA, 0, 0);
	  break;
  }

  DestroyMenu(hMenu);


}


// Message handler callback function for our tray icon window
void CTrayIcon::TrayCallback(WPARAM wParam, LPARAM lParam, CTrayIcon* pThis)
{
   UINT uID;
   UINT uMouseMsg;

   uID = (UINT) wParam;
   uMouseMsg = (UINT) lParam;
   POINT pt;

   GetCursorPos(&pt);

   dprintf(_T("CTrayIcon::TrayCallback called\n"));

   if (uMouseMsg == WM_LBUTTONDOWN)
   {
	   //		NotifyLeftButton(pt);
   }
   else if (uMouseMsg == WM_LBUTTONDBLCLK)
   {
	   NotifyLeftButton(pt, pThis);
   }
   else if (uMouseMsg == WM_RBUTTONDOWN)
   {
	   NotifyRightButton(pt, pThis);
   }
}

// Message handler that notifies the Shell about our tray icon status
void CTrayIcon::TrayMessage(UINT message)
{
   NOTIFYICONDATA tnd;

   memset(&tnd, 0, sizeof(tnd));
   strcpy(tnd.szTip, m_szTooltip);

   switch (message)
   {
      case NIM_ADD:
	 tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	 break;

      case NIM_MODIFY:
	 tnd.uFlags = NIF_TIP | NIF_ICON;
	 break;

     case NIM_DELETE:
	 tnd.uFlags = 0;
	 break;
   }

   tnd.uID 				= (UINT) m_uID;
   tnd.cbSize			= sizeof(NOTIFYICONDATA);
   tnd.hWnd				= m_hWnd;
   tnd.uCallbackMessage = TRAY_CALLBACK;
   tnd.hIcon			= m_hIcon;

   dprintf(_T("Calling Shell_NotifyIcon...\n"));

   Shell_NotifyIcon(message, &tnd);
}

