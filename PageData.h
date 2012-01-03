#ifndef PAGEDATA_H
#define PAGEDATA_H

#include <atlbase.h>
#include <atlconv.h>
#include "List.h"
#include "ListIterator.h"
#include "serializable.h"
#include "ReverseListIterator.h"
#include <tchar.h>
#include <fstream.h>

struct FormElement : public Serializable
{
	// TODO: Should we be using Append() here?
	FormElement(CComBSTR& Type, CComBSTR& Value) { type.Append(Type); value.Append(Value); }
	FormElement() { }
	FormElement(const FormElement& rhs) { this->type = rhs.type ; this->value = rhs.value; }

	CComBSTR type;
	CComBSTR value;

	int serialize(ofstream& archive) { 
		USES_CONVERSION;

		int		memberSize;
		TCHAR	tBuf[MAX_PATH];

		bytesWritten  = 0;

		// Write type BSTR value
		_tcscpy(tBuf, W2T(type.m_str));
		memberSize = _tcslen(tBuf) + 1;
		dprintf(_T("FormElement::SERIALIZE: Writing %d to stream...\n"), memberSize);
		archive.write(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesWritten += sizeof(memberSize);
		dprintf(_T("FormElement::SERIALIZE: Writing %s to stream...\n"), tBuf);
		archive.write(reinterpret_cast<char*>(tBuf), memberSize);
		bytesWritten += memberSize;

		// Write value BSTR value
		_tcscpy(tBuf, _T(""));
		_tcscpy(tBuf, W2T(value.m_str));
		memberSize = _tcslen(tBuf) + 1;
		dprintf(_T("FormElement::SERIALIZE: Writing %d to stream...\n"), memberSize);
		archive.write(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesWritten += sizeof(memberSize);
		dprintf(_T("FormElement::SERIALIZE: Writing %s to stream...\n"), tBuf);
		archive.write(reinterpret_cast<char*>(tBuf), memberSize);
		bytesWritten += memberSize;

		dprintf(_T("FormElement::SERIALIZE: %d bytes written to stream\n"), bytesWritten);

		return 0;
	}
	
	int deserialize(ifstream& archive) { 

		int		memberSize;
		TCHAR	tBuf[MAX_PATH];

		bytesRead = 0;

		// read in type BSTR
		dprintf(_T("FormElement::DESERIALIZE: reading %d bytes from stream\n"), sizeof(memberSize));
		archive.read(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesRead += sizeof(memberSize);
		dprintf(_T("FormElement::DESERIALIZE: reading %d bytes from stream\n"), memberSize);
		archive.read(reinterpret_cast<char*>(tBuf), memberSize);
		type = tBuf;
		_tcscpy(tBuf, _T(""));

		// read in value BSTR
		dprintf(_T("FormElement::DESERIALIZE: reading %d bytes from stream\n"), sizeof(memberSize));
		archive.read(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesRead += sizeof(memberSize);
		dprintf(_T("FormElement::DESERIALIZE: reading %d bytes from stream\n"), memberSize);
		archive.read(reinterpret_cast<char*>(tBuf), memberSize);
		value = tBuf;
		_tcscpy(tBuf, _T(""));

		dprintf("FormElement::DESERIALIZE: %d bytes read from stream\n", bytesRead);
		
		return 0;
	} 
};


struct PageData : public Serializable
{
	PageData()
	{
		elements = new List<FormElement*>();
	}

	~PageData()
	{
		if(elements)
			delete elements;
	}

	int serialize(ofstream& archive) { 
		// A PageData structure contains other subobjects. So we need to serialize
		// details about this object, and also the number of subobjects that we 
		// are containing, then write them to a stream.

		USES_CONVERSION;
		
		// TODO: What if list->isEmpty()?
		int		nElemsToWrite = elements->getCount();
		int		memberSize = 0;
		TCHAR	tBuf[MAX_PATH];
		bytesWritten = 0;

		// Write URL BSTR
		_tcscpy(tBuf, W2T(URL.m_str));
		memberSize = _tcslen(tBuf) + 1;
		dprintf(_T("PageData::SERIALIZE: Writing %d to stream...\n"), memberSize);
		archive.write(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesWritten += sizeof(memberSize);
		dprintf(_T("SERIALIZE: Writing %s to stream...\n"), tBuf);
		archive.write(reinterpret_cast<char*>(tBuf), memberSize);
		bytesWritten += memberSize;

		// Write the number of FormElement elements
		dprintf(_T("PageData::SERIALIZE: Writing % bytes\n"), sizeof(nElemsToWrite));
		archive.write(reinterpret_cast<char*>(&nElemsToWrite), sizeof(nElemsToWrite));
		bytesWritten += sizeof(nElemsToWrite);

		dprintf(_T("PageData::SERIALIZE: Serializing list of subelements...\n"));
		ReverseListIterator<FormElement*> iter(*elements);
		
		// Serialize the sublist of form elements
		for(iter.reset(); !iter; ++iter)
		{
			dprintf(_T("PageData::SERIALIZE::current element: %x\n"), iter());
			iter()->serialize(archive);
		}
		
		dprintf(_T("PageData::SERIALIZE: %d bytes written...\n"), bytesWritten);
		return 0; 
	}


	int deserialize(ifstream& archive) { 

		int		memberSize = 0;
		TCHAR	tBuf[MAX_PATH];

		bytesRead = 0;

		// First read in the URL BSTR
		dprintf(_T("PageData::DESERIALIZE: reading %d bytes from stream\n"), sizeof(memberSize));
		archive.read(reinterpret_cast<char*>(&memberSize), sizeof(memberSize));
		bytesRead += sizeof(memberSize);
		dprintf(_T("PageData::DESERIALIZE: reading %d bytes from stream\n"), memberSize);
		archive.read(reinterpret_cast<char*>(tBuf), memberSize);
		URL = tBuf;

		// Now read in individual form elements
		int nElemsToRead = 0;
		dprintf(_T("PageData::DESERIALIZE: reading %d bytes from stream\n"), sizeof(nElemsToRead));
		archive.read(reinterpret_cast<char*>(&nElemsToRead), sizeof(nElemsToRead));
		bytesRead += sizeof(nElemsToRead);
		
		// Empty the existing list, if any
		elements->empty();

		for(int i = 0; i < nElemsToRead; ++i) {
			FormElement frmElem;
			frmElem.deserialize(archive);
			elements->insert(new FormElement(frmElem));
		}

		dprintf(_T("PageData::DESERIALIZE::Read %d bytes from stream\n"), bytesRead);

		return 0;
	}
	
	CComBSTR			URL;
	List<FormElement*>*	elements;

};


#endif
