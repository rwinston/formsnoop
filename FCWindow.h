#ifndef FCWINDOW_H
#define FCWINDOW_H

// for _ltoa
#include <stdlib.h>
#include "PageData.h"
#include "ListIterator.h"
#include "ReverseListIterator.h"
#include "ConfirmDlg.h"
#include "TrayIcon.h"
#include <fstream.h>
#include <iostream.h>


// TODO: Put in persistence
// TODO: Fix issues with checkboxes and radio buttons (DONE)
// This is a hack to allow us to insert ATL objects into the project

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

const int MAX_PAGES = 64;

class CFCWindow : 
public CWindowImpl<CFCWindow, CWindow, CWinTraits<WS_SIZEBOX | WS_CAPTION | WS_POPUPWINDOW, 0> >, 
public Serializable
{
public:
	CFCWindow(HINSTANCE hInstance) : 
	  numPages(MAX_PAGES), m_uHotKeyRead(0), m_uHotKeyWrite(0), m_hInstance(hInstance)
	  {
		  m_trayIcon = new CTrayIcon(hInstance);
		  _tcscpy(strFileName, _T("c:\\objects.dat"));
	  }
	  
	  ~CFCWindow()
	  {
		  if(::IsWindow(m_hWnd)) {
			  DestroyWindow();
		  }
		  
		  dprintf(_T("Calling the destructor for the tray object...\n"));
		  
		  delete m_trayIcon;
	  }
	  
	  BEGIN_MSG_MAP(CFCWindow)
		  MESSAGE_HANDLER(WM_CLOSE, OnClose)
		  MESSAGE_HANDLER(WM_HOTKEY, OnHotKey)
		  MESSAGE_HANDLER(TRAY_CALLBACK, OnTrayCallback)
		  MESSAGE_HANDLER(WM_LOAD_DATA, OnLoadData)
		  MESSAGE_HANDLER(WM_SAVE_DATA, OnSaveData)
		  END_MSG_MAP()
		  
		  
	  int SetAppHotKeys(UINT& KeyRead, UINT& KeyWrite);
	  int RemoveHotKeys(UINT& KeyRead, UINT& KeyWrite);
	  bool exists(CComBSTR& URL);
	  int serialize(ofstream& archive);
	  int deserialize(ifstream& archive);
	  
private:
	
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLoadData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSaveData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTrayCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	
	void initTrayIcon();
	int	 ReadForm();
	int  WriteForm();
	bool GetIEServer(HWND ParentWnd, HWND& IEServer);
	bool GetDocPointerFromHWND(HWND IEServer, MSHTML::IHTMLDocument2Ptr& spDoc);
	bool processDocument(MSHTML::IHTMLDocument2Ptr& spDoc, int context);
	bool processFormElements(MSHTML::IHTMLFormElementPtr& spForm, PageData* currentPage, int context);
	PageData* GetPageDataFromURL(CComBSTR& URL);
	
	UINT		m_uHotKeyRead, m_uHotKeyWrite;
	enum		{ READ = 0xf, WRITE, REWRITE };
	int			numPages;
	HICON		m_hTrayIcon;
	TCHAR		strFileName[64];
	
protected:
	
	List<PageData*> Pages;
	HINSTANCE		m_hInstance;
	CTrayIcon*		m_trayIcon;
};

int CFCWindow::serialize(ofstream& archive)
{
	// write the number of PageData structures from our list
	int nPages = Pages.getCount();
	
	dprintf(_T("CFCWIndow::SERIALIZE::Writing %d bytes to stream, value is %d..\n"), sizeof(nPages), nPages);
	archive.write(reinterpret_cast<char*>(&nPages), sizeof(nPages));	
	ListIterator<PageData*> iter(Pages);
		
	// Serialize the sublist of PageData elements
	for(iter.reset(); !iter; ++iter)
	{
		iter()->serialize(archive);
	}
		
	return 0;
}

