#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer()
{
	static bool raw_input_initialized = false;
	if (!raw_input_initialized)
	{
		raw_input_initialized = true;

		RAWINPUTDEVICE rid = { 0 };
		rid.usUsagePage = 0x01; //Mouse move raw
		rid.usUsage = 0x02;		

		if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
		{
			Debug::Log(GetLastError(), L"�ο� ��ǲ ������ ���� �Ͽ����ϴ�.");
			exit(-1);
		}
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT WindowContainer::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) return true;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		if (keyboard.IsKeysAutoRepeat())
		{
			keyboard.OnKeyPressed(key);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.OnKeyPressed(key);
			}
		}
		return 0;
	}		
	case WM_KEYUP:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		keyboard.OnKeyReleased(key);
		return 0;
	}		
	case WM_CHAR:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		if (keyboard.IsCharsAutoRepeat())
		{
			keyboard.OnChar(key);
		}
		else
		{
			const bool wasPressed = lParam & 0x40000000;
			if (!wasPressed)
			{
				keyboard.OnChar(key);
			}
		}
		return 0;
	}		
	case WM_MOUSEMOVE:
	{
		mouse.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_LBUTTONDOWN:		
	{
		mouse.OnLeftPressed(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_LBUTTONUP:
	{
		mouse.OnLeftReleased(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		mouse.OnRightPressed(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_RBUTTONUP:
	{
		mouse.OnRightReleased(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		mouse.OnMiddlePressed(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_MBUTTONUP:
	{
		mouse.OnMiddleReleased(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mouse.OnWheelUp(LOWORD(lParam), HIWORD(lParam));
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			mouse.OnWheelDown(LOWORD(lParam), HIWORD(lParam));
		}
		return 0;
	}
	case WM_INPUT:
	{
		UINT size = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
		if (size > 0)
		{
			//shared_ptr; ��������, ���۷��� ī��Ʈ�� �����ȴ�. �����ϰԵǸ� ���۷��� ī��Ʈ�� �����ǰ�, ������� ���۷��� ī��Ʈ�� ���� �Ѵ�. ���۷��� ī��Ʈ�� 0�� �Ǹ� �ν��Ͻ��� ���� �ȴ�.
			//weak_ptr; ��������, ���۷��� ī��Ʈ���� ���� �ȴ�. (������ �ϵ� �����ڰ� ������� ���۷��� ī��Ʈ�� ������ ���� ��Ű�� �ʴ´�.)
			//unique_ptr; ���ϳ��� �����ڸ� ��� �Ѵ�.		
			
			unique_ptr<BYTE[]> rawdata = make_unique<BYTE[]>(size);			
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &size, sizeof(RAWINPUTHEADER)) == size)
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse.OnMouseMoveRaw(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}    
}
