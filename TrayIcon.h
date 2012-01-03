// This class is a modified version of one written by Brad Metzler
// Modified by Rory Winston 08/10/2001

#if _MSC_VER >= 1000
#pragma once
#endif 

class CTrayIcon  
{
public:
	CTrayIcon(HINSTANCE hInstance);
	virtual ~CTrayIcon();

	void			SetCallbackWnd(HWND hWnd);
	void			SetID(UINT uID);
	void			SetIcon(UINT uiResID, bool bShow = TRUE);
	void			SetIcon(HICON hIcon, bool bShow = TRUE);
	void			SetTooltip(char *tooltip);

	void			Enable(bool bEnable);
	void			Show(bool bShow);

	static void		TrayCallback(WPARAM wParam, LPARAM lParam, CTrayIcon* pThis);
	void			RemoveTrayIcon();

protected:

	char			m_szTooltip[255];
	HICON			m_hIcon;
	UINT			m_uID;
	HWND			m_hWnd;
	HINSTANCE		m_hInstance;
	bool			m_bTray;

	static void		NotifyLeftButton(POINT&, CTrayIcon* pThis);
	static void		NotifyRightButton(POINT&, CTrayIcon* pThis);

	void			AddTrayIcon();
	void			ModifyTrayState();
	void			TrayMessage(UINT message);
};