int CFCWindow::deserialize(ifstream& archive)
{
	// empty list, read the number of PageData structures and place in our list
	// TODO: we really SHOULDNT empty the list until the whole lot has been 
	// read in successfully, but need to get assignment operator for List<T>
	// sorted out first...
	int nPages = 0;
	Pages.empty();

	// Read in the number of PageData* structures we will be dealing with
	archive.read(reinterpret_cast<char*>(&nPages), sizeof(nPages));
	dprintf(_T("CFCWindow::DESERIALIZE::Read %d bytes from stream, value is %d\n"), sizeof(nPages), nPages);

	// TODO: DANGER Will Robinson! Possible memory leak here
	// not sure if each new() will have a corresponding delete()
	// need to check List definition
	for(int i = 0; i < nPages; ++i) {
			PageData* pageData = new PageData();
			pageData->deserialize(archive);
			Pages.insert(pageData);
		}
	
	return 0;
}

LRESULT CFCWindow::OnLoadData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	dprintf(_T("WM_LOAD_DATA received\n"));
	ifstream archive(strFileName, ios::binary);
	deserialize(archive);
	archive.close();
	// empty current list and load in a new one
	return TRUE;
}

LRESULT CFCWindow::OnSaveData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	dprintf(_T("WM_SAVE_DATA received\n"));
	ofstream archive(strFileName, ios::binary);
	// Save current PageData structure
	serialize(archive);
	archive.close();
	return TRUE;
}
	

LRESULT CFCWindow::OnTrayCallback(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	dprintf(_T("TRAY_CALLBACK message received...msg id is %d\n"), TRAY_CALLBACK);
	m_trayIcon->TrayCallback(wParam, lParam, m_trayIcon);
	return (TRUE);
}



void CFCWindow::initTrayIcon()
{
	dprintf(_T("Setting up tray icon...\n"));
	
	m_hTrayIcon = (HICON) LoadIcon(m_hInstance, (LPCTSTR)MAKEINTRESOURCE(ID_TRAYICON));
	m_trayIcon->SetID(ID_TRAYICON) ;
	m_trayIcon->SetCallbackWnd(m_hWnd) ;
	m_trayIcon->SetTooltip(_T("FormSnoop is running")) ;
	m_trayIcon->SetIcon(m_hTrayIcon, TRUE) ;
	m_trayIcon->Enable(TRUE) ;
	m_trayIcon->Show(TRUE) ;
}


PageData* CFCWindow::GetPageDataFromURL(CComBSTR& URL)
{
	USES_CONVERSION;
	ListIterator<PageData*> pageIter(Pages);
	
	for(pageIter.reset(); !pageIter; ++pageIter)
	{
		if(_tcsicmp(W2T(pageIter()->URL), W2T(URL.m_str)) == 0)
			return pageIter();
	}
	
	dprintf(_T("Cannot find PageData* structure but URL should exist! Confused...\n"));
	return NULL;
}


// return true if we can locate the current URL in the list of
// PageData structures
bool CFCWindow::exists(CComBSTR& URL)
{
	USES_CONVERSION;
	ListIterator<PageData*> pageIter(Pages);
	
	if(Pages.getCount() == 0)
		return false;
	
	for(pageIter.reset(); !pageIter; ++pageIter)
	{
		if(_tcsicmp(W2T(pageIter()->URL), W2T(URL.m_str)) == 0)
			return true;
	}
	
	return false;
	
}

// Get a handle to the child IE Server Window
bool CFCWindow::GetIEServer(HWND ParentWnd, HWND& IEServer)
{
	
	IEServer = FindWindowEx( ParentWnd, NULL, _T("Shell DocObject View"), NULL );
	
	if ( IEServer )
		IEServer = FindWindowEx( IEServer, NULL, _T("Internet Explorer_Server"), NULL );
	
	if(IEServer)
		return true;
	else
		return false;
	
}


