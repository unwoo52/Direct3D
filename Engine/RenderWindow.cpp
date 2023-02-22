#include "WindowContainer.h"

LRESULT CALLBACK HandleMessageRedirect(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    default:
    {
        WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
    }
    }
}

LRESULT CALLBACK HandleMessageSetup(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
    {        
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
        if (pWindow == nullptr)
        {
            Debug::Log(L"윈도우 생성에 실패 하였습니다.");
            exit(-1);
        }
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMessageRedirect));
        return pWindow->WindowProc(hWnd, uMsg, wParam, lParam);
    }        
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void RenderWindow::RegisterWindowClass()
{
    WNDCLASSEX wc = { 0 };
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = HandleMessageSetup;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = winClass.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);
    RegisterClassEx(&wc);
}

bool RenderWindow::Initialize(WindowContainer* pWindow, HINSTANCE hInst, wstring title, wstring winClass, int w, int h)
{
    this->hInst = hInst;
    width = w;
    height = h;
    winTitle = title;
    this->winClass = winClass;
    RegisterWindowClass();

    RECT wr = { 0 };
    wr.left = 100;
    wr.top = 100;
    wr.right = width + wr.left;
    wr.bottom = height + wr.top;

    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

    handle = CreateWindowEx(0,
        winClass.c_str(), winTitle.c_str(),
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        wr.left, wr.top, // window position
        wr.right - wr.left, wr.bottom - wr.top, //window size
        NULL, NULL, hInst, 
        pWindow); // lParam to Create window

    if (handle == NULL)
    {
        //Error
        Debug::Log(GetLastError(), L"윈도우 생성에 실패 하였습니다.");
        return false;
    }

    ShowWindow(handle, SW_SHOW);
    SetForegroundWindow(handle);
    SetFocus(handle);

    return true;
}

bool RenderWindow::ProcessMessages()
{
    MSG msg = { 0 };
    while (PeekMessage(&msg, handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_NULL)
    {
        if (!IsWindow(handle))
        {
            handle = NULL;
            UnregisterClass(winClass.c_str(), hInst);
            return false;
        }
    }
    return true;
}

HWND RenderWindow::GetHandle() const
{
    return handle;
}

RenderWindow::~RenderWindow()
{
    if (handle != NULL)
    {
        UnregisterClass(winClass.c_str(), hInst);
        DestroyWindow(handle);
    }
}
