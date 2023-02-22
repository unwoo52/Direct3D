#include "Color.h"

ColorClass::ColorClass() : color(0)
{
}

ColorClass::ColorClass(unsigned int val) : color(val)
{
}

ColorClass::ColorClass(BYTE r, BYTE g, BYTE b) : ColorClass(r, g, b, 255)
{
}

ColorClass::ColorClass(BYTE r, BYTE g, BYTE b, BYTE a)
{
	rgba[0] = r;
	rgba[1] = g;
	rgba[2] = b;
	rgba[3] = a;
}

ColorClass::ColorClass(const ColorClass& src) : color(src.color)
{
}

ColorClass& ColorClass::operator=(const ColorClass& src)
{
	color = src.color;
	return *this;
}

bool ColorClass::operator==(const ColorClass& rhs) const
{
	return color == rhs.color;
}

bool ColorClass::operator!=(const ColorClass& rhs) const
{
	return !(color == rhs.color);
}

constexpr BYTE ColorClass::GetR() const
{
	return rgba[0];
}

constexpr BYTE ColorClass::GetG() const
{
	return rgba[1];
}

constexpr BYTE ColorClass::GetB() const
{
	return rgba[2];
}

constexpr BYTE ColorClass::GetA() const
{
	return rgba[3];
}

void ColorClass::SetR(BYTE r)
{
	rgba[0] = r;
}

void ColorClass::SetG(BYTE g)
{
	rgba[1] = g;
}

void ColorClass::SetB(BYTE b)
{
	rgba[2] = b;
}

void ColorClass::SetA(BYTE a)
{
	rgba[3] = a;
}