bool CFCWindow::processFormElements(MSHTML::IHTMLFormElementPtr& spForm, PageData* currentPage, int context)
{
	long numFormElements;
	HRESULT hr;
	int count;
	
	ReverseListIterator<FormElement*>* formIter;
	
	if(context == WRITE && exists(currentPage->URL)) {
		formIter = new ReverseListIterator<FormElement*>(*(currentPage->elements));
		count = currentPage->elements->getCount();
		dprintf(_T("currentPage->elements->getCount() = %d\n"), count);
	}
	
	USES_CONVERSION;
	
	// Count the forms
	spForm->get_length(&numFormElements);
	dprintf(_T("Number of elements in form: %ld\n"), numFormElements);
	
	
	// For each element in the form
	for(long frmIndex = 0; frmIndex < numFormElements; frmIndex++) {
		
		// retrieve a pointer to the form element
		_variant_t var((long)frmIndex, VT_I4);
		IDispatchPtr spformElem;
		hr = spForm->raw_item(var,var,&spformElem);
		
		if(hr != S_OK) {
			dprintf(_T("Couldn't get a pointer to a form element!\n"));
			return false;
		}
		
		// Convert the input to a <SELECT> pointer
		MSHTML::IHTMLSelectElementPtr spiSelect(spformElem);
		
		if(spiSelect) {
			CComBSTR txtVal;
			CComBSTR txtType;
			spiSelect->get_type((BSTR*)&txtType);
			// Not valid for SELECT elements
			// spiSelect->get_value((BSTR*)&txtVal);
			dprintf(_T("Got a SELECT element , type %ls, value is %ls\n"), (const wchar_t*)txtType, (const wchar_t*)txtVal);
			dprintf(_T("Selected index is %d\n"), spiSelect->selectedIndex);
			
			
			// Write SELECT element's name and selected index
			// instead of name and value
			if(context == READ) {
				long value = spiSelect->selectedIndex;
				
				/*
				wchar_t buffer[32];
				_ltow(value, buffer, 10);
				wcscpy(txtVal.m_str, buffer);
				*/
				
				char buffer[32];
				ltoa(value, buffer, 10);
				txtVal = buffer;
				
				currentPage->elements->insert(new FormElement(txtType, txtVal));
			}
			else
			{
				dprintf(_T("Writing %ls to SELECT element...\n"), (const wchar_t*)(*formIter)()->value.m_str);
				long value = _wtol((const wchar_t*)(*formIter)()->value.m_str);
				
				spiSelect->put_selectedIndex(value);
				//spiSelect->put_value((BSTR)(*formIter)()->value.m_str);
				++(*formIter);
			}
			
			continue;
		}
		
		// Convert the input to a <TEXTAREA> pointer
		MSHTML::IHTMLTextAreaElementPtr spiTextArea(spformElem);
		if(spiTextArea) {
			CComBSTR txtVal;
			CComBSTR txtType;
			spiTextArea->get_type((BSTR*)&txtType);
			spiTextArea->get_value((BSTR*)&txtVal);
			dprintf(_T("Got a TEXTAREA element , type %ls, value is %ls\n"), (const wchar_t*)txtType, (const wchar_t*)txtVal);
			
			if(context == READ) {
				currentPage->elements->insert(new FormElement(txtType, txtVal));
			}
			else
			{
				dprintf(_T("Writing %ls to TEXTAREA element...\n"), (const wchar_t*)(*formIter)()->value.m_str);
				spiTextArea->put_value((BSTR)(*formIter)()->value.m_str);
				++(*formIter);
			}
			
			continue;
		}
		
		
		// Buttons (just ignore, we dont want to break them)
		MSHTML::IHTMLInputButtonElementPtr spiButton(spformElem);
		if(spiButton) {
			CComBSTR txtVal;
			CComBSTR txtType;
			spiButton->get_type((BSTR*)&txtType);
			spiButton->get_value((BSTR*)&txtVal);
			dprintf(_T("Got a BUTTON element, type %ls, value is %ls\n"), (const wchar_t*)txtType, (const wchar_t*)txtVal);
			continue;
		}
		
		// Option buttons and checkboxes
		MSHTML::IHTMLOptionButtonElementPtr spiOption(spformElem);
		if(spiOption) {
			CComBSTR txtVal;
			CComBSTR txtType;
			VARIANT_BOOL vbChecked = false;
			spiOption->get_type((BSTR*)&txtType);
			spiOption->get_value((BSTR*)&txtVal);
			HRESULT hr = spiOption->get_checked(&vbChecked);
			
			if(FAILED(hr)) {
				dprintf("Can't get checked status - errCode = %d\n", GetLastError());
			}
			
			if(vbChecked) { 
				dprintf("Input is checked");
				txtVal = "checked";
			}
			else
				txtVal = "unchecked";

			dprintf(_T("Got a RADIO/CHECKBOX element %ls, value is %ls\n"), (const wchar_t*)txtType, vbChecked ? L"checked" : L"unchecked");
			
			if(context == READ) {
				currentPage->elements->insert(new FormElement(txtType, txtVal));
			}
			else
			{
				dprintf(_T("Writing %ls to INPUT element...\n"), (const wchar_t*)(*formIter)()->value.m_str);
				
				//hr = spiOption->put_value((BSTR)(*formIter)()->value.m_str);
				if(wcscmp((*formIter)()->value.m_str, L"checked") == 0) {
					vbChecked = true;
					spiOption->put_checked(vbChecked);
				}
				else {
					vbChecked = false;
					spiOption->put_checked(vbChecked);
				}
				
				// Remember to dereference pointer before calling any overloaded
				// operators on it!!! 
				++(*formIter);
			}
			
			continue;
		}	
		
	
	// Everything else	
	MSHTML::IHTMLInputElementPtr spiInput(spformElem);
	if(spiInput) {
		CComBSTR txtVal;
		CComBSTR txtType;
		spiInput->get_type((BSTR*)&txtType);
		spiInput->get_value((BSTR*)&txtVal);
		dprintf(_T("Got an INPUT element %ls, value is %ls\n"), (const wchar_t*)txtType, (const wchar_t*)txtVal);
		
		if(context == READ) {
			currentPage->elements->insert(new FormElement(txtType, txtVal));
		}
		else
		{
			dprintf(_T("Writing %ls to INPUT element...\n"), (const wchar_t*)(*formIter)()->value.m_str);
			
			hr = spiInput->put_value((BSTR)(*formIter)()->value.m_str);
			
			// Remember to dereference pointer before calling any overloaded
			// operators on it!!! 
			++(*formIter);
		}
		
		continue;
	}	
	}
	
	// Put the current page data structure into the list of PageData structures
	if(context == READ)
		Pages.insert(currentPage);
	
	return true;
}


