#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPWSTR lpCmd, _In_ int nCmd)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		Debug::Log(hr, L"CoInitialize ½ÇÆÐ");
		return -1;
	}

	Engine engine;
	if (engine.Initialize(hInst, L"DirectX11", L"StudyDX11", 800, 600))
	{
		while (engine.ProcessMessages())
		{
			engine.Update();
			engine.Render();
		}
	}
	CoUninitialize();
	return 0;
}