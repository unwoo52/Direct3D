#include "Keyboard.h"

Keyboard::Keyboard()
{
    for (int i = 0; i < 256; ++i)
    {
        keyStates[i] = false;
    }
}

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
    return keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty()
{
    return keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty()
{
    return charBuffer.empty();
}

KeyboardEvent Keyboard::ReadKey()
{
    if (!keyBuffer.empty())
    {
        KeyboardEvent e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }    
    return KeyboardEvent();
}

unsigned char Keyboard::ReadChar()
{
    if (!charBuffer.empty())
    {
        unsigned char c = charBuffer.front();
        charBuffer.pop();
        return c;
    }
    return 0u;
}

void Keyboard::OnKeyPressed(const unsigned char key)
{
    keyStates[key] = true;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void Keyboard::OnKeyReleased(const unsigned char key)
{
    keyStates[key] = false;
    keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void Keyboard::OnChar(const unsigned char key)
{
    charBuffer.push(key);
}

void Keyboard::EnableAutoRepeatKeys()
{
    autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
    autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars()
{
    autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars()
{
    autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
    return autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat()
{
    return autoRepeatChars;
}