bool CFCWindow::processDocument(MSHTML::IHTMLDocument2Ptr& spDoc, int context)
{
	HRESULT hr;
	MSHTML::IHTMLElementCollectionPtr spElemColl;
	PageData* currentPage;
	
	
	// Retrieve the page URL and check if it exists
	CComBSTR bstrTempURL;
	spDoc->get_url((BSTR*)&(bstrTempURL.m_str));
	
	if(context == READ && exists(bstrTempURL))
	{
		dprintf(_T("URL %ls exists in list already!\n"), bstrTempURL.m_str);
		
		CConfirmDlg confirmDlg;
		int retVal = confirmDlg.DoModal();
		
		if(retVal == IDOK) {
			dprintf(_T("User has selected OVERWRITE\n"));
			currentPage = GetPageDataFromURL(bstrTempURL);
			ATLASSERT(currentPage != NULL);
			
			// if user wants to overwrite page form data, call empty() on form element list
			// and replace
			
		}
		else {
			// user doesnt want to overwrite, so exit
			dprintf(_T("User doent't want to overwrite existing data, backing out...\n"));
			return false;
		}
		
	}
	
	// Are there any forms in the document?
	hr = spDoc->get_forms(&spElemColl);
	
	if(hr != S_OK) {
		dprintf(_T("Can't get any forms from document\n"));
		return false;
	}
	else 
	{
		long lenColl;
		spElemColl->get_length(&lenColl);
		dprintf(_T("Number of forms in document %ld\n"),lenColl);
		
		// No forms to process, so return 
		if(lenColl == 0) 
			return false;
		
		// set up a new PageData  structure to hold the page details	
		if(context == READ) {
			currentPage = new PageData();
			currentPage->URL.Append(bstrTempURL);
		}
		else 
		{
			// Get existing page details	
			dprintf(_T("Retrieving PageData for URL %ls\n"), (const wchar_t*)bstrTempURL.m_str);
			currentPage = GetPageDataFromURL(bstrTempURL);
			ATLASSERT(currentPage != NULL);
		}
		
		
		// Get a pointer to each form and process its elements
		
		for(long frmIndex = 0; frmIndex < lenColl; ++frmIndex) {
			
			_variant_t var(frmIndex, VT_I4);
			IDispatchPtr spDispElem;
			hr = spElemColl->raw_item(var,var,&spDispElem);
			
			if(hr != S_OK) {
				dprintf(_T("Failed to retrieve <form> element from document\n"));
				return false;
			}
			
			// Try to get a pointer to the form
			MSHTML::IHTMLFormElementPtr spForm(spDispElem);
			if(spForm == NULL) {
				dprintf(_T("Can't get access to <form> element %ld\n"), frmIndex);
				return false;
			}
			else
			{		
				// Now process the form elements
				dprintf(_T("Got a <form> pointer, calling processFormElements, context = %d\n"), context);			
				return processFormElements(spForm, currentPage, context);
			}	
			
		}
		
		return true;
	}	
	
}



