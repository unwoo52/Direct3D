#include "MouseEvent.h"

MouseEvent::MouseEvent() : type(EventType::Invalid), point({0,0})
{
}

MouseEvent::MouseEvent(const EventType type, const int x, const int y) : type(type), point({x,y})
{
}

bool MouseEvent::IsValid() const
{
    return type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const
{
    return type;
}

MousePoint MouseEvent::GetPos() const
{
    return point;
}

int MouseEvent::GetPosX() const
{
    return point.x;
}

int MouseEvent::GetPosY() const
{
    return point.y;
}
