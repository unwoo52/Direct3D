#pragma once
#include <string>
#include <Windows.h>
using namespace std;
class Debug
{
public:
	static void Log(wstring msg);
	static void Log(HRESULT hr, wstring msg);
};