bool CFCWindow::GetDocPointerFromHWND(HWND IEServer, MSHTML::IHTMLDocument2Ptr& spDoc)
{
	DWORD		result;
	HRESULT		hr;
	HMODULE		hModule;
	
	hModule = LoadLibrary("Oleacc.dll");
	
	if(hModule) {
		
		dprintf("Got an instance handle to Oleacc.dll\n");
		
		typedef long (WINAPI *OBJFNPTR)(long, const GUID*, unsigned int, void**);
		OBJFNPTR FuncPtr;
		
		FuncPtr = (OBJFNPTR)GetProcAddress(hModule, "ObjectFromLresult");
		
		if(FuncPtr) {
			
			UINT msg = RegisterWindowMessage(_T("WM_HTML_GETOBJECT"));
			if(msg == 0)
				dprintf(_T("Failed to register MSG WM_HTML_GETOBJECT, error=%d\n"), GetLastError());
			
			SendMessageTimeout(IEServer, msg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, &result);
			if(result == 0L) 
				dprintf(_T("SendMessage() Result: %d, error=%d\n"), result, GetLastError());
			
			hr = FuncPtr(result, &(__uuidof(MSHTML::IHTMLDocument2)), 0, (void**)&spDoc);
			
			if(hr == S_OK) {
				dprintf(_T("Got an IHTMLDocument2 pointer from IE Server HWND\n"));
				return true;
			}
			else 
			{
				dprintf(_T("Failed to get IHTMLDoc2 pointer from a HWND, hr=%ld\n"), hr);
				return false;
			}
			
		}
		
		dprintf(_T("Freeing library...\n"));
		FreeLibrary(hModule);
	}
	
	return false;	
}


// Get the active window
// if(window is an IE window)
//   then process the window and extract form elements

int CFCWindow::ReadForm()
{
	HWND hWndActive		= GetForegroundWindow();
	HWND hWndIEServer	= NULL;
	MSHTML::IHTMLDocument2Ptr	spDoc;

	if(!GetIEServer(hWndActive, hWndIEServer)) {
		dprintf(_T("Cannot get a pointer to IE Server from HWND %d\n"), hWndActive);
		return -1;
	}
	
	dprintf(_T("Got IE Server HWND: %d\n"), hWndIEServer);
	
	if(!GetDocPointerFromHWND(hWndIEServer, spDoc)) {
		dprintf(_T("Can't get document pointer from HWND!\n"));
		return -1;
	}
	
	if(!processDocument(spDoc, READ)) {
		dprintf(_T("processDocument failed.\n"));
		return -1;
	}
	
	return 0;
}

