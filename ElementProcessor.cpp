#include "stdafx.h"
#include "ElementProcessor.h"
#include "Utilities.h"



int ElementProcessor::fillVector()
{
	vecInterfaces.push_back(MSHTML::IHTMLButtonElement);
	vecInterfaces.push_back(MSHTML::IHTMLInputButtonElement);
	//vecInterfaces.push_back(MSHTML::IHTMLInputElement);
	vecInterfaces.push_back(MSHTML::IHTMLInputFileElement);
	vecInterfaces.push_back(MSHTML::IHTMLInputHiddenElement);
	vecInterfaces.push_back(MSHTML::IHTMLInputTextElement);
}


template<class T>
int ElementProcessor::CreateElement(T Interface)
{
	typedef _com_ptr_t<_com_IIID<MSHTML::IHTMLElement,&__uuidof(MSHTML::IHTMLElement)> > HTMLElemPtr;
	HTMLElemPtr ptr(spformElem);
	ptr->get_id((BSTR*)&txtVal);
	_tprintf(_T("id: %ls"), (const wchar_t*)txtVal);
	
}
