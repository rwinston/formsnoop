// ConfirmDlg.h : Declaration of the CConfirmDlg

#ifndef __CONFIRMDLG_H_
#define __CONFIRMDLG_H_

#include "resource.h"       // main symbols
#include <atlhost.h>
#include "atlcontrols.h"

/////////////////////////////////////////////////////////////////////////////
// CConfirmDlg
class CConfirmDlg : 
	public CSimpleDialog<IDD_CONFIRMDLG> // CConfirmDlg>
{
public:
	CConfirmDlg()
	{
	}

	~CConfirmDlg()
	{
		if(::IsWindow(m_hWnd))
			DestroyWindow();
	}

	enum { IDD = IDD_CONFIRMDLG };

BEGIN_MSG_MAP(CConfirmDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 1;  // Let the system set the focus
	}

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(m_hWnd, IDOK);
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(m_hWnd, IDCANCEL);
		return 0;
	}
};

#endif //__CONFIRMDLG_H_
