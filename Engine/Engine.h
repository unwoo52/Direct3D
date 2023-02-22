#pragma once
#include "WindowContainer.h"
#include "Time.h"

class Engine : public WindowContainer
{
	Time time;
public:
	bool Initialize(HINSTANCE hInst, wstring title, wstring winClass, int w, int h);
	bool ProcessMessages();
	void Update();
	void Render();
};

