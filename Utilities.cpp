#include "stdafx.h"

void dprintf(const char* format, ...)
{
#ifdef _DEBUG
	
	char errBuffer[256];
	
	va_list args;
	
	va_start(args, format);
	vsprintf(errBuffer, format, args);
	va_end(args);
	
	OutputDebugString(errBuffer);
	
#endif 
}
