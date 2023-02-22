#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInst, wstring title, wstring winClass, int w, int h)
{    
    if (!window.Initialize(this, hInst, title, winClass, w, h)) return false;
    if (!gfx.Initialize(window.GetHandle(), w, h)) return false;
    return true;
}

bool Engine::ProcessMessages()
{
    return window.ProcessMessages();
}

void Engine::Update()
{
    time.UdateDeltaTime();    

    while (!keyboard.KeyBufferIsEmpty())
    {
        KeyboardEvent ke = keyboard.ReadKey();        
    }

    while (!keyboard.CharBufferIsEmpty())
    {
        unsigned char key = keyboard.ReadChar();        
    }

    while (!mouse.EventBufferIsEmpty())
    {
        MouseEvent me = mouse.ReadEvent();
        if (mouse.IsRightDown() && me.GetType() == MouseEvent::EventType::MoveRaw)
        {
            gfx.camera.GetTransform()->Rotate(me.GetPosY() * 0.01f, me.GetPosX() * 0.01f, 0);
        }
    }

    const float camSpeed = 1.0f;
    if (keyboard.KeyIsPressed('W'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetForward() * camSpeed * Time::GetDeltaTime());
    }
    if (keyboard.KeyIsPressed('S'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetBackward() * camSpeed * Time::GetDeltaTime());
    }
    if (keyboard.KeyIsPressed('A'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetLeft() * camSpeed * Time::GetDeltaTime());
    }
    if (keyboard.KeyIsPressed('D'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetRight() * camSpeed * Time::GetDeltaTime());
    }
    if (keyboard.KeyIsPressed('Q'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetUp() * camSpeed * Time::GetDeltaTime());
    }
    if (keyboard.KeyIsPressed('E'))
    {
        gfx.camera.GetTransform()->Translate(gfx.camera.GetTransform()->GetDown() * camSpeed * Time::GetDeltaTime());
    }
}

void Engine::Render()
{
    gfx.RenderFrame();
}
