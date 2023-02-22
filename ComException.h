#pragma once
#include <comdef.h>
#include "Engine/DebugLog.h"

#define COM_ERROR_IF_FAILED(hr,msg) if(FAILED(hr)) throw ComException(hr,msg,__FILE__,__FUNCTION__,__LINE__)

class ComException
{
	wstring errMsg;
public:
	ComException(HRESULT hr, const wstring& msg, const string& file, const string& function, int line)
	{
		_com_error error(hr);
		errMsg = L"Message: " + msg + L"\n";
		errMsg += error.ErrorMessage();
		errMsg += L"\nFile: " + wstring(file.begin(), file.end());
		errMsg += L"\nFunction: " + wstring(function.begin(), function.end());
		errMsg += L"\nLine: " + to_wstring(line);
	}

	const wchar_t* what() const
	{
		return errMsg.c_str();
	}
};