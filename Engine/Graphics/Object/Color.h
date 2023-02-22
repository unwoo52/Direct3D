#pragma once
typedef unsigned char BYTE;

class ColorClass
{
	union
	{
		BYTE rgba[4];
		unsigned int color;
	};
public:
	ColorClass();
	ColorClass(unsigned int val);
	ColorClass(BYTE r, BYTE g, BYTE b);
	ColorClass(BYTE r, BYTE g, BYTE b, BYTE a);
	ColorClass(const ColorClass& src);

	ColorClass& operator=(const ColorClass& src);
	bool operator==(const ColorClass& rhs) const;
	bool operator!=(const ColorClass& rhs) const;

	constexpr BYTE GetR() const;
	constexpr BYTE GetG() const;
	constexpr BYTE GetB() const;
	constexpr BYTE GetA() const;

	void SetR(BYTE r);
	void SetG(BYTE g);
	void SetB(BYTE b);
	void SetA(BYTE a);
};

namespace COLORS
{
	const ColorClass UnloadedTextureColor(100, 100, 100);
	const ColorClass UnhandledTextureColor(250, 0, 0);
}

