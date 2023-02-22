#pragma once
#include "RenderWindow.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Graphics/Graphics.h"

class WindowContainer
{
protected:
	RenderWindow window;
	Keyboard keyboard;
	Mouse mouse;
	Graphics gfx;
public:
	WindowContainer();
	LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

