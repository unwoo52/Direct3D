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
			Debug::Log(GetLastError(), L"로우 인풋 설정에 실패 하였습니다.");
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
			//shared_ptr; 직접참조, 레퍼런스 카운트로 관리된다. 참조하게되면 레퍼런스 카운트가 증가되고, 사라지면 레퍼런스 카운트가 감소 한다. 레퍼런스 카운트가 0이 되면 인스턴스가 삭제 된다.
			//weak_ptr; 간접참조, 레퍼런스 카운트에서 제외 된다. (참조를 하든 참조자가 사라지든 레퍼런스 카운트를 증가나 감소 시키지 않는다.)
			//unique_ptr; 단하나의 참조자만 허용 한다.		
			
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
