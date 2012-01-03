// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#include <windows.h>

#include <atlbase.h>
extern CComModule _Module;
#include <atlwin.h>
#include <atlcom.h>

#include <shellapi.h>

// disable default VC import warnings

#pragma warning (disable : 4192)
#pragma warning (disable : 4146)

#import <mshtml.tlb>
#import <shdocvw.dll>

#pragma warning (default : 4192)
#pragma warning (default : 4146)

#include "Utilities.h"

#define APPNAME "FormComplete_v1.0"

// Set up our tray Window message
#define	TRAY_CALLBACK			WM_USER + 101
#define WM_LOAD_DATA			WM_USER + 102
#define WM_SAVE_DATA			WM_USER + 103

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
