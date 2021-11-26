#pragma once
#include "math.h"
#include <string>
#include <windows.h>

#include <iostream>
#include <fstream>
#include <sstream>

class Texture
{
public:
	UINT m_width, m_height;
	Color** m_buffer;

public:
	Texture() = default;
	Texture(const UINT& width, const UINT& height);
	~Texture();
	Texture(const Texture& rhs);
	Texture& operator=(const Texture& rhs);

	void setBuffer(const UINT& x, const UINT& y, const Color& color);
	Color sampleTex(const float& u, const float& v);

private:
	Texture loadTexture(std::wstring filePath);
};

