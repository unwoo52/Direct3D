#include "DebugLog.h"
#include <comdef.h>

void Debug::Log(wstring msg)
{
	wstring error = L"Error: " + msg;
	MessageBox(NULL, error.c_str(), L"Error", MB_ICONERROR);
}

void Debug::Log(HRESULT hr, wstring msg)
{
	_com_error error(hr);
	wstring errMsg = L"Error: " + msg + L"\n" + error.ErrorMessage();
	MessageBox(NULL, errMsg.c_str(), L"Error", MB_ICONERROR);
}
