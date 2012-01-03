#if _MSC_VER > 1000
#pragma once
#endif 

#ifndef UTILITIES_H
#define UTILITIES_H

void dprintf(const char* format, ...);

bool GetFormattedError(DWORD lastError, char* ErrBuffer)
{
	BYTE	width = 0;
	DWORD	flags;
	
	flags |= FORMAT_MESSAGE_MAX_WIDTH_MASK & width;
	flags |= FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	
	DWORD status = FormatMessage(
		flags,
		NULL,
		lastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)ErrBuffer,
		(DWORD)MAX_PATH,
		NULL
		);
	
	if(status == 0) {
		dprintf("FormatMessage() failed: %x.\n", GetLastError());
		return false;
	}
	
	return true;
}


#endif