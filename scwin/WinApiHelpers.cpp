#include "pch.h"
#include "WinApiHelpers.h"
#include <strsafe.h>

String ^ scwin::WinApiHelpers::GetErrorMessage(DWORD lastErrorCode)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		lastErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);	
	auto msg = ref new String((LPTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);

	return msg;
}
