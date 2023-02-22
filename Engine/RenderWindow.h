#pragma once
#include "../ComException.h"

class WindowContainer;

class RenderWindow
{
	HWND handle = nullptr;
	HINSTANCE hInst = nullptr;
	wstring winTitle = L"";
	wstring winClass = L"";
	int width = 0;
	int height = 0;
	void RegisterWindowClass();
public:
	bool Initialize(WindowContainer* pWindow, HINSTANCE hInst, wstring title, wstring winClass, int w, int h);
	bool ProcessMessages();
	HWND GetHandle() const;
	~RenderWindow();
};