// Get Active Window
// if (window is IE)
// Get Document Pointer from Active Window
// Iterate through list of pages
// if(match)
// write data to page

int CFCWindow::WriteForm()
{
	
	HWND hWndActive		= GetForegroundWindow();
	HWND hWndIEServer	= NULL;
	MSHTML::IHTMLDocument2Ptr	spDoc;
	
	if(!GetIEServer(hWndActive, hWndIEServer)) {
		dprintf(_T("Cannot get a pointer to IE Server from HWND %d\n"), hWndActive);
		return -1;
	}
	
	dprintf(_T("Got IE Server HWND: %d\n"), hWndIEServer);
	
	if(!GetDocPointerFromHWND(hWndIEServer, spDoc)) {
		dprintf(_T("Can't get document pointer from HWND!\n"));
		return -1;
	}
	
	
	CComBSTR bstrTempURL;
	spDoc->get_url((BSTR*)&(bstrTempURL.m_str));
	
	int index = 0;
	
	if(exists(bstrTempURL) == false) {
		dprintf(_T("WRITE: Cant find URL in list, exiting...\n"));
		return -1;
	}
	
	if(!processDocument(spDoc, WRITE)) {
		dprintf(_T("ProcessDocument() failed!\n"));
		return -1;
	}
	
	return 0;
}


LRESULT CFCWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	dprintf(_T("WM_CLOSE\n"));
	PostQuitMessage(0);
	return 0;
}



// HotKey message handler
LRESULT CFCWindow::OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	dprintf(_T("WM_HOTKEY\n"));
	UINT id = wParam;
	
	// Dispatch the message to the handlers
	if(id == m_uHotKeyRead) {
		dprintf(_T("READ event\n"));
		ReadForm();
	}
	else if(id == m_uHotKeyWrite)
	{
		dprintf(_T("WRITE event\n"));
		WriteForm();
	}
	
	return 0;
}


// Set the hot keys for form read and write
int CFCWindow::SetAppHotKeys(UINT& KeyRead, UINT& KeyWrite)
{	
	KeyRead = GlobalAddAtom("FormSnoop Read");
	
	BOOL status = ::RegisterHotKey(
		(HWND)m_hWnd,
		KeyRead,            
		MOD_WIN,
		'A'            
		);
	
	if(status == FALSE) {
		dprintf(_T("Can't register hotkey, error = %d\n"), GetLastError());
		return -1;
	}
	else
	{
		dprintf(_T("Read Hotkey registered, atom = %d\n"), KeyRead);
		m_uHotKeyRead = KeyRead;
	}
	
	KeyWrite = GlobalAddAtom("FormSnoop Write");
	
	status = ::RegisterHotKey(
		(HWND)m_hWnd,
		KeyWrite,            
		MOD_WIN,
		'S'            
		);
	
	if(status == FALSE) {
		dprintf(_T("Can't register hotkey, error = %d\n"), GetLastError());
		return -1;
	}
	else 
	{
		dprintf(_T("Write Hotkey registered, atom = %d\n"), KeyWrite);
		m_uHotKeyWrite = KeyWrite;
	}
	
	
	// Lets create the tray icon here as well, since we know
	// that the window handle is valid at this point...
	// (Should really have a separate Init() method
	
	initTrayIcon();
	return 0;
	
}

// Unmap the app hot keys
int CFCWindow::RemoveHotKeys(UINT& KeyRead, UINT& KeyWrite)
{
	dprintf(_T("Unregistering hotkey, ID=%d\n"), KeyRead);
	
	BOOL status = UnregisterHotKey(m_hWnd, KeyRead);
	if(status == FALSE) {
		dprintf(_T("Can't unregister hotkey, error=%d\n"), GetLastError());
		return -1;
	}
	
	dprintf(_T("Unregistering hotkey, ID=%d\n"), KeyWrite);
	
	status = UnregisterHotKey(m_hWnd, KeyWrite);
	if(status == FALSE) {
		dprintf(_T("Can't unregister hotkey, error=%d\n"), GetLastError());
		return -1;
	}
	
	return 0;
}


#endif