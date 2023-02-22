#include "Mouse.h"

void Mouse::OnLeftPressed(int x, int y)
{
    isLeftDown = true;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void Mouse::OnLeftReleased(int x, int y)
{
    isLeftDown = false;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void Mouse::OnRightPressed(int x, int y)
{
    isRightDown = true;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void Mouse::OnRightReleased(int x, int y)
{
    isRightDown = false;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void Mouse::OnMiddlePressed(int x, int y)
{
    isMiddleDown = true;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void Mouse::OnMiddleReleased(int x, int y)
{
    isMiddleDown = false;
    eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void Mouse::OnWheelUp(int x, int y)
{
    eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void Mouse::OnWheelDown(int x, int y)
{
    eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void Mouse::OnMouseMove(int x, int y)
{
    point = { x, y };
    eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void Mouse::OnMouseMoveRaw(int x, int y)
{
    eventBuffer.push(MouseEvent(MouseEvent::EventType::MoveRaw, x, y));
}

bool Mouse::IsLeftDown()
{
    return isLeftDown;
}

bool Mouse::IsRightDown()
{
    return isRightDown;
}

bool Mouse::IsMiddleDown()
{
    return isMiddleDown;
}

int Mouse::GetPosX()
{
    return point.x;
}

int Mouse::GetPosY()
{
    return point.y;
}

MousePoint Mouse::GetPos()
{
    return point;
}

bool Mouse::EventBufferIsEmpty()
{
    return eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent()
{
    if (!eventBuffer.empty())
    {
        MouseEvent e = eventBuffer.front();
        eventBuffer.pop();
        return e;
    }
    return MouseEvent();
}
